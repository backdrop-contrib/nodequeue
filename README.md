Node Queue
==========

Allows people to put content into an ordered grouping.

Nodequeue provides a simple drag-and-drop interface to manually order any queue.
Additionally, it allows nodes to be added and removed from queues without
requiring edit permissions for the node. Nodes can be added to queues either
from a queue management tab or by links on the node teaser.

Once a queue is set up, a new tab will appear on eligible nodes for eligible
users. This tab will allow the user--regardless of edit permissions--to add or
remove that node from the queue. Queue admins can view the nodes in the queue,
and can modify the order of items already in the queue. Items may also appear
in a nodes links area to add/remove them from the queue.

When a node is added to the queue, it is added to the back of the queue. If a
queue is full when a node is added, the front of the queue is removed.

Nodequeue has support for nodes with i18n Internationalizations.

Examples of uses for ordered groupings include:

* To control the ontent in, and order of, a home page slideshow.
* To highlight one particular node, as in a typical news site's Lead Article.
* To create a block listing 5 posts that are deemed the most important.
* To create a group of special posts, and then have a block on the home page
  that selects one of these randomly.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- Assign permissions at Administration > Configuration > User accounts >
  Permissions (admin/config/people/permissions).

- Add and manage Queues at Administration > Structure > NodeQueue
  (admin/structure/nodequeue).

- Additional NodeQueue settings can be found at Administration > Structure >
  NodeQueue > Settings (admin/structure/nodequeue/settings).

Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/nodequeue/wiki/Documentation

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/nodequeue/issues

Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton)
- Seeking additional maintainers

Credits
-------

- Ported to Backdrop CMS by [biolithic](https://github.com/biolithic)
- Maintained for Drupal by [Earl Miles](https://www.drupal.org/u/merlinofchaos)
- Maintained for Drupal by [Ezra Gildesgame](https://www.drupal.org/u/ezra-g)
- Maintained for Drupal by [Andrei Mateescu](https://www.drupal.org/u/amateescu)
- Maintained for Drupal by [Yonas Yanfa](https://www.drupal.org/u/fizk)
- Maintained for Drupal by [Sébastien Corbin](https://www.drupal.org/u/sebcorbin)
- Created for Drupal by [Paul Byrne](https://www.drupal.org/u/pfaocle)
- View a screencast about Nodequeue by [Learn by the Drop](https://www.youtube.com/watch?v=s3oCr5iWeTc)

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

