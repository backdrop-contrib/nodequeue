<?php
// $Id$

/**
 * @file nodequeue.views.inc
 * Provides support for the Views module.
 */

/**
 * Implementation of hook_views_tables()
 */
function nodequeue_views_tables() {
  $tables['nodequeue_nodes'] = array(
    "name" => "nodequeue_nodes",
    "join" => array(
      "left" => array(
        "table" => "node",
        "field" => "nid"
      ),
      "right" => array(
        "field" => "nid"
      ),
    ),
    "fields" => array(
      'timestamp' => array(
        'name' => t('NodeQueue: Timestamp'),
        'sortable' => true,
        'handler' => views_handler_field_dates(),
        'option' => 'string',
        'help' => t('Display the time the node was added to a given node queue.').' '.
                  t('The option field may be used to specify the custom date format as it\'s required by the date() function or if "as time ago" has been chosen to customize the granularity of the time interval.'),
      ),
      'ajaxtoggle' => array(
        'name' => t('NodeQueue: AJAX Add/Remove Link'),
        'handler' => 'nodequeue_views_ajax_link',
        'notafield' => true,
      ),
      'nodequeue_link' => array(
        'name' => t('NodeQueue: Link to nodequeue tab'),
        'sortable' => false,
        'notafield' => TRUE,
        'handler' => 'nodequeue_views_tab_link',
        'option' => 'string',
        'help' => t('Display a link to the nodequeue tab for the node.  Enter the text of this link into the option field; if blank the default "Node queues" will be used.'),
      ),
    ),
    "filters" => array(
      "qid" => array(
        'name' => t("NodeQueue: Queue"),
        'value' => array(
          '#type' => 'select',
          '#options' => 'nodequeue_handler_queuelist',
          '#simple_queues' => TRUE,
        ),
        'operator' => 'views_handler_operator_andor',
        'help' => t('Filter the view to a specific Node Queue. This only applies to queues with a single subqueue.'),
      ),
      "position" => array(
        'name' => t("NodeQueue: Queue Position"),
        'field' => 'position',
        'operator' => 'views_handler_operator_gtlt',
        'help' => t('Filter by where in the queue an item is.'),
      ),
      "timestamp" => array(
        'name' => t('NodeQueue: Queue Timestamp'),
        'operator' => 'views_handler_operator_gtlt',
        'value' => views_handler_filter_date_value_form(),
        'handler' => 'views_handler_filter_timestamp',
        'option' => 'string',
        'help' => t('This filter allows nodes to be filtered by the time they were added to a given NodeQueue.') .' '. views_t_strings('filter date'),
      ),
      "exclusion" => array(
        'name' => t('NodeQueue: Not in a queue'),
        'operator' => array('IS' => t('is')),
        'value' => array(
          '#type' => 'select',
          '#options' => 'nodequeue_handler_queuelist',
          '#all_ok' => TRUE,
        ),
        'field' => 'qid',
        'handler' => 'nodequeue_handler_filter_exclusion',
        'help' => t('This filter allows nodes to be filtered that do not belong to any node queue, or do not belong to a particular node queue.'),
      ),

    ),
    "sorts" => array(
      "position" => array(
        'name' => t("NodeQueue: Queue Position"),
        'field' => 'position',
        'help' => t('When sorting by queue position, be sure the view is filtered to a single queue or the sort will not work very well.'),
      ),
      'timestamp' => array(
        'name' => t('NodeQueue: Timestamp'),
        'handler' => 'views_handler_sort_date',
        'option' => views_handler_sort_date_options(),
        'help' => t('Sort by the time the node was added to a node queue.'),
      ),
    ),
  );

  foreach (nodequeue_load_queues(nodequeue_get_all_qids(NULL)) as $queue) {
    if ($queue->subqueues > 1) {
      $tables['nodequeue_nodes']['filters']["qid_$queue->qid"] = array(
        'name' => t("NodeQueue: Subqueues for @queue", array('@queue' => $queue->title)),
        'field' => 'qid',
        'value' => array(
          '#type' => 'select',
          '#options' => 'nodequeue_handler_queuelist',
          '#qid' => $queue->qid,
        ),
        'operator' => 'views_handler_operator_andor',
        'help' => t('Filter the view to a specific Node Queue.'),
      );
    }
    else {
      $tables['nodequeue_nodes_'. $queue->qid] = array(
        "name" => "nodequeue_nodes",
        "join" => array(
          "left" => array(
            "table" => "node",
            "field" => "nid"
          ),
          "right" => array(
            "field" => "nid"
          ),
          'extra' => array(
            'qid' => $queue->qid,
          ),
        ),
        "sorts" => array(
          "position" => array(
            'name' => t("NodeQueue: Queue Position in @queue", array('@queue' => $queue->title)),
            'field' => 'position',
            'help' => t('Sort by the position of the node in a given queue. Use this only if, for some reason, you are filtering in such a way that you are including nodes from multiple queues; otherwise use the simpler "NodeQueue: Queue Position" filter.'),
          ),
        ),
      );
    }
  }
  $tables['nodequeue_subqueue'] = array(
    "name" => "nodequeue_subqueue",
    "join" => array(
      "left" => array(
        "table" => "nodequeue_nodes",
        "field" => "sqid"
      ),
      "right" => array(
        "field" => "sqid"
      ),
    ),
  );
  $tables['nodequeue_queue'] = array(
    "name" => "nodequeue_queue",
    "join" => array(
      "left" => array(
        "table" => "nodequeue_subqueue",
        "field" => "qid"
      ),
      "right" => array(
        "field" => "qid"
      ),
    ),
  );
  return $tables;
}

function nodequeue_views_arguments() {
  $arguments = array(
    'nodequeue_qid' => array(
      'name' => t("NodeQueue: Queue ID"),
      'handler' => "nodequeue_handler_arg_qid",
      'help' => t('The Queue ID argument allows users to filter a view by specifying a Node Queue ID. This is only valid for simple nodequeues with just one subqueue. You should not use this argument if you have queues with subqueues.'),
    ),
    'nodequeue_qtitle' => array(
      'name' => t("NodeQueue: Subqueue title"),
      'handler' => "nodequeue_handler_arg_qtitle",
      'option' => array(
        '#type' => 'select',
        '#options' => 'nodequeue_handler_queuelist',
        '#all_ok' => TRUE,
      ),
      'help' => t('The Queue Title argument allows users to filter a view by specifying a Subqueue Title. Use the Option to restrict this to just one queue.'),
    ),
    "nodequeue_sqid" => array(
      'name' => t("NodeQueue: Subqueue ID"),
      'handler' => "nodequeue_handler_arg_sqid",
      'operator' => 'views_handler_operator_andor',
      'option' => array(
        '#type' => 'select',
        '#options' => 'nodequeue_handler_queuelist',
        '#all_ok' => TRUE,
      ),
      'help' => t('Filter the view to a specific Sub Queue.'),
    ),
    "nodequeue_reference" => array(
      'name' => t("NodeQueue: Subqueue reference"),
      'handler' => "nodequeue_handler_arg_reference",
      'operator' => 'views_handler_operator_andor',
      'option' => array(
        '#type' => 'select',
        '#options' => 'nodequeue_handler_queuelist',
        '#any_ok' => TRUE,
      ),
      'help' => t('Filter the view to a specific Sub Queue. The subqueue reference is the ID that the subqueue is attached to; tax id for taxonomy or user id for user queues, etc.'),
    ),
  );
  return $arguments;
}

function nodequeue_views_default_views() {
  $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
  foreach ($queues as $queue) {
    $view = new stdClass();
    $view->name = "nodequeue_$queue->qid";
    $view->disabled = TRUE;
    $view->description = t('View node queue @queue', array('@queue' => $queue->title));
    $view->access = array();
    $view->view_args_php = '';
    $view->page = TRUE;
    $view->page_title = check_plain($queue->title);
    $view->page_header = '';
    $view->page_header_format = '1';
    $view->page_footer = '';
    $view->page_footer_format = '1';
    $view->page_empty = '';
    $view->page_empty_format = '1';
    $view->page_type = 'teaser';
    $view->url = 'nodequeue/' . $queue->qid;
    $view->use_pager = TRUE;
    $view->nodes_per_page = '10';
    $view->block = TRUE;
    $view->block_title = check_plain($queue->title);
    $view->block_header = '';
    $view->block_header_format = '1';
    $view->block_footer = '';
    $view->block_footer_format = '1';
    $view->block_empty = '';
    $view->block_empty_format = '1';
    $view->block_type = 'list';
    $view->nodes_per_block = '5';
    $view->block_more = TRUE;
    $view->block_use_page_header = FALSE;
    $view->block_use_page_footer = FALSE;
    $view->block_use_page_empty = FALSE;
    $view->sort = array (
      array (
        'tablename' => 'nodequeue_nodes',
        'field' => 'position',
        'sortorder' => 'ASC',
        'options' => '',
      ),
    );
    $view->argument = array (
    );
    $view->field = array (
      array (
        'tablename' => 'node',
        'field' => 'title',
        'label' => '',
        'handler' => 'views_handler_field_nodelink',
        'options' => 'link',
      ),
    );
    $view->exposed_filter = array (
    );
    $view->requires = array('nodequeue_nodes', 'node');
    $view->filter = array (
      array (
        'tablename' => 'nodequeue_nodes',
        'field' => 'qid',
        'operator' => 'OR',
        'options' => '',
        'value' => array($queue->qid),
      ),
    );
    $views[$view->name] = $view;
  }
  return $views;
}

/**
 * Generate a list of queues for use in handlers.
 */
function nodequeue_handler_queuelist($op, $argument) {
  $options = array();
  if (isset($argument['#all_ok']) || isset($argument['value']['#all_ok'])) {
    $options[0] = t('<All>');
  }

  if (isset($argument['#any_ok']) || isset($argument['value']['#any_ok'])) {
    $options[-1] = t('<Any>');
  }

  $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
  foreach ($queues as $queue) {
    // no check plain necessary as this goes into a fieldset which takes
    // care of that.
    if (empty($argument['#simple_queues']) || $queue->subqueues == 1) {
      $options[$queue->qid] = $queue->title;
    }
  }
  return $options;
}

/**
 * Generate a list of queues for use in option/value fields.
 */
function nodequeue_handler_subqueuelist($op, $argument) {
  $subqueues = nodequeue_load_subqueues_by_queue($argument['#qid']);
  foreach ($subqueues as $subqueue) {
    $items[$subqueue->sqid] = $subqueue->title;
  }
  return $items;
}

function nodequeue_handler_arg_qid($op, &$query, $argtype, $arg = '') {
  switch($op) {
    case 'summary':
      $query->ensure_table('nodequeue_queue', true);
      $query->add_field('title', 'nodequeue_queue');
      $query->add_field('qid', 'nodequeue_queue');
      $query->add_where('nodequeue_queue.qid IS NOT NULL');
      $fieldinfo['field'] = "nodequeue_queue.title";
      return $fieldinfo;
      break;
    case 'sort':
      $query->add_orderby('nodequeue_queue', 'title', $argtype);
      break;
    case 'filter':
      $qid = intval($arg);
      $query->ensure_table('nodequeue_queue', true);
      $query->add_where("nodequeue_queue.qid = %d", $qid);
      break;
    case 'link':
      return l($query->title, "$arg/" . intval($query->qid));
    case 'title':
      $queue = db_fetch_object(db_query("SELECT title FROM {nodequeue_queue} WHERE qid = %d", $query));
      return $queue->title;
  }
}

function nodequeue_handler_arg_qtitle($op, &$query, $argtype, $arg = '') {
  switch($op) {
    case 'summary':
      $query->ensure_table('nodequeue_subqueue', true);
      $query->add_field('title', 'nodequeue_subqueue');
      $query->add_field('sqid', 'nodequeue_subqueue');
      $query->add_where('nodequeue_subqueue.sqid IS NOT NULL');
      if (!empty($arg)) {
        // User wanted to restrict to a specific queue, too.
        $query->add_where("nodequeue_subqueue.qid = %d", $arg);
      }
      $fieldinfo['field'] = "nodequeue_subqueue.title";
      return $fieldinfo;
      break;
    case 'sort':
      $query->add_orderby('nodequeue_subqueue', 'title', $argtype);
      break;
    case 'filter':
      $qtitle = $arg;
      $query->ensure_table('nodequeue_subqueue', true);
      $query->add_where("nodequeue_subqueue.title = '%s'", $qtitle);
      if (!empty($argtype['options'])) {
        // User wanted to restrict to a specific queue, too.
        $query->add_where("nodequeue_subqueue.qid = %d", $argtype['options']);
      }
      break;
    case 'link':
      return l($query->title, "$arg/" . $query->title);
    case 'title':
      return check_plain($query);
  }
}

function nodequeue_handler_arg_sqid($op, &$query, $argtype, $arg = '') {
  switch($op) {
    case 'summary':
      $query->ensure_table('nodequeue_subqueue', true);
      $query->add_field('title', 'nodequeue_subqueue');
      $query->add_field('sqid', 'nodequeue_subqueue');
      $query->add_where('nodequeue_subqueue.sqid IS NOT NULL');
      if ($arg) {
        $query->add_where('nodequeue_subqueue.qid = %d', $arg);
      }
      $fieldinfo['field'] = "nodequeue_subqueue.title";
      return $fieldinfo;
      break;
    case 'sort':
      $query->add_orderby('nodequeue_subqueue', 'title', $argtype);
      break;
    case 'filter':
      $query->ensure_table('nodequeue_subqueue', true);
      $query->add_where("nodequeue_subqueue.sqid = %d", $arg);
      if ($argtype['options']) {
        $query->add_where('nodequeue_subqueue.qid = %d', $argtype['options']);
      }
      break;
    case 'link':
      return l($query->title, "$arg/" . intval($query->sqid));
    case 'title':
      $queue = db_fetch_object(db_query("SELECT title FROM {nodequeue_subqueue} WHERE sqid = %d", $query));
      return check_plain($queue->title);
  }
}

function nodequeue_handler_arg_reference($op, &$query, $argtype, $arg = '') {
  switch($op) {
    case 'summary':
      // $arg == $option
      $query->ensure_table('nodequeue_subqueue', true);
      $query->add_field('title', 'nodequeue_subqueue');
      $query->add_field('reference', 'nodequeue_subqueue');
      $query->add_where('nodequeue_subqueue.reference IS NOT NULL');
      if ($argtype['options'] != -1) {
        $query->add_where('nodequeue_subqueue.qid = %d', $arg);
      }
      $fieldinfo['field'] = "nodequeue_subqueue.title";
      return $fieldinfo;
      break;
    case 'sort':
      $query->add_orderby('nodequeue_subqueue', 'title', $argtype);
      break;
    case 'filter':
      // $argtype['options'] == $option
      $query->ensure_table('nodequeue_subqueue', true);
      $query->add_where("nodequeue_subqueue.reference = %d", $arg);
      if ($argtype['options'] != -1) {
       $query->add_where('nodequeue_subqueue.qid = %d', $argtype['options']);
      };
      break;
    case 'link':
      return l($query->title, "$arg/" . intval($query->reference));
    case 'title':
      $queue = db_fetch_object(db_query("SELECT title FROM {nodequeue_subqueue} WHERE reference = %d", $query));
      return check_plain($queue->title);
  }
}

function nodequeue_handler_filter_exclusion($op, $filter, $filterinfo, &$query) {
  $table = $filterinfo['table'];
  $column = $filterinfo['field'];
  $value = $filter['value'];
  $joininfo = array(
    'type' => 'LEFT',
    'left' => array(
      'table' => 'node',
      'field' => 'nid',
    ),
    'right' => array(
      'field' => 'nid',
    ),
  );

  // If we're just checking for one queue, add the qid to our join info.
  if (!empty($value)) {
    $joininfo['extra']['qid'] = $value;
  }

  $num = $query->add_table($table, true, 1, $joininfo);
  $tablename = $query->get_table_name($table, $num);
  $field = "$tablename.$column";
  $query->add_where("$field IS NULL");
}

/**
 * Views field handler for "NodeQueue: AJAX Add/Remove Link"
 */
function nodequeue_views_ajax_link($fieldinfo, $fielddata, $value, $data) {
  return theme('links', nodequeue_link('node', node_load($data->nid)));
}

/**
 * display a link to view a node
 */
function nodequeue_views_tab_link($fieldinfo, $fielddata, $value, $data) {
  $link_text = $fielddata['options'] ? $fielddata['options'] : t('Node queues');
  return l($link_text, "node/$data->nid/nodequeue");
}
