<?php
/**
 * @file
 * Provides admin functions for Workbench Scheduler.
 */

/**
 * Display a table of workbench schedule for administration.
 *
 * @return string
 *   themed output.
 */
function workbench_scheduler_admin_page() {
  // Build a table to show the different schedules.
  $headers = array(
    array('data' => t('Name')),
    array('data' => t('Machine Name')),
    array('data' => t('Transition')),
    array('data' => t('Content Types')),
    array('data' => t('Operations'), 'colspan' => 2),
  );

  $rows = array();
  // Retrieve any schedules that exist.
  if ($schedules = workbench_scheduler_schedules_load()) {
    // Get list of the different moderation states.
    $states = workbench_scheduler_state_labels();

    $transitions = workbench_moderation_transitions();
    // Get list of the different content types.
    $node_types = node_type_get_types();
    // Loop through the schedules to add them to the table.
    foreach ($schedules as $sid => $schedule) {

      // Load transition info.
      unset($transition);
      foreach ($transitions as $transition) {
        if ($transition->id == $schedule->transition) {
          break;
        }
      }

      // Format the content types the schedule is available for,
      // Based on number.
      $type_count = count($schedule->types);
      // More then one type?
      if ($type_count > 1) {
        $items = array();
        // Loop through each type.
        foreach ($schedule->types as $type) {
          // Display the human readable name.
          $items[] = $node_types[$type]->name;
        }

        // Format into an item list.
        $types = theme('item_list', array('items' => $items, 'type' => 'ul'));
      }
      // Have only a single type?
      elseif (count($schedule->types) == 1) {
        // Display the human readable name.
        $types = $node_types[array_pop($schedule->types)]->name;
      }
      // No types found (either deleted or not imported?).
      else {
        // Display null.
        $types = 'NULL';
      }
      // Format the row.
      $row = array(
        $schedule->label,
        $schedule->name,
        $transition->name,
        $types,
        // Link to edit the schedule.
        l(t('edit'), 'admin/config/workbench/scheduler/schedules/' . $schedule->name . '/edit'),
        // Link to delete the scheduler.
        l(t('delete'), 'admin/config/workbench/scheduler/schedules/' . $schedule->name . '/delete'),
      );
      // Add to the rows array.
      $rows[] = $row;
    }
  }
  // No schedules found.
  else {
    // Display message in first row.
    $rows[] = array(array(
        'data'    => t('No Schedules Found'),
        'colspan' => 7,
      ),
    );
  }
  // Returned the themed table.
  return theme('table', array('header' => $headers, 'rows' => $rows));
}

/**
 * Display a form for adding/editing a schedule.
 */
function workbench_scheduler_admin_edit_schedule($form, &$form_state, $name = FALSE) {
  $schedule = FALSE;
  if ($name) {
    // Attempt to load the schedule.
    if ($schedule = workbench_scheduler_machine_name_schedules_load($name)) {
      // Store in the form.
      $form['#schedule']  = $schedule;
    }
  }
  // Label field.
  $form['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Schedule Name'),
    '#required' => TRUE,
    '#description' => t('A user friendly name. Only used for admin purposes'),
    '#size' => 40,
    '#maxlength' => 127,
  );

  // Field for machine name.
  $form['name'] = array(
    '#title' => t('Machine Name'),
    '#description' => t('A machine friendly name.'),
    '#type' => 'machine_name',
    '#required' => TRUE,
    '#size' => 25,
    '#maxlength' => 25,
    '#default_value' => ($schedule ? $schedule->name : ''),
    '#machine_name' => array(
      'exists' => '_workbench_schedule_check_machine_name_exists',
      'source' => array('label'),
    ),
  );

  // Get state labels.
  $states = workbench_moderation_states();

  // Fetch all transitions (don't use states anymore).
  module_load_include('module', 'workbench_moderation');
  $transitions = workbench_moderation_transitions();

  $transition_options = array();
  // Make into an associative array.
  foreach ($transitions as $transition) {
    $transition_options[$transition->id] = array(
      'id' => $transition->id,
      'label' =>  $transition->name,
      'from_name' => $states[$transition->from_name]->label,
      'to_name' => $states[$transition->to_name]->label,
    );
  }

  // Label  and description.
  $form['transition_label'] = array(
    '#type' => 'markup',
    '#markup' => '<label>' . t('Transitions') . '</label>',
  );
  $form['transition_description'] = array(
    '#type' => 'markup',
    '#markup' => t('Select the transition to use for this schedule. Disabled transitions are being used by another schedule.'),
  );

  // Checkboxes for transitions.
  $form['transition'] = array(
    '#type' => 'tableselect',
    '#title'  => t('Transition'),
    '#options' => $transition_options,
    '#multiple' => FALSE,
    '#required' => TRUE,
    '#header' => array(
      'label' => t('Name'),
      'from_name' => t('When content state is . . .') ,
      'to_name' => t('Set content to . . .'),
    ),
    '#empty' => t('No transitions are configured with workbench moderation.'),
  );

  // Check to see if there are any schedules using this transition.
  $schedules = workbench_scheduler_schedules_load();
  if (!empty($schedules)) {
    foreach ($schedules as $schedule_obj) {
      if (empty($schedule) || $schedule_obj->sid != $schedule->sid) {
        $form['transition'][$schedule_obj->transition]['#disabled'] = TRUE;
      }
    }
  }

  // Fetch a list of content types that have moderation enabled.
  // From the workbench moderation module.
  module_load_include('module', 'workbench_moderation');
  $types = workbench_moderation_moderate_node_types();
  $info = entity_get_info('node');

  $tmp_types = array();
  // Make into an associative array.
  foreach ($types as $type) {
    $default_state = variable_get('workbench_moderation_default_state_' . $type, workbench_moderation_state_none());
    $label = $info['bundles'][$type]['label'];
    $tmp_types[$type] = array(
      'label' => $label,
      'default_state' => $states[$default_state]->label,
    );
  }
  // Store types to the form.
  $form['#types'] = $types;
  $types = $tmp_types;
  unset($tmp_types);

  $header = array(
    'label' => t('Content Type'),
    'default_state' => t('Default Moderation State'),
  );

  // Label  and description.
  $form['table_label'] = array(
    '#type' => 'markup',
    '#markup' => '<label>' . t('Content Types') . '</label>',
  );
  $form['table_description'] = array(
    '#type' => 'markup',
    '#markup' => t('Select the content types that can use this schedule.'),
  );
  // Checkboxes for content types.
  $form['types'] = array(
    '#type'           => 'tableselect',
    '#title'          => t('Content Types'),
    '#description'    => t('Select the content types that can use this schedule'),
    '#header'         => $header,
    '#options'        => $types,
    '#required'       => TRUE,
    '#empty' => t('No content types are configured with workbench moderation.'),
  );

  $form['submit'] = array(
    '#type'   => 'submit',
    '#value' => ($schedule ? t('Update') : t('Save')),
  );

  // Editing a schedule?
  if ($schedule) {
    // Add a delete button.
    $form['delete'] = array(
      '#type'   => 'submit',
      '#value'  => t('Delete'),
    );

    // Remove the machine name field.
    unset($form['name']);
    // Set default values for the other fields.
    $form['label']['#default_value']        = $schedule->label;
    $form['transition']['#default_value']   = $schedule->transition;
    $form['types']['#default_value']        = drupal_map_assoc($schedule->types);
  }
  // Return the form.
  return $form;
}

/**
 * Validation function for adding / editing a schedule.
 */
function workbench_scheduler_admin_edit_schedule_validate($form, &$form_state) {
  // Validate label.
  if (strlen(trim(strip_tags($form_state['values']['label']))) < 1) {
    form_set_error('label', t('Invalid Schedule name provided'));
  }
}

/**
 * Submit function for adding/editing a schedule.
 */
function workbench_scheduler_admin_edit_schedule_submit($form, &$form_state) {
  // Check to see if the delete button was clicked.
  if ($form_state['clicked_button']['#id'] == 'edit-delete') {
    // Redirect to the delete form.
    drupal_goto('admin/config/workbench/scheduler/schedules/' . $form['#schedule']->name . '/delete');
  }
  // Only other button is the submit button.
  else {
    // Build the array of schedule data.
    $schedule_data = array(
      'label'       => trim(strip_tags($form_state['values']['label'])),
      'transition'   => $form_state['values']['transition'],
      // Add the checked types to the schedule data.
      'types'       => array_intersect($form['#types'], $form_state['values']['types']),
    );

    // Updating an existing schedule?
    if (isset($form['#schedule'])) {
      // Fetch the machine name form the object.
      $machine_name = $form['#schedule']->name;
    }
    else {
      // Fetch the new machine name from passed values.
      $machine_name = $form_state['values']['name'];
    }

    // Attempt a save/update
    if ($result = workbench_scheduler_save_schedule($machine_name, $schedule_data)) {
      drupal_set_message(t('Schedule @status', array('@status' => ($result == 1 ? t('Saved') : t('Updated')))), 'status', FALSE);
      // Redirect back to the schedules table.
      $form_state['redirect'] = 'admin/config/workbench/scheduler/schedules';
    }
    else {
      drupal_set_message(t('Error saving schedule'), 'error', FALSE);
    }
  }
}

/**
 * Confirmation form for deleting a schedule.
 */
function workbench_scheduler_admin_delete_schedule($form, &$form_state, $name) {
  // Attempt to load the schedule.
  if ($schedule = workbench_scheduler_machine_name_schedules_load($name)) {
    // Store to form.
    $form['#schedule'] = $schedule;
    // Build confirmation form.
    return confirm_form($form,
      t('Are you sure you want to delete the schedule "@label"?', array('@label' => $schedule->label)),
      'admin/config/workbench/scheduler/schedules',
      t('This action cannot be undone')
    );
  }
  // Unable to load form, not sure what trying to delete.
  else {
    drupal_set_message(t('Invalid Schedule machine name'), 'error', FALSE);
    // Send back to schedules table.
    drupal_goto('admin/config/workbench/scheduler/schedules');
  }
}

/**
 * Submit function for deleting a schedule.
 */
function workbench_scheduler_admin_delete_schedule_submit($form, &$form_state) {
  // Fetch the schedule to delete.
  $schedule = $form['#schedule'];
  // Attempt to delete.
  if (workbench_scheduler_delete_schedules($schedule->name)) {
    drupal_set_message(t('Schedule and associated data deleted'), 'status', FALSE);
    // Go back to schedules page.
    $form_state['redirect'] = 'admin/config/workbench/scheduler/schedules';
  }
  // Unable to delete, show error message.
  else {
    drupal_set_message(t('Error deleting schedule'), 'error', FALSE);
  }
}

/**
 * Workbench scheduler admin settings page callback.
 */
function workbench_scheduler_admin_settings_page($form, &$form_state) {
  // Cron settings fieldset.
  $form['cron'] = array(
    '#type' => 'fieldset',
    '#title' => t('Cron settings'),
  );

  // Run schedule on published revisions.
  $form['cron']['workbench_scheduler_settings_published_revisions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Run schedules on published revisions.'),
    '#description' => t('By default, schedules are only executed on the latest node revision. Enable this to execute schedules on any published node revision.'),
    '#default_value' => variable_get('workbench_scheduler_settings_published_revisions', FALSE)
  );

  // Date settings fieldset.
  $form['date'] = array(
    '#type' => 'fieldset',
    '#title' => t('Date settings'),
  );

  // Configure date format.
  // We're only fetching "Short" date formats because those are the only formats
  // supported by the Date Popup Module.
  $supported_formats = system_get_date_formats('short');
  $date_formats = array();
  foreach ($supported_formats as $format => $data) {
    $date_formats[$format] = date($format);
  }

  $form['date']['workbench_scheduler_settings_date_format'] = array(
    '#type' => 'select',
    '#title' => t('Date Format'),
    '#options' => $date_formats,
    '#description' => t('By default, Workbench Scheduler uses the date format assigned to the system\'s "Short" date type. Choose a different date format here.'),
    '#default_value' => variable_get('workbench_scheduler_settings_date_format', date_format_type_format('short')),
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );

  return system_settings_form($form);
}

/**
 * Tableselect form for current schedules applied to a node.
 */
function workbench_scheduler_admin_manage_node_schedules($form, &$form_state, $node) {

  // Getting language (for date formats).
  global $language;

  // Declare schedules form array.
  $schedules = array(
    'schedule_active' => array(),
    'schedule_inactive' => array(),
  );

  // Fetch node schedule information for every revision.
  $node_schedules = workbench_scheduler_load_node_schedule($node->nid);


  // Process schedules.
  _workbench_scheduler_process_node_schedules($node_schedules);

  // Preparing results for render.
  if (!empty($node_schedules)) {
    foreach ($node_schedules as $schedule) {
      if ($schedule->active) {
        $schedule_data['schedule_active'][$schedule->vid][] = $schedule;
      }
      else {
        $schedule_data['schedule_inactive'][$schedule->vid][] = $schedule;
      }
    }
  }

  if (!empty($schedule_data)) {
    foreach ($schedule_data as $status => $status_schedules) {
      foreach ($status_schedules as $vid => $revision_schedules) {

        $items = array();
        foreach ($revision_schedules as $node_schedule) {
          // Loading full schedule data.
          $schedule = workbench_scheduler_schedules_load($node_schedule->sid);

          // Date.
          $date = t('not set');
          if (isset($node_schedule->date)) {
            $date = format_date($node_schedule->date, 'custom', workbench_scheduler_date_format(), NULL, $language->language);
          }

          // TODO: Need to change the way this looks when rendered.
          $completed = ($node_schedule->completed) ? '(' . t('Completed') . ')' : '';
          $items[] = $schedule->label . ' on ' . $date . ' ' . $completed;
        }

        // Schedule list.
        $schedule_list = array(
          'items' => $items,
        );

        // Build each row of the tableselect.
        $manage_schedules = array(
          'title' => array('data' => array('#title' => $vid)),
          'vid' => (array('data' => l($vid, 'node/' . $node->nid . '/revisions/' . $vid . '/view'), 'width' => 75)),
          'schedules' => theme('item_list', $schedule_list),
        );

        // Loading the revision.
        $revision = node_load($node->nid, $vid);
        if (workbench_scheduler_node_schedule_access('set', $revision) || workbench_scheduler_node_schedule_access('view', $revision)) {
          $manage_schedules['vid_edit'] = l(t('edit'), 'node/' . $node->nid . '/manage_schedules/' . $vid . '/edit');
        }
        $schedules[$status][$vid] = $manage_schedules;
      }
    }

    // Render Schedules.
    foreach ($schedules as $status => $schedule) {
      if (!empty($schedule)) {
        // Build the the tableselect form, with a hidden field for the nid.
        $form['nid'] = array('#type' => 'hidden', '#value' => $node->nid);

        // Fieldsets.
        if ($status == 'schedule_active') {
          $fieldset = array(
            '#type' => 'fieldset',
            '#title' => t('Active Schedule'),
            '#description' => t('Only active schedules are run.'),
          );
        }
        else {
          $fieldset = array(
            '#type' => 'fieldset',
            '#title' => t('Inactive Schedules'),
          );
        }
        // Active/Inactive fieldset.
        $form[$status . '_fieldset'] = $fieldset;

        $records = array();

        // Flattening the array.
        foreach ($schedule as $vid => $schedule_revision) {
          $records[$vid] = $schedule_revision;
        }

        // Output the table of schedules.
        $form[$status . '_fieldset']['schedules'] = array(
          '#type' => 'tableselect',
          '#title' => t('Schedules applied to this node'),
          '#validated' => TRUE,
          '#options' => $records,
          '#multiple' => TRUE,
          '#header' => array(
            'vid' => t('Revision'),
            'schedules' => t('Schedules'),
          ),
        );

        // Only edit if have permissions.
        // Loading the revision.
        $revision = node_load($node->nid, $vid);
        if (workbench_scheduler_node_schedule_access('set', $revision) || workbench_scheduler_node_schedule_access('view', $revision)) {
          $form[$status . '_fieldset']['schedules']['#header']['vid_edit'] = array(
            'data' => t('Operations'),
            'width' => 200,
          );
        }
      }
    }
    // The form submit actions.
    if (workbench_scheduler_node_schedule_access('set', $node)) {
      $form['actions'] = array('#type' => 'actions');
      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Delete selected'),
      );
    }
    return $form;
  }
  else {
    // When no schedules applied to this node, display a status message.
    drupal_set_message(t('No schedules applied to this node'), 'status', FALSE);
  }
}

/**
 * Submit function for deleting the schedule(s) set for a node's vid.
 */
function workbench_scheduler_admin_manage_node_schedules_submit($form, &$form_state) {
  $nid = $form_state['values']['nid'];
  foreach ($form_state['values']['schedules'] as $vid => $checked) {
    if ($vid == $checked) {
      workbench_scheduler_delete_node_schedule($nid, $vid);
    }
  }
}

/**
 * Form to edit the schedule for an existing node revision.
 */
function workbench_scheduler_admin_edit_revision_schedule($form, &$form_state, $node, $vid) {
  $form['nid'] = array('#type' => 'hidden', '#value' => $node->nid);
  $form['vid'] = array('#type' => 'hidden', '#value' => $vid);

  $type_schedules = workbench_scheduler_type_schedules_load($node->type);

  $moderation_states = workbench_scheduler_state_labels();

  $transitions = workbench_moderation_transitions();

  $revision = node_load($node->nid, $vid);

  // Getting language (for date formats).
  global $language;

  // Build array of data being passed to settings in javascript.
  $schedule_data = array();

  // Build an options array for which schedules to choose.
  $schedule_options = array();

  // Load workbench schedule by vid
  $node_schedules = workbench_scheduler_load_node_schedule($node->nid, $vid);

  // Setting selected schedules.
  $selected_schedules = array();
  if (!empty($node_schedules)) {
    foreach ($node_schedules as $node_schedule) {
      $selected_schedules[$node_schedule->sid] = TRUE;
    }
  }

  // Add each type schedule to the options array.
  if (!empty($type_schedules)) {
    foreach ($type_schedules as $sid => $schedule) {

      // TODO: These new permission refactoring needs testing.
      // If no set permissions check view permissions.
      if (!workbench_scheduler_node_schedule_access('set', $revision, $schedule)) {
        // If no view permissions then skip.
        if(!workbench_scheduler_node_schedule_access('view', $revision, $schedule)) {
          continue;
        }
        // If view permission but no data exists then skip again.
        elseif(workbench_scheduler_node_schedule_access('view', $revision, $schedule)
          && empty($selected_schedules[$sid])) {
          continue;
        }
      }

      // Get transition states.
      unset($transition);
      if (!empty($schedule->transition)) {
        foreach ($transitions as $transition) {
          if ($transition->id == $schedule->transition) {
            break;
          }
        }
      }

      // Final array for rendering schedule fieldsets and their dates.
      $schedule_data[] = array(
        'sid' => $schedule->sid,
        'label' => $schedule->label,
        'name' => $schedule->name,
        'transition' => (!empty($transition)) ? $transition->id : '',
        'from_name' => (!empty($transition) && !empty($moderation_states[$transition->from_name])) ? $moderation_states[$transition->from_name] : '',
        'to_name' => (!empty($transition) && !empty($moderation_states[$transition->to_name])) ? $moderation_states[$transition->to_name] : '',
      );

      // Checking permissions for setting schedules.
      // Added to schedules table.
      if (workbench_scheduler_node_schedule_access('set', $revision, $schedule)) {
        $schedule_options[$schedule->sid] = array(
          'label' => $schedule->label,
          'from_name' => (!empty($transition) && !empty($moderation_states[$transition->from_name])) ? $moderation_states[$transition->from_name] : '',
          'to_name' => (!empty($transition) && !empty($moderation_states[$transition->to_name])) ? $moderation_states[$transition->to_name] : '',
        );
      }
    }
  }

  // Add schedules to JavaScript settings.
  $form['workbench_scheduler']['#attached']['js'][] = drupal_get_path('module', 'workbench_scheduler') . '/js/workbench_scheduler.js';
  $form['workbench_scheduler']['#attached']['js'][] = array(
    'data' => array('workbench_scheduler' => array('schedules' => $schedule_data)),
    'type' => 'setting',
  );

  // Create table select for selecting schedule.
  if (!empty($schedule_options)) {
    $form['workbench_scheduler']['workbench_scheduler_sid'] = array(
      '#type' => 'tableselect',
      '#title' => t('Select Schedule'),
      '#description' => t('Select the schedule to use for this node.'),
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
  }

  foreach ($schedule_data as $schedule) {

    $sid = $schedule['sid'];

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

    // Start date.
    $form['workbench_scheduler']['dates'][$sid]['workbench_scheduler_date'][$sid] = array(
      '#type' => 'date_popup',
      '#date_format' => workbench_scheduler_date_format(),
      '#title' =>  t('Set content to ') . $schedule['to_name'] . t(' after . . .'),
      '#description' => t('Note: This schedule will only run when the current moderation state is') .' <strong>'. $schedule['from_name'] . '</strong>.',
    );

    // if user does not have set access but has view access.
    $schedule_obj = workbench_scheduler_schedules_load($schedule['sid']);
    if (!workbench_scheduler_node_schedule_access('set', $revision, $schedule_obj) && workbench_scheduler_node_schedule_access('view', $revision, $schedule_obj)) {
      unset($form['workbench_scheduler']['dates'][$sid]['#states']);
      $form['workbench_scheduler']['dates'][$sid]['workbench_scheduler_date'][$sid]['#disabled'] = TRUE;
    }

    // Have a node schedule(editing a node)? Use for defaults.
    if (!empty($node_schedules)) {
      foreach ($node_schedules as $node_schedule) {
        if ($node_schedule) {
          if ($node_schedule->sid == $sid && $node_schedule->date) {
            // Format date for field.
            $date = format_date($node_schedule->date, 'custom', DATE_FORMAT_DATETIME);
            $form['workbench_scheduler']['dates'][$sid]['workbench_scheduler_date'][$sid]['#default_value'] = $date;
            // Disable schedule after it has been run.
            if ($node_schedule->completed) {
              if (!empty($form['workbench_scheduler']['workbench_scheduler_sid'][$sid])) {
                $form['workbench_scheduler']['workbench_scheduler_sid'][$sid]['#attributes']['disabled'] = TRUE;
              }
              $form['workbench_scheduler']['dates'][$sid]['workbench_scheduler_date'][$sid]['#disabled'] = TRUE;
              $form['workbench_scheduler']['dates'][$sid]['workbench_scheduler_date'][$sid]['#description'] = t('Note: This schedule has completed and cannot be changed for this revision.');
            }
          }
        }
      }
      $form['workbench_scheduler']['workbench_scheduler_sid']['#default_value'] = $selected_schedules;
    }
  }

  if (workbench_scheduler_node_schedule_access('set', $revision)) {
    $form['actions'] = array('#type' => 'actions');
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Update'),
      '#validate' => array(
        'workbench_scheduler_node_form_validate',
      ),
    );
  }
  return $form;
}

/**
 * Submit function for editing the schedule applied to a node's revision.
 */
function workbench_scheduler_admin_edit_revision_schedule_submit($form, &$form_state) {
  // Get the node data from the form state.
  $nid = $form_state['values']['nid'];
  $vid = $form_state['values']['vid'];

  foreach ($form_state['values']['workbench_scheduler_sid'] as $sid => $sid_enabled) {

    if (!empty($form_state['values']['workbench_scheduler_date'][$sid])) {
      $date = $form_state['values']['workbench_scheduler_date'][$sid];
    }
    else {
      $date = '';
    }

    // Have a schedule selected?
    if ($sid_enabled) {
      // Retrieve schedule data.
      $schedule_data = array(
        'sid' => $sid,
        // Format to timestamp for storage.
        'date' => strtotime($date),
      );
      // Attempt to save the schedule for this node.
      if (workbench_scheduler_save_node_schedule($nid, $vid, $schedule_data)) {
        // Success.
        drupal_set_message(t('Revision schedule updated'), 'status', FALSE);
      }
      else {
        drupal_set_message(t('Error saving workbench schedule for node'), 'error', FALSE);
      }
    }
    else {
      workbench_scheduler_delete_node_schedule($nid, $vid, $sid);
    }
  }
  drupal_goto('node/' . $nid . '/manage_schedules/');
}
