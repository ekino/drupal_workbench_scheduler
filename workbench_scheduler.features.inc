<?php
/**
 * @file
 * Features file for the workbench_scheduler module.
 */

/**
 * Implements hook_features_api().
 */
function workbench_scheduler_features_api() {
  return array(
    'workbench_scheduler' => array(
      'name' => t('Workbench Schedules'),
      'default_hook' => 'workbench_scheduler_export_schedules',
      'feature_source' => TRUE,
      'default_file' => FEATURES_DEFAULTS_INCLUDED,
      'file' => drupal_get_path('module', 'workbench_scheduler') . '/workbench_scheduler.features.inc',
    ),
  );
}

/**
 * Implements COMPONENT_features_export_options().
 *
 * Inform features about the available schedules in the database.
 */
function workbench_scheduler_features_export_options() {
  return db_select('workbench_scheduler_schedules', 'wss')
    ->fields('wss', array('name', 'name'))
    ->execute()
    ->fetchAllKeyed();
}

/**
 * Implements COMPONENT_features_export().
 *
 * Process the features export array for schedules.
 */
function workbench_scheduler_features_export($data, &$export, $module_name) {
  $export['dependencies']['workbench_scheduler'] = 'workbench_scheduler';

  foreach ($data as $component) {
    $export['features']['workbench_scheduler'][$component] = $component;
  }

  return array();
}

/**
 * Implements COMPONENT_features_export_render().
 *
 * Render workbench schedules as code.
 */
function workbench_scheduler_features_export_render($module_name, $data) {
  $items = array();
  foreach ($data as $schedule) {
    // TODO: refactor this, pass it sid. (but still save machine name in feature).
    $items[$schedule] = workbench_scheduler_machine_name_schedules_load($schedule);

    // Unset SID to prevent overridden state.
    unset($items[$schedule]->sid);
  }

  // Get transition info.
  // Include workbench moderation.
  module_load_include('module', 'workbench_moderation');
  $transitions = workbench_moderation_transitions();
  foreach ($items as $key => $schedule) {

    // Get transition state info.
    if (!empty($schedule->transition)) {
      foreach ($transitions as $transition) {
        if ($transition->id == $schedule->transition) {
          $schedule->from_name = $transition->from_name;
          $schedule->to_name = $transition->to_name;
          break;
        }
      }
    }
  }

  foreach ($items as $key => $schedule) {
    // Unset SID to prevent overridden state.
    unset($items[$key]->sid);

    // Unset transition ID because this is mutable between databases.
    unset($items[$key]->transition);
  }

  $code = "  \$items = " . features_var_export($items, '  ') . ";\n";
  $code .= '  return $items;';

  return array('workbench_scheduler_export_schedules' => $code);
}


/**
 * Implements COMPONENT_features_revert().
 */
function workbench_scheduler_features_revert($module) {
  workbench_scheduler_features_rebuild($module);
}

/**
 * Implements COMPONENT_features_enable_feature().
 */
function workbench_scheduler_features_enable_feature($module) {
  workbench_scheduler_features_rebuild($module);
}

/**
 * Implements COMPONENT_features_rebuild().
 *
 * Store each exported schedule in the database.
 */
function workbench_scheduler_features_rebuild($module) {
  if ($defaults = features_get_default('workbench_scheduler', $module)) {
    foreach ($defaults as $schedule) {

      // Find the transition from from_name and to_name.
      module_load_include('module', 'workbench_moderation');
      $transitions = workbench_moderation_transitions();
      foreach($transitions as $transition) {
        if ($transition->from_name == $schedule['from_name'] && $transition->to_name == $schedule['to_name']) {
          unset($schedule['from_name']);
          unset($schedule['to_name']);
          $schedule['transition'] = $transition->id;
          break;
        }
      }

      workbench_scheduler_save_schedule($schedule['name'], $schedule);
    }
  }
  drupal_static_reset('workbench_scheduler_schedules_load');
}
