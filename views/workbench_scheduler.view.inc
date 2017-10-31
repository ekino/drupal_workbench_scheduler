<?php
/**
 * @file workbench_scheduler.view.inc
 * Default view. Provides a tab on the Workbench, "Scheduled Content" which lists
 * all scheduled content.
 */

$view = new view();
$view->name = 'workbench_scheduler';
$view->description = 'Lists content by schedule.';
$view->tag = 'Workbench Scheduler';
$view->base_table = 'node_revision';
$view->human_name = 'Workbench Scheduler: Content';
$view->core = 7;
$view->api_version = '3.0';
$view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */

/* Display: Master */
$handler = $view->new_display('default', 'Master', 'default');
$handler->display->display_options['title'] = 'Scheduled Content';
$handler->display->display_options['use_more_always'] = FALSE;
$handler->display->display_options['access']['type'] = 'perm';
$handler->display->display_options['access']['perm'] = 'use workbench_scheduler scheduled content tab';
$handler->display->display_options['cache']['type'] = 'none';
$handler->display->display_options['query']['type'] = 'views_query';
$handler->display->display_options['exposed_form']['type'] = 'basic';
$handler->display->display_options['pager']['type'] = 'none';
$handler->display->display_options['pager']['options']['offset'] = '0';
$handler->display->display_options['style_plugin'] = 'table';
$handler->display->display_options['style_options']['grouping'] = array(
  0 => array(
    'field' => 'label',
    'rendered' => 1,
    'rendered_strip' => 0,
  ),
);
$handler->display->display_options['style_options']['columns'] = array(
  'nid' => 'nid',
  'label' => 'label',
  'vid' => 'vid',
  'title' => 'title',
  'type' => 'type',
  'completed' => 'completed',
  'nothing' => 'nothing',
);
$handler->display->display_options['style_options']['default'] = '-1';
$handler->display->display_options['style_options']['info'] = array(
  'nid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'label' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'vid' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'title' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'type' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'completed' => array(
    'sortable' => 0,
    'default_sort_order' => 'asc',
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
  'nothing' => array(
    'align' => '',
    'separator' => '',
    'empty_column' => 0,
  ),
);
/* No results behavior: Global: Text area */
$handler->display->display_options['empty']['area']['id'] = 'area';
$handler->display->display_options['empty']['area']['table'] = 'views';
$handler->display->display_options['empty']['area']['field'] = 'area';
$handler->display->display_options['empty']['area']['empty'] = TRUE;
$handler->display->display_options['empty']['area']['content'] = 'There is no scheduled content.';
$handler->display->display_options['empty']['area']['format'] = 'filtered_html';
/* Relationship: Workbench Scheduler: Node */
$handler->display->display_options['relationships']['nid']['id'] = 'nid';
$handler->display->display_options['relationships']['nid']['table'] = 'workbench_scheduler_nodes';
$handler->display->display_options['relationships']['nid']['field'] = 'nid';
$handler->display->display_options['relationships']['nid']['required'] = TRUE;
/* Field: Content revision: Nid */
$handler->display->display_options['fields']['nid']['id'] = 'nid';
$handler->display->display_options['fields']['nid']['table'] = 'node_revision';
$handler->display->display_options['fields']['nid']['field'] = 'nid';
$handler->display->display_options['fields']['nid']['label'] = '';
$handler->display->display_options['fields']['nid']['exclude'] = TRUE;
$handler->display->display_options['fields']['nid']['element_label_colon'] = FALSE;
/* Field: Workbench Scheduler: Schedule Label */
$handler->display->display_options['fields']['label']['id'] = 'label';
$handler->display->display_options['fields']['label']['table'] = 'workbench_scheduler_schedules';
$handler->display->display_options['fields']['label']['field'] = 'label';
$handler->display->display_options['fields']['label']['label'] = '';
$handler->display->display_options['fields']['label']['exclude'] = TRUE;
$handler->display->display_options['fields']['label']['element_label_colon'] = FALSE;
/* Field: Content revision: Vid */
$handler->display->display_options['fields']['vid']['id'] = 'vid';
$handler->display->display_options['fields']['vid']['table'] = 'node_revision';
$handler->display->display_options['fields']['vid']['field'] = 'vid';
/* Field: Content revision: Title */
$handler->display->display_options['fields']['title']['id'] = 'title';
$handler->display->display_options['fields']['title']['table'] = 'node_revision';
$handler->display->display_options['fields']['title']['field'] = 'title';
$handler->display->display_options['fields']['title']['alter']['word_boundary'] = FALSE;
$handler->display->display_options['fields']['title']['alter']['ellipsis'] = FALSE;
$handler->display->display_options['fields']['title']['link_to_node_revision'] = TRUE;
/* Field: Content: Type */
$handler->display->display_options['fields']['type']['id'] = 'type';
$handler->display->display_options['fields']['type']['table'] = 'node';
$handler->display->display_options['fields']['type']['field'] = 'type';
$handler->display->display_options['fields']['type']['relationship'] = 'nid';
/* Field: Workbench Scheduler: Date */
$handler->display->display_options['fields']['date']['id'] = 'date';
$handler->display->display_options['fields']['date']['table'] = 'workbench_scheduler_nodes';
$handler->display->display_options['fields']['date']['field'] = 'date';
$handler->display->display_options['fields']['date']['date_format'] = 'short';
$handler->display->display_options['fields']['date']['second_date_format'] = 'long';
/* Field: Workbench Scheduler: Completed */
$handler->display->display_options['fields']['completed']['id'] = 'completed';
$handler->display->display_options['fields']['completed']['table'] = 'workbench_scheduler_nodes';
$handler->display->display_options['fields']['completed']['field'] = 'completed';
$handler->display->display_options['fields']['completed']['not'] = 0;
/* Field: Global: Custom text */
$handler->display->display_options['fields']['nothing']['id'] = 'nothing';
$handler->display->display_options['fields']['nothing']['table'] = 'views';
$handler->display->display_options['fields']['nothing']['field'] = 'nothing';
$handler->display->display_options['fields']['nothing']['label'] = 'Operations';
$handler->display->display_options['fields']['nothing']['alter']['text'] = 'edit';
$handler->display->display_options['fields']['nothing']['alter']['make_link'] = TRUE;
$handler->display->display_options['fields']['nothing']['alter']['path'] = 'node/[nid]/manage_schedules/[vid]/edit?destination=admin/workbench/scheduled';
/* Sort criterion: Workbench Scheduler: Date */
$handler->display->display_options['sorts']['date']['id'] = 'date';
$handler->display->display_options['sorts']['date']['table'] = 'workbench_scheduler_nodes';
$handler->display->display_options['sorts']['date']['field'] = 'date';
$handler->display->display_options['sorts']['date']['granularity'] = 'day';
/* Filter criterion: Workbench Scheduler: Schedule Name */
$handler->display->display_options['filters']['name']['id'] = 'name';
$handler->display->display_options['filters']['name']['table'] = 'workbench_scheduler_schedules';
$handler->display->display_options['filters']['name']['field'] = 'name';
$handler->display->display_options['filters']['name']['group'] = 1;
$handler->display->display_options['filters']['name']['exposed'] = TRUE;
$handler->display->display_options['filters']['name']['expose']['operator_id'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['label'] = 'Schedule Name';
$handler->display->display_options['filters']['name']['expose']['operator'] = 'name_op';
$handler->display->display_options['filters']['name']['expose']['identifier'] = 'name';
$handler->display->display_options['filters']['name']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Workbench Scheduler: Completed */
$handler->display->display_options['filters']['completed']['id'] = 'completed';
$handler->display->display_options['filters']['completed']['table'] = 'workbench_scheduler_nodes';
$handler->display->display_options['filters']['completed']['field'] = 'completed';
$handler->display->display_options['filters']['completed']['value'] = 'All';
$handler->display->display_options['filters']['completed']['group'] = 1;
$handler->display->display_options['filters']['completed']['exposed'] = TRUE;
$handler->display->display_options['filters']['completed']['expose']['operator_id'] = '';
$handler->display->display_options['filters']['completed']['expose']['label'] = 'Completed';
$handler->display->display_options['filters']['completed']['expose']['operator'] = 'completed_op';
$handler->display->display_options['filters']['completed']['expose']['identifier'] = 'completed';
$handler->display->display_options['filters']['completed']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);
/* Filter criterion: Workbench Scheduler: Active */
$handler->display->display_options['filters']['is_active']['id'] = 'is_active';
$handler->display->display_options['filters']['is_active']['table'] = 'workbench_scheduler_nodes';
$handler->display->display_options['filters']['is_active']['field'] = 'is_active';
$handler->display->display_options['filters']['is_active']['value'] = '1';
$handler->display->display_options['filters']['is_active']['exposed'] = TRUE;
$handler->display->display_options['filters']['is_active']['expose']['operator_id'] = '';
$handler->display->display_options['filters']['is_active']['expose']['label'] = 'Active';
$handler->display->display_options['filters']['is_active']['expose']['operator'] = 'is_active_op';
$handler->display->display_options['filters']['is_active']['expose']['identifier'] = 'is_active';
$handler->display->display_options['filters']['is_active']['expose']['required'] = TRUE;
$handler->display->display_options['filters']['is_active']['expose']['remember_roles'] = array(
  2 => '2',
  1 => 0,
  3 => 0,
);

/* Display: Scheduled Content Page */
$handler = $view->new_display('page', 'Scheduled Content Page', 'scheduled_content_page');
$handler->display->display_options['path'] = 'admin/workbench/scheduled';
$handler->display->display_options['menu']['type'] = 'tab';
$handler->display->display_options['menu']['title'] = 'Scheduled Content';
$handler->display->display_options['menu']['weight'] = '99';
$handler->display->display_options['menu']['context'] = 0;
$handler->display->display_options['menu']['context_only_inline'] = 0;
$translatables['workbench_scheduler'] = array(
  t('Master'),
  t('Scheduled Content'),
  t('more'),
  t('Apply'),
  t('Reset'),
  t('Sort by'),
  t('Asc'),
  t('Desc'),
  t('There is no scheduled content.'),
  t('Node'),
  t('Vid'),
  t('Title'),
  t('Type'),
  t('Date'),
  t('Completed'),
  t('Operations'),
  t('edit'),
  t('Schedule Name'),
  t('Active'),
  t('Scheduled Content Page'),
);
