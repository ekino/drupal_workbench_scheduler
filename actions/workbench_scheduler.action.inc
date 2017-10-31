<?php

/**
 * Workbench scheduler actions form.
 * @param $options
 * @return array
 */
function workbench_scheduler_schedules_action_form($options) {
  // Does the user have permission to set schedules?
  $user_permission = workbench_scheduler_schedule_access('set');
  // And are there any schedules for this node type?
  $form = array();
  $types = node_type_get_types();

  // Get all schedules from all content types.
  $cts_schedules = workbench_scheduler_type_schedules_load(array_keys($types));
  foreach ($cts_schedules as $ct => $schedules) {
    foreach ($schedules  as $sid => $schedule) {
      $type_schedules[$sid] = $schedule;
    }
  }

  // If user has permission and schedules exist.
  if ($user_permission && $type_schedules) {
    // Add a scheduler section.
    $form['workbench_scheduler'] = array(
      '#type' => 'fieldset',
      '#title' => t('Workbench Schedule'),
      '#access' => workbench_scheduler_schedule_access('set') || workbench_scheduler_schedule_access('view'),
      '#attached' => array(
        'js' => array(drupal_get_path('module', 'workbench_scheduler') . '/js/workbench_scheduler.js'),
      ),
    );

    // Build an options array for which schedules to choose.
    $schedule_options = array();

    // Retrieve a list of human safe moderation state names.
    $moderation_states = workbench_scheduler_state_labels();

    // Loading transitions.
    $transitions = workbench_moderation_transitions();

    // Add each type schedule to the options array.
    foreach ($type_schedules as $schedule) {
      if (workbench_scheduler_schedule_access('set', $schedule)) {
        // Get transition states.
        if (!empty($schedule->transition)) {
          foreach ($transitions as $transition) {
            if ($transition->id == $schedule->transition) {
              break;
            }
          }
        }

        $schedule_options[$schedule->sid] = array(
          'label' => $schedule->label,
          'from_name' => (!empty($moderation_states[$transition->from_name])) ? $moderation_states[$transition->from_name] : '',
          'to_name' => (!empty($moderation_states[$transition->to_name])) ? $moderation_states[$transition->to_name] : '',
        );
      }
    }

    // Add schedules to JavaScript settings.
    $form['workbench_scheduler']['#attached']['js'][] = array(
      'data' => array('workbench_scheduler' => array('schedules' => $schedule_options)),
      'type' => 'setting',
    );

    // Reset schedules checkbox.
    $form['workbench_scheduler']['workbench_scheduler_reset'] = array(
      '#type' => 'checkbox',
      '#title' => t('Reset Schedules'),
      '#description' => t("Delete any existing schedules on this node."),
    );

    // Create table select for selecting schedule.
    $form['workbench_scheduler']['workbench_scheduler_sid'] = array(
      '#type' => 'tableselect',
      '#title'  => t('Select Schedule'),
      '#prefix' => t('Select the schedule(s) to use for this node.'),
      '#options' => $schedule_options,
      '#multiple' => TRUE,
      '#header' => array(
        'label' => t('Name'),
        'from_name' => t('When content state is. . .'),
        'to_name' => t('Set content state to . . .'),
      ),
      '#attributes' => array(
        'class' => array('workbench-schedule-sid'),
      ),
    );

    foreach ($schedule_options as $sid => $schedule) {
      // Fieldset for collecting schedule dates.
      $form['workbench_scheduler']['dates'][$sid] = array(
        '#type' => 'fieldset',
        '#title' =>  $schedule['label'],
        '#states' => array(
          'visible' => array(
            ':input[name="workbench_scheduler_sid['.$sid.']"]' => array('checked' => TRUE),
          ),
        ),
      );

      $form['workbench_scheduler']['dates'][$sid]['workbench_scheduler_date']['#tree'] = TRUE;

      // Date.
      $form['workbench_scheduler']['dates'][$sid]['workbench_scheduler_date'][$sid] = array(
        '#type' => 'date_popup',
        '#date_format' => workbench_scheduler_date_format(),
        '#title' =>  t('Set content to ') . $schedule['to_name'] . t(' after . . .'),
        '#description' => t('Note: This schedule will only run when the current moderation state is') .' <strong>'. $schedule['from_name'] . '</strong>.',
      );
    }

    // Custom validation hook.
    $form['#validate'][] = 'workbench_scheduler_node_form_validate';
  }

  return $form;
}

/**
 * Workbench scheduler action submit.
 * @param $form
 * @param $form_state
 * @return mixed
 */
function workbench_scheduler_schedules_action_submit($form, &$form_state) {
  $context = array();

  if (isset($form_state['values']['workbench_scheduler_sid']))
    $context['workbench_scheduler_sid'] = $form_state['values']['workbench_scheduler_sid'];

  if (isset($form_state['values']['workbench_scheduler_date']))
    $context['workbench_scheduler_date'] = $form_state['values']['workbench_scheduler_date'];

  if (isset($form_state['values']['workbench_scheduler_reset']))
    $context['workbench_scheduler_reset'] = $form_state['values']['workbench_scheduler_reset'];

  return $context;
}

/**
 * Implements the action that is applied to the revisions.
 * @param $entity
 * @param array $context
 */
function  workbench_scheduler_schedules_action(&$entity, $context = array()) {
  // Change vid to latest.
  $latest_revision = workbench_moderation_node_current_load($entity);
  $entity->vid = $latest_revision->vid;

  // Only save schedules for content that supports them.
  $types_moderated = workbench_moderation_moderate_node_types();
  $is_moderated = (in_array($entity->type, $types_moderated)) ? TRUE : FALSE;
  $schedules = workbench_scheduler_type_schedules_load($entity->type);

  if ($is_moderated && !empty($schedules)) {
    // Reset the workbench schedule.
    if ($context['workbench_scheduler_reset']) {
      $entity->workbench_schedule = array();
      workbench_scheduler_delete_node_schedule($entity->nid, $entity->vid);
    }

    foreach ($context['workbench_scheduler_sid'] as $sid => $sid_enabled) {
      foreach ($schedules as $name => $info) {
        if ($sid_enabled && $info->sid == $sid) {

          // Format date.
          if (!empty($context['workbench_scheduler_date'][$sid])) {
            $date = strtotime($context['workbench_scheduler_date'][$sid]);
          }
          else {
            $date = '';
          }

          // Add new properties.
          $info->date = $date;
          $info->completed = 0;

          // Does this schedule already exist?
          $updated = FALSE;
          if (!empty($entity->workbench_schedule)) {
            foreach ($entity->workbench_schedule as $index => $entity_schedule) {
              // Instead of creating a new element, update the existing one.
              if ($info->sid == $entity_schedule->sid) {
                $entity->workbench_schedule[$index] = $info;
                $updated = TRUE;
              }
            }
          }

          // If schedule doesn't already exist create it.
          if (!$updated) {
            $entity->workbench_schedule[] = $info;
          }
        }
      }
    }
  }
}
