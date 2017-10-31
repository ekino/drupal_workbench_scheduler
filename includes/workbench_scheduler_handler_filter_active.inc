<?php
/**
 * @file
 * Filter based on active or inactive.
 */

class workbench_scheduler_handler_filter_active extends views_handler_filter_boolean_operator {

  /**
   * Return filter options for active or inactive schedules.
   */
  function query() {
    // Getting all the current revisions of all the nodes.
    $query = db_select('workbench_moderation_node_history', 'wmnh');
    $query->fields('wmnh', array('vid'));
    $query->join('workbench_scheduler_nodes', 'workbench_scheduler_nodes', 'workbench_scheduler_nodes.vid = wmnh.vid AND wmnh.is_current = 1');
    $relevant_vids = $query->execute()->fetchCol('vid');

    // If active, find all published revisions too.
    if (variable_get('workbench_scheduler_settings_published_revisions', FALSE)) {
      $query = db_select('workbench_moderation_node_history', 'wmnh');
      $query->fields('wmnh', array('vid'));
      $query->join('workbench_scheduler_nodes', 'workbench_scheduler_nodes', 'workbench_scheduler_nodes.vid = wmnh.vid');
      $query->condition('wmnh.published', 1);
      $published_vids = $query->execute()->fetchCol('vid');
      if (!empty($published_vids)) {
        $relevant_vids = array_merge($relevant_vids, $published_vids);
      }
    }

    // Filter by current vids.
    if (!empty($relevant_vids)) {
      $operator = ($this->value ? "IN" : "NOT IN");
      $this->ensure_my_table();
      $this->query->add_where($this->options['group'], "$this->table_alias.vid", $relevant_vids, $operator);
    }
  }
}
