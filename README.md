Node Queue
---------------------

This module allows an administrator to arbitrarily put nodes in a group for some purpose; examples of this might be to highlight one particular node, as in a typical news site's Lead Article.

Another use might be to create a block listing teasers for 5 forum posts that the admin thinks are important. Another use might be to create a group of nodes, and then have a block or the front page offer one of these randomly.

CONTENTS OF THIS FILE
---------------------

 - Introduction
 - Tested
 - Known Issues
 - Special Thanks
 - Requirements
 - Installation
 - Coming From Drupal?
 - Usage
 - License
 - Credits
 - Maintainers

TESTED
-----

Not working in Backdrop 1.3, but won't break your site.  This module has a lot of parts/integrations (Services, Rules, Drush, Taxonomy, Views, Search, Tests, etc ) so that if you want to work on this module, you are welcomed to.  The basic porting is done, but there are issues to fix with the sub-systems.

KNOWN ISSUES
---------------------

There are variables such as:
$boost = apachesolr_environment_variable_get($env_id, "nodequeue_apachesolr_boost_$sqid", 0);

What should we do with these?


SPECIAL THANKS
--------------

You can view a screencast about Nodequeue by Learn by the Drop. <https://www.youtube.com/watch?v=s3oCr5iWeTc>


REQUIREMENTS
------------

@todo

INSTALLATION
------------

Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules

COMING FROM DRUPAL?
-------------------

nothing substantially changed yet.

@todo

PERMISSIONS
------------

@todo

USAGE
-----

The Nodequeue module allows users to collect nodes in an arbitrarily ordered list. The order in the list can be used for a any purpose, such as:

A block listing teasers for the five top news stories on a site
A user’s favorite music albums
A group of favorite from which one is randomly displayed
Nodequeue provides a simple drag-and-drop interface to manually order any queue. Additionally, it allows nodes to be added and removed from queues without needing edit permissions to the node. Nodes can be added to queues either from a queue management tab or by links on the node teaser.

Queues can be set to allow only certain types of nodes to be added to the
queue. Queue can be a fixed size or of infinite length. And the admin can
select which roles have permission to add nodes to a given queue.

Once a queue is set up, a new tab will appear on eligible nodes for eligible
users. This tab will allow the user--regardless of edit permissions--to add or
remove that node from the queue. Queue admins can view the nodes in the queue,
and can modify the order of items already in the queue. Items may also appear
in a nodes links area to add/remove them from the queue.

When a node is added to the queue, it is added to the back of the queue. If a
queue is full when a node is added, the front of the queue is removed.

Nodequeue has support for nodes with i18n Internationalizations.

It is highly recommended that you use the Views module to display your queues.
However, if you choose not to, here is an alternative: Writing a PHP snippet.

To Create a Block to Display Node Titles of a Queue
---------------------------------------------------------

You'll need the Queue ID, which is easily extracted from the URL on the
queue administration page.

Create a new block, and insert the following PHP snippet into the block:

   <?php print nodequeue_node_titles(QUEUEID); ?>

If you want this queue to be printed in the reverse order, you can tell it
to print backward:

   <?php print nodequeue_node_titles(QUEUEID, '', true); ?>

The '' in the line above is an optional title field. Feel free to put
something here, but it's not terribly necessary in a block.

Programatically displaying nodes from a Queue
---------------------------------------------------------

The following funcitons can be used to display nodes from a subqueue, based on their position.
For more customized displays of content from a queue, please use the Views module (http://drupal.org/project/views).
For the most up to date Nodequeue API information, see http://drupal.org/node/293117.
Programmatic Ways of Displaying Content from a Queue

Nodequeue provides several functions which simplify getting a loaded node object from the front, back or a random position in a queue. For more selecting or displaying content in a more specific or complicated way, the Views module is probably your best bet.


To Create a Block to Display Node Titles of a Queue

You'll need the Queue ID, which is easily extracted from the URL on the queue administration page.

Create a new block, and insert the following PHP snippet into the block:

  <?php print nodequeue_node_titles($subqueue_id); ?>

If you want this queue to be printed in the reverse order, you can tell it to print backward:

  <?php print nodequeue_node_titles($subqueue_id, '', TRUE); ?>

The '' in the line above is an optional title field. Feel free to put something here, but it's not terribly necessary in a block.

LICENSE
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.

CREDITS
-----------

https://www.drupal.org/u/merlinofchaos

https://www.drupal.org/u/ezra-g

https://www.drupal.org/u/amateescu

https://www.drupal.org/u/fizk

https://www.drupal.org/u/sebcorbin


MAINTAINERS
-----------

- seeking

Ported to Backdrop by:

- biolithic <https://github.com/biolithic>
