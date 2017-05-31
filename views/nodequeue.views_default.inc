<?php
/**
 * @file
 *
 * Even though there is no hook for _views_default_views() in Backdrop, this
 * function will remain here to assist with creating views.
 */

/**
 * Helper function: Creates "default" views for nodequeue.
 * @see ().
 */
function nodequeue_views_default_views() {
  $queues = nodequeue_load_queues(nodequeue_get_all_qids());
  foreach ($queues as $queue) {
    $data = array(
      '_config_name' => 'views.view.nodequeue_' . $queue->qid,
      'name' => 'nodequeue_' . $queue->qid,
      'description' => t('Display a list of all nodes in queue @queue', array('@queue' => $queue->title)),
      'module' => 'views',
      'storage' => 2,
      'tag' => 'default',
      'disabled' => FALSE,
      'base_table' => 'node',
      'human_name' => t('Nodequeue: @queue', array('@queue' => $queue->title)),
      'core' => '1.7.0',
      'display' => array(
        'default' => array(
          'display_title' => 'Master',
          'display_plugin' => 'default',
          'display_options' => array(
            'query' => array(
              'type' => 'views_query',
              'options' => array(),
            ),
            'access' => array(
              'type' => 'perm',
              'perm' => 'access content',
            ),
            'cache' => array(
              'type' => 'none',
            ),
            'exposed_form' =>  array(
              'type' => 'basic',
            ),
            'pager' => array(
              'type' => 'some',
              'options' => array(
                'items_per_page' => '10',
              ),
            ),
            'style_plugin' => 'default',
            'row_plugin' => 'node',
            'fields' => array(
              'title' => array(
                'id' => 'title',
                'table' => 'node',
                'field' => 'title',
                'label' => '',
                'alter' => array(
                  'alter_text' => 0,
                  'make_link' => 0,
                  'absolute' => 0,
                  'trim' => 0,
                  'word_boundary' => 0,
                  'ellipsis' => 0,
                  'strip_tags' => 0,
                  'html' => 0,
                ),
                'hide_empty' => 0,
                'empty_zero' => 0,
                'link_to_node' => 1,
              ),
            ),
            'filters' => array(
              'status_extra' => array(
                'id' => 'status_extra',
                'table' => 'node',
                'field' => 'status_extra',
                'relationship' => 'none',
                'group_type' => 'group',
                'ui_name' => '',
                'operator' => '=',
                'value' => '',
                'group' => '1',
                'exposed' => FALSE,
                'expose' => array(
                  'operator_id' => FALSE,
                  'label' => '',
                  'description' => '',
                  'use_operator' => FALSE,
                  'operator' => '',
                  'identifier' => '',
                  'required' => FALSE,
                  'remember' => FALSE,
                  'multiple' => FALSE,
                  'remember_roles' => array(
                    'authenticated' => 'authenticated',
                  ),
                ),
                'is_grouped' => FALSE,
                'group_info' => array(
                  'label' => '',
                  'description' => '',
                  'identifier' => '',
                  'optional' => TRUE,
                  'widget' => 'select',
                  'multiple' => FALSE,
                  'remember' => 0,
                  'default_group' => 'All',
                  'default_group_multiple' => array(),
                  'group_items' => array(),
                ),
              ),
            ),
            'sorts' => array(
              'position' => array(
                'id' => 'position',
                'table' => 'nodequeue_nodes',
                'field' => 'position',
                'relationship' => 'nodequeue_rel',
                'group_type' => 'group',
                'ui_name' => '',
                'order' => 'ASC',
                'exposed' => FALSE,
                'expose' => array(
                  'label' => '',
                ),
              ),
            ),
            'title' => t('Queue @queue', array('@queue' => $queue->title)),
            'row_options' => array(
              'build_mode' => 'teaser',
              'links' => TRUE,
              'comments' => FALSE,
            ),
            'relationships' => array(
              'nodequeue_rel' => array(
                'id' => 'nodequeue_rel',
                'table' => 'node',
                'field' => 'nodequeue_rel',
                'relationship' => 'none',
                'group_type' => 'group',
                'ui_name' => '',
                'label' => 'queue',
                'required' => 1,
                'limit' => 1,
                'names' => array(
                  $queue->qid => $queue->qid,
                ),
              ),
            ),
          ),
        ),
        'page' => array(
          'display_title' => 'Page',
          'display_plugin' => 'page',
          'display_options' => array(
            'query' => array(
              'type' => 'views_query',
              'options' => array(),
            ),
            'path' => 'nodequeue-' . $queue->qid,
          ),
        ),
        'block' => array(
          'display_title' => 'Block',
          'display_plugin' => 'block',
          'display_options' => array(
            'query' => array(
              'type' => 'views_query',
              'options' => array(),
            ),
            'style_plugin' => 'default',
            'defaults' => array(
              'style_plugin' => FALSE,
              'style_options' => FALSE,
              'row_plugin' => FALSE,
              'row_options' => FALSE,
              'pager' => FALSE,
              'pager_options' => FALSE,
            ),
            'style_options' => array(),
            'row_plugin' => 'fields',
            'row_options' => array(
              'build_mode' => 'teaser',
              'links' => TRUE,
              'comments' => FALSE,
            ),
            'pager' => array(
              'type' => 'some',
              'options' => array(
                'items_per_page' => '5',
              ),
            ),
            'pager_options' => NULL,
            'field' => array(
              'title' => array(
                'link_to_node' => 1,
              ),
            ),
            'block_description' => 'Queue',
            'inherit_path' => '1',
          ),
        ),
      ),
    );
    $config = config('views.view.nodequeue_' . $queue->qid);
    // Don't overwrite existintg config.
    if ($config->isNew()) {
      $config->setData($data);
      $config->save();
    }
  }
}
