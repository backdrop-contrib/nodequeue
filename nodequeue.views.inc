<?php
// $Id$

/**
 * @file nodequeue.views.inc
 * Provides support for the Views module.
 */

/**
 * Implementation of hook_views_tables()
 */
function nodequeue_views_data() {
  $data = array();

  // ----------------------------------------------------------------
  // nodequeue_nodes table

  $data['nodequeue_nodes']['table']['group'] = t('Node queue');

  // The nodequeue_nodes table is how nodequeue connects to the
  // rest of the world, but we will only join it in via relationship.

  // position
  $data['nodequeue_nodes']['position'] = array(
    'title' => t('Position'),
    'help' => t('The position of the node within a queue.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );

  // timestamp
  $data['nodequeue_nodes']['timestamp'] = array(
    'title' => t('Added date'),
    'help' => t('The date the node was added to a queue.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );

  $data['nodequeue_nodes']['qid'] = array(
    'title' => t('Queue ID'),
    'help' => t('The ID of the queue the node belongs to.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
      'name field' => 'title',
      'name table' => 'nodequeue_queue',
      'empty field name' => t('No queue'),
    ),
  );

  $data['nodequeue_nodes']['sqid'] = array(
    'title' => t('Subqueue ID'),
    'help' => t('The ID of the subqueue the node belongs to.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'nodequeue_handler_argument_subqueue_qid',
      'name field' => 'title',
      'name table' => 'nodequeue_subqueue',
      'empty field name' => t('No queue'),
    ),
  );

  // specialized is null/is not null filter
  $data['nodequeue_nodes']['in_queue'] = array(
    'title' => t('In queue'),
    'help' => t('Filter to ensure a node is NOT in the related queue'),
    'filter' => array(
      'handler' => 'nodequeue_handler_filter_not_in_queue',
      'label' => t('In queue'),
    ),
  );

  // ----------------------------------------------------------------
  // nodequeue_queue table

  $data['nodequeue_queue']['table']['group'] = t('Node queue');

  // For use with the relationship.
  $data['nodequeue_queue']['table']['join'] = array(
    'nodequeue_nodes' => array(
      'left_field' => 'qid',
      'field' => 'qid',
    ),
  );

  $data['nodequeue_queue']['title'] = array(
    'title' => t('Title'),
    'help' => t('The title of the node queue.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  // ----------------------------------------------------------------
  // nodequeue_subqueue table
  $data['nodequeue_subqueue']['table']['group'] = t('Node queue');

  // For use with the relationship.
  $data['nodequeue_subqueue']['table']['join'] = array(
    'nodequeue_nodes' => array(
      'left_field' => 'qid',
      'field' => 'qid',
    ),
  );

  return $data;

}

/**
 * Implementation of hook_views_data_alter().
 */
function nodequeue_views_data_alter(&$data) {
  $data['node']['nodequeue_rel'] = array(
    'group' => t('Node queue'),
    'title' => t('queue'),
    'help' => t('Create a relationship to a nodequeue'),
    'real field' => 'nid',
    'relationship' => array(
      'handler' => 'nodequeue_handler_relationship_nodequeue',
      'base' => 'nodequeue_nodes',
      'field' => 'nid',
      'label' => t('queue'),
    ),
  );

  // ajaxtoggle

  // all queues

  // link to queue tab
}

/**
 * Specialized relationship handler to add nodequeues.
 */
class nodequeue_handler_relationship_nodequeue extends views_handler_relationship {
  function options(&$options) {
    parent::options($options);

    $options['qid'] = -1;
  }

  /**
   * Default options form that provides the label widget that all fields
   * should have.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $queues = nodequeue_load_queues(nodequeue_get_all_qids(0));

    $options = array(-1 => t('All queues'));
    foreach ($queues as $queue) {
      $options[$queue->qid] = $queue->title;
    }

    $form['qid'] = array(
      '#type' => 'select',
      '#title' => t('Queue'),
      '#options' => $options,
      '#description' => t('Select which queue to create this relationship for; if "All queues" is selected, all queues a node belongs to will be in the query; this will cause duplicates so be careful when using this option.'),
      '#default_value' => $this->options['qid'],
    );
  }

  /**
   * Called to implement a relationship in a query.
   */
  function query() {
    // Figure out what base table this relationship brings to the party.
    $join = new views_join();
    $join->definition = array(
      'table' => 'nodequeue_nodes',
      'field' => 'nid',
      'left_table' => 'node',
      'left_field' => 'nid',
    );

    if (!empty($this->options['required'])) {
      $join->definition['type'] = 'INNER';
    }

    if (!empty($this->options['qid']) && $this->options['qid'] != -1) {
        $join->definition['extra'] = array(array(
          'field' => 'qid',
          'value' => $this->options['qid'],
          'numeric' => TRUE,
        ),
      );
    }

    $join->construct();

    $alias = $join->definition['table'] . '_' . $join->definition['left_table'];

    $this->alias = $this->query->add_relationship($alias, $join, 'nodequeue_nodes', $this->relationship);
  }
}

/**
 * Handler to filter for nodes that are not in a queue.
 */
class nodequeue_handler_filter_not_in_queue extends views_handler_filter_in_operator {
  function query() {
    $operator = empty($this->operator) ? 'IS NOT' : 'IS';
    $this->add_where($this->options['group'], "$this->table_alias.nid $operator NULL");
  }
}

/**
 * Argument handler for subqueues.
 *
 * Subqueues have titles that are determined by doing substitution from
 * the subqueue_title on the queue using the title on the subqueue, which
 * takes a little extra work to do here.
 */
class nodequeue_handler_argument_subqueue_qid extends views_handler_argument_numeric {
  function summary_name_field() {
    $this->queue_alias = $this->query->ensure_table('nodequeue_queue', $this->relationship);
    $this->subqueue_alias = $this->name_table_alias = $this->query->ensure_table('nodequeue_subqueue', $this->relationship);

    $this->queue_title_alias = $this->query->add_field($this->queue_alias, 'subqueue_title');
    $this->subqueue_title_alias = $this->name_alias = $this->query->add_field($this->subqueue_alias, 'title');
  }

  function summary_name($data) {
    $queue_title = $data->{$this->queue_title_alias};
    $subqueue_title = $data->{$this->subqueue_title_alias};

    $value = empty($queue_title) ? $subqueue_title : str_replace('%subqueue', $subqueue_title, $queue_title);

    if (empty($value) && !empty($this->definition['empty field name'])) {
      $value = $this->definition['empty field name'];
    }
    return check_plain($value);
  }
}

function nodequeue_views_default_views() {
  $view = array();
  return ;

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
    $view->url = 'nodequeue/'. $queue->qid;
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
    $view->sort = array(
      array(
        'tablename' => 'nodequeue_nodes',
        'field' => 'position',
        'sortorder' => 'ASC',
        'options' => '',
      ),
    );
    $view->argument = array(
    );
    $view->field = array(
      array(
        'tablename' => 'node',
        'field' => 'title',
        'label' => '',
        'handler' => 'views_handler_field_nodelink',
        'options' => 'link',
      ),
    );
    $view->exposed_filter = array(
    );
    $view->requires = array('nodequeue_nodes', 'node');
    $view->filter = array(
      array(
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
  switch ($op) {
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
  switch ($op) {
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
  switch ($op) {
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
  switch ($op) {
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
