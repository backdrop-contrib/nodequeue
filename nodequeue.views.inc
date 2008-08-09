<?php
// $Id$

/**
 * @file nodequeue.views.inc
 * Provides support for the Views module.
 *
 * This implementation is almost entirely based around a relationship
 * which is stapled to the 'node' table via hook_views_data_alter.
 *
 * The relationship allows you to define 0 or more queues that join the
 * queue tables, limiting the join to just the queues selected. Once
 * the queues are joined in, the queue fields will show up, and they
 * have the usual array of arguments, sorts, fields and filters; since
 * the real legwork is done by the relationship, most of these are
 * really quite simple.
 */

/**
 * Implementation of hook_views_data()
 */
function nodequeue_views_data() {
  $data = array();

  // ----------------------------------------------------------------
  // nodequeue_nodes table

  $data['nodequeue_nodes']['table']['group'] = t('Nodequeue');

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
    'help' => t('Filter to ensure a node IS or IS NOT in the related queue'),
    'filter' => array(
      'handler' => 'nodequeue_handler_filter_in_queue',
      'label' => t('In queue'),
    ),
  );

  // ----------------------------------------------------------------
  // nodequeue_queue table

  $data['nodequeue_queue']['table']['group'] = t('Nodequeue');

  // For use with the relationship.
  $data['nodequeue_queue']['table']['join'] = array(
    'nodequeue_nodes' => array(
      'left_field' => 'qid',
      'field' => 'qid',
    ),
  );

  $data['nodequeue_queue']['title'] = array(
    'title' => t('Title'),
    'help' => t('The title of the nodequeue.'),
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
  $data['nodequeue_subqueue']['table']['group'] = t('Nodequeue');

  // For use with the relationship.
  $data['nodequeue_subqueue']['table']['join'] = array(
    'nodequeue_nodes' => array(
      'left_field' => 'qid',
      'field' => 'qid',
    ),
  );

  $data['nodequeue_subqueue']['reference'] = array(
    'reference' => t('Subqueue reference'),
    'help' => t('The reference that defines a subqueue; what this actually is depends upon the type of subqueue, but is a taxonomy tid for smartqueue taxonomy, or a uid for authorview.'),
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

  return $data;
}

/**
 * Implementation of hook_views_data_alter().
 */
function nodequeue_views_data_alter(&$data) {
  // queue relationship
  $data['node']['nodequeue_rel'] = array(
    'group' => t('Nodequeue'),
    'title' => t('Queue'),
    'help' => t('Create a relationship to a nodequeue.'),
    'real field' => 'nid',
    'relationship' => array(
      'handler' => 'nodequeue_handler_relationship_nodequeue',
      'base' => 'nodequeue_nodes',
      'field' => 'nid',
      'label' => t('queue'),
    ),
  );

  // links
  $data['node']['nodequeue_links'] = array(
    'group' => t('Nodequeue'),
    'title' => t('Add/remove links'),
    'help' => t('Display add/remove links for all eligible nodequeues. Note that this performs a node_load so could add many extra queries to a view.'),
    'real field' => 'nid',
    'field' => array(
      'handler' => 'nodequeue_handler_field_links',
    ),
  );

  $data['node']['nodequeue_queue_tab'] = array(
    'group' => t('Nodequeue'),
    'title' => t('Link to tab'),
    'help' => t('Display add/remove links for all eligible nodequeues. Note that this performs a node_load so could add many extra queries to a view.'),
    'real field' => 'nid',
    'field' => array(
      'handler' => 'nodequeue_handler_field_queue_tab',
    ),
  );

  // all queues
  $data['node']['nodequeue_all_queues'] = array(
    'group' => t('Nodequeue'),
    'title' => t('All queues'),
    'help' => t('Display all queues a node is a member of.'),
    'real field' => 'nid',
    'field' => array(
      'handler' => 'nodequeue_handler_field_all_queues',
    ),
  );

  // all subqueues
  $data['node']['nodequeue_all_subqueues'] = array(
    'group' => t('Nodequeue'),
    'title' => t('All subqueues'),
    'help' => t('Display all subqueues a node is a member of.'),
    'real field' => 'nid',
    'field' => array(
      'handler' => 'nodequeue_handler_field_all_subqueues',
    ),
  );
}

/**
 * Specialized relationship handler to add nodequeues.
 */
class nodequeue_handler_relationship_nodequeue extends views_handler_relationship {
  function options(&$options) {
    parent::options($options);

    $options['limit'] = FALSE;
    $options['qids'] = array();
  }

  /**
   * Default options form that provides the label widget that all fields
   * should have.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));

    $form['limit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Limit to one or more queues (recommended)'),
      '#default_value'=> $this->options['limit'],
    );

    $options = array();
    $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
    foreach ($queues as $queue) {
      $options[$queue->qid] = $queue->title;
    }

    $form['qids'] = array(
      '#prefix' => '<div><div id="edit-options-qids">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Queues'),
      '#options' => $options,
      '#default_value' => $this->options['qids'],
      '#process' => array('expand_checkboxes', 'views_process_dependency'),
      '#dependency' => array('edit-options-limit' => array(TRUE)),
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

    if (!empty($this->options['limit'])) {
      $join->definition['extra'] = array(array(
        'field' => 'qid',
        'value' => array_filter($this->options['qids']),
        'numeric' => TRUE,
      ));
    }

    $join->construct();

    $alias = $join->definition['table'] . '_' . $join->definition['left_table'];

    $this->alias = $this->query->add_relationship($alias, $join, 'nodequeue_nodes', $this->relationship);
  }
}

/**
 * Handler to filter for nodes that are not in a queue.
 */
class nodequeue_handler_filter_in_queue extends views_handler_filter_boolean_operator {
  function query() {
    $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.nid " . ($this->value ? '<> 0' : 'IS NULL'));
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

/**
 * Views field handler for the nodequeue queue links
 */
class nodequeue_handler_field_links extends views_handler_field {
  function render($values) {
    $nid = $nids->{$this->field_alias};
    return theme('links', nodequeue_link('node', node_load($nid)));
  }
}

/**
 * Views field handler for the nodequeue queue links
 */
class nodequeue_handler_field_queue_tab extends views_handler_field_node_link {
  function render($values) {
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
    $nid = $values->{$this->aliases['nid']};
    return l($text, "node/$nid/nodequeue");
  }
}

/**
 * Field handler for all queues.
 *
 * @ingroup views_field_handlers
 */
class nodequeue_handler_field_all_queues extends views_handler_field_prerender_list {
  /**
   * Provide meaningful defaults
   */
  function options(&$options) {
    parent::options($options);
    $options['link_to_queue'] = TRUE;
    $options['limit'] = FALSE;
    $options['qids'] = array();
  }

  /**
   * Provide "link to term" option.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_queue'] = array(
      '#title' => t('Link this field to queue arrange page'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_taxonomy']),
    );

    $form['limit'] = array(
      '#type' => 'checkbox',
      '#title' => t('Limit to queues'),
      '#default_value'=> $this->options['limit'],
    );

    $options = array();
    $queues = nodequeue_load_queues(nodequeue_get_all_qids(NULL));
    foreach ($queues as $queue) {
      $options[$queue->qid] = $queue->title;
    }

    $form['qids'] = array(
      '#prefix' => '<div><div id="edit-options-qids">',
      '#suffix' => '</div></div>',
      '#type' => 'checkboxes',
      '#title' => t('Queues'),
      '#options' => $options,
      '#default_value' => $this->options['qids'],
      '#process' => array('expand_checkboxes', 'views_process_dependency'),
      '#dependency' => array('edit-options-limit' => array(TRUE)),
    );
  }

  function pre_render($values) {
    $nids = array();
    foreach ($values as $result) {
      $nids[] = $result->{$this->field_alias};
    }

    if ($nids) {
      $queue = '';
      if (!empty($this->options['limit']) && !empty($this->options['qids'])) {
        $queue = " AND nn.qid IN (" . implode(', ', array_keys(array_filter($this->options['qids']))) . ")";
      }

      $result = db_query("SELECT nn.nid, nn.qid, nq.title FROM {nodequeue_nodes} nn INNER JOIN {nodequeue_queue} nq ON nq.qid = nn.qid WHERE nn.nid IN (" . implode(', ', $nids) . ")$queue ORDER BY nq.title");

      while ($queue = db_fetch_object($result)) {
        if (empty($this->options['link_to_queue'])) {
          $this->items[$queue->nid][$queue->qid] = check_plain($queue->title);
        }
        else {
          $this->items[$queue->nid][$queue->qid] = l($queue->title, "admin/content/nodequeue/$queue->qid");
        }
      }
    }
  }
}

/**
 * Field handler for all subqueues
 *
 * @ingroup views_field_handlers
 */
class nodequeue_handler_field_all_subqueues extends nodequeue_handler_field_all_queues {
  function pre_render($values) {
    $nids = array();
    foreach ($values as $result) {
      $nids[] = $result->{$this->field_alias};
    }

    if ($nids) {
      $queue = '';
      if (!empty($this->options['limit']) && !empty($this->options['qids'])) {
        $queue = " AND nn.qid IN (" . implode(', ', array_keys(array_filter($this->options['qids']))) . ")";
      }

      $result = db_query("SELECT nn.nid, nn.sqid, nn.qid, nq.subqueue_title, ns.title FROM {nodequeue_nodes} nn INNER JOIN {nodequeue_queue} nq ON nq.qid = nn.qid INNER JOIN {nodequeue_subqueue} ns on nn.sqid = ns.sqid WHERE nn.nid IN (" . implode(', ', $nids) . ")$queue ORDER BY ns.title, nq.subqueue_title");

      while ($queue = db_fetch_object($result)) {
        $title = empty($queue->subqueue_title) ? $queue->title : str_replace('%subqueue', $queue->title, $queue->subqueue_title);
        if (empty($this->options['link_to_queue'])) {
          $this->items[$queue->nid][$queue->sqid] = check_plain($title);
        }
        else {
          $this->items[$queue->nid][$queue->sqid] = l($title, "admin/content/nodequeue/$queue->qid/$queue->sqid");
        }
      }
    }
  }
}
