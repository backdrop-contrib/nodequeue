<?php
//$Id$
/**
 * Views field handler for the nodequeue queue links
 */
class nodequeue_handler_field_links extends views_handler_field {
  function render($values) {
    $nid = $nids->{$this->field_alias};
    return theme('links', nodequeue_link('node', node_load($nid)));
  }
}