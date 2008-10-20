<?php
//$Id$
/**
 * Views field handler for the nodequeue queue links
 */
class nodequeue_handler_field_links extends views_handler_field_node_link {
  function render($values) {
    $nid = $values->{$this->aliases['nid']};
    return theme('links', nodequeue_link('node', node_load($nid)));
  }
}