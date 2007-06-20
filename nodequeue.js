// $Id$

Drupal.nodequeue = {};

Drupal.nodequeue.autoAttach = function() {
  $('a.nodequeue-move-up').click(function(e) {
    var item = $(this).parent().parent();
    var prev = $(this).parent().parent().prev();
    if (item.attr('id') != prev.attr('id')) {
      // swap classes to retain striping.
      var temp = item.attr('class');
      item.attr('class', prev.attr('class'));
      prev.attr('class', temp);

      // move item
      prev.before(item);
    $('.nodequeue-js-hide').show();
    }
    return false;
  });

  $('a.nodequeue-move-down').click(function() {
    var item = $(this).parent().parent();
    var next = $(this).parent().parent().next();
    if (item.attr('id') != next.attr('id')) {
      // swap classes to retain striping.
      var temp = item.attr('class');
      item.attr('class', next.attr('class'));
      next.attr('class', temp);

      // move item
      next.after(item);
      $('.nodequeue-js-hide').show();
    }
    return false;
  });

  $('a.nodequeue-move-front').click(function() {
    var current = $(this).parent().parent();
    var item = current;
    var prev = $(this).parent().parent().prev();
    while (current.attr('id') != prev.attr('id')) {
      prev.toggleClass('even').toggleClass('odd');
      current = prev;
      prev = $('#' + prev.attr('id')).prev();
    }
    if (item.attr('id') != current.attr('id')) {
      item.removeClass('even').addClass('odd');
      current.before(item);
      $('.nodequeue-js-hide').show();
    }
    return false;
  });

  $('a.nodequeue-move-back').click(function() {
    var current = $(this).parent().parent();
    var item = current;
    var next = $(this).parent().parent().next();
    var count = 0;
    while (current.attr('id') != next.attr('id')) {
      count++;
      next.toggleClass('even').toggleClass('odd');
      current = next;
      next = $('#' + next.attr('id')).next();
    }
    if (item.attr('id') != current.attr('id')) {
      if (count % 2 == 1) {
        item.toggleClass('even').toggleClass('odd');
      }
      current.after(item);
      $('.nodequeue-js-hide').show();
    }
    return false;
  });

  $('a.nodequeue-remove').click(function() {
    var current = $(this).parent().parent();
    var next = $(this).parent().parent().next();
    while (current.attr('id') != next.attr('id')) {
      next.toggleClass('even').toggleClass('odd');
      current = next;
      next = $('#' + next.attr('id')).next();
    }

    $('.nodequeue-js-hide').show();
    $(this).parent().parent().remove();    
    return false;
  });

  $('input.nodequeue-save').click(function() {
    var order = '';
    $('#nodequeue-table tr.nodequeue-row').each(function() {
      if (order != '') {
        order += ',';
      }
      order += $(this).attr('id').replace('nodequeue-row-', '');
    });
    $('#nodequeue-order').val(order);
  });

  $('a.nodequeue-ajax-toggle').click(function() {
    var a = this;
    href = $(this).attr('href');
    $.ajax({
      type: 'POST',
      data: { js: 1 },
      url: href,
      global: true,
      success: function (data) {
        // Parse response
        data = Drupal.parseJson(data);
        // Change text on success
        if (data.status) {
          // Change label back
          $(a).attr('href', data.href);
          $(a).html(data.label);
          return;
        }
      },
    });
    return false;
  });
}

if (Drupal.jsEnabled) {
  $(document).ready(Drupal.nodequeue.autoAttach);
}
