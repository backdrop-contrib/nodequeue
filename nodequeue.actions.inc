<?php
// $Id$

/**
 * @file nodequeue.actions.inc
 * Provides actions integration for node queues.
 */

/**
 * Action to add a node to a queue.
 */
function action_nodequeue_add($op, $edit = array(), $node) {
  switch($op) {
    case 'metadata':
      return array(
        'description' => t('Add to Node Queues'),
        'type' => t('node'),
        'batchable' => true,
        'configurable' => true,
      );
      break;

    case 'do':
      $queues = nodequeue_load_queues($edit['qids']);
      // Filter out queues by node type. We choose not to use nodequeue_get_qids() because it checks for access control which only matters if we administering a queue.
      $eligible_queues = array();
      foreach ($queues as $queue) {
        if (in_array($node->type, $queue->types)) {
          $eligible_queues[$queue->qid] = $queue;
        }
      }
      
      if (!empty($eligible_queues)) {
        // Remove the node from the eligible queues (if needed).
        action_nodequeue_remove('do', array('qids' => array_keys($eligible_queues)), $node);

        // Use API to get the eligible subqueues
        $eligible_subqueues = nodequeue_get_subqueues_by_node($eligible_queues, $node);

        // Add node to each subqueue.
        foreach ($eligible_subqueues as $subqueue) {
          nodequeue_subqueue_add($queues[$subqueue->qid], $subqueue, $node->nid);
        }
      }
      break;

    // return an HTML config form for the action
    case 'form':
      // default values for form
      if (!isset($edit['qids'])) $edit['qids'] = '';

      $queues = nodequeue_load_queues(nodequeue_get_all_qids(500));
      foreach ($queues as $qid => $queue) {
        $options[$qid] = $queue->title;
      }
      $form = array();
      if (count($options)) {
        // add form components
        $form['qids'] = array(
          '#type' => 'select',
          '#title' => t("Queue"),
          '#default_value' => $edit['qids'],
          '#multiple' => TRUE,
          '#options' => $options,
          '#required' => TRUE,
          '#description' => t('Specify the queues into which the node should be submitted. If the queue is a smartqueue, the node shall be placed into every subqueue for which it is eligible.')
          );
      }
      else {
        drupal_set_message(t('Please <a href="!url">create</a> a nodequeue first.', array('!url' => url('admin/content/nodequeue'))));
      }
      return $form;

     // validate the HTML form

    // process the HTML form to store configuration
    case 'submit':
      $params = array(
        'qids' => $edit['qids']
      );
      return $params;
      break;
  }
}

/**
 * Action to remove a node from a queue.
 */
function action_nodequeue_remove($op, $edit = array(), $node) {
  switch($op) {
    case 'metadata':
      return array(
        'description' => t('Remove from Node Queues'),
        'type' => t('node'),
        'batchable' => true,
        'configurable' => true,
      );
      break;

    case 'do':
      $qids = $edit['qids'];
      // If a node is being deleted, ensure it's also removed from any queues.
      $placeholders = implode(',', array_fill(0, count($qids), '%d'));
      $args = $qids;
      $args[] = $node->nid;
      $result = db_query("SELECT * FROM {nodequeue_nodes} WHERE qid IN ($placeholders) AND nid = %d", $args);
      while ($obj = db_fetch_object($result)) {
        // This removes by nid, not position, because if we happen to have a
        // node in a queue twice, the 2nd position would be wrong.
        nodequeue_subqueue_remove_node($obj->sqid, $node->nid);
      }
      break;

    // return an HTML config form for the action
    case 'form':
      // default values for form
      if (!isset($edit['qids'])) $edit['qids'] = array();

      $queues = nodequeue_load_queues(nodequeue_get_all_qids(500));
      foreach ($queues as $qid => $queue) {
        $options[$qid] = $queue->title;
      }

      // add form components
      $form['qids'] = array(
        '#type' => 'select',
        '#title' => t("Queues"),
        '#default_value' => $edit['qids'],
        '#multiple' => TRUE,
        '#decription' => t('Specify the queues from which the node should be removed. If the queue is a smartqueue, the node shall be removed from all subqueues.'),
        '#required' => TRUE,
        '#options' => $options,
      );
      return $form;
      break;

     // validate the HTML form

    // process the HTML form to store configuration
    case 'submit':
      $params = array(
        'qids' => $edit['qids']
      );
      return $params;
      break;
  }
}
