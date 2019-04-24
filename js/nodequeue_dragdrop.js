(function ($) {

Backdrop.behaviors.nodequeueDrag = {
  attach: function(context) {
    $('.nodequeue-dragdrop').each(function() {
      var table_id = $(this).attr('id');
      var tableDrag = Backdrop.tableDrag[table_id];

      tableDrag.onDrop = function() {
        $('#' + table_id + ' td.position').each(function(i) {
          $(this).html(i + 1);
        });

        nodequeueUpdateNodePositions(table_id);
      }
    });
  }
};

Backdrop.behaviors.nodequeueWeightChange = {
  attach: function(context) {
    $('.nodequeue-dragdrop tr.draggable select.node-position').bind('change', function() {
      var table_id = $(this).parents('.nodequeue-dragdrop').attr('id');
      var row = $(this).parents('tr');
      var pos = $(this).attr('value') * 1;
      var true_pos = row.parent().children().index(row) + 1;

      var row_after = $('.nodequeue-dragdrop tbody tr:nth-child(' + pos + ')');

      if (pos < true_pos) {
        row_after.before(row);
      }
      else if (pos > true_pos) {
        row_after.after(row);
      }
      else { // Nothing to do, in the same position.
        return;
      }

      nodequeueUpdateNodePositions(table_id);
      nodequeueInsertChangedWarning(table_id);
      nodequeueRestripeTable(table_id);
    });
  }
};

Backdrop.behaviors.nodequeueReverse = {
  attach: function(context) {
    $('#edit-actions-reverse').click(function() {
      var $table = $(this).parent().parent().find('.nodequeue-dragdrop:first');
      var table_id = $table.attr('id');

      // Reverse table rows.
      $table.find('tr.draggable').each(function(i) {
        $table.find('tbody').prepend(this);
      });

      nodequeueUpdateNodePositions(table_id);
      nodequeueInsertChangedWarning(table_id);
      nodequeueRestripeTable(table_id);

      return false;
    });
  }
};

Backdrop.behaviors.nodequeueShuffle = {
  attach: function(context) {
    $('#edit-actions-shuffle').click(function() {
      var $table = $(this).parent().parent().find('.nodequeue-dragdrop:first');
      var table_id = $table.attr('id');

      // Randomize table rows.
      var rows = $('#' + table_id + ' tbody tr:not(:hidden)').get();
      rows.sort(function(){return (Math.round(Math.random())-0.5);});
      $.each(rows, function(i, row) {
        $('#' + table_id + ' tbody').prepend(this);
      });

      nodequeueUpdateNodePositions(table_id);
      nodequeueInsertChangedWarning(table_id);
      nodequeueRestripeTable(table_id);

      return false;
    });
  }
};

Backdrop.behaviors.nodequeueClear = {
  attach: function(context) {
    $('#edit-actions-clear').click(function() {
      var $table = $(this).parent().parent().find('.nodequeue-dragdrop:first');
      var table_id = $table.attr('id');

      // Mark nodes for removal.
      $('#' + table_id + ' .node-position').each(function(i) {
        $(this).val('r');
      });

      // Remove table rows.
      rows = $('#' + table_id + ' tbody tr:not(:hidden)').hide();

      nodequeuePrependEmptyMessage(table_id);
      nodequeueInsertChangedWarning(table_id);

      return false;
    });
  }
};

Backdrop.behaviors.nodequeueRemoveNode = {
  attach: function(context) {
    $('a.nodequeue-remove').click(function() {

      var node_edit = '#' + $(this).attr('id').replace('nodequeue-remove-', 'edit-nodes-') + '-position';
      $(node_edit).val('r');
      // Remove "node-position" class so that position rearrangement in misc/tabledrag.js will ignore this node.
      $(node_edit).removeClass('node-position');

      // Hide the current row.
      $(this).closest('tr').fadeOut('fast', function() {
        var $table = $(this).closest('tr').parent().parent();
        var table_id = $table.attr('id');

        if ($('#' + table_id + ' tbody tr:not(:hidden)').size() == 0) {
          nodequeuePrependEmptyMessage(table_id);
          nodequeueInsertChangedWarning(table_id);
        }
        else {
          nodequeueRestripeTable(table_id)
          nodequeueInsertChangedWarning(table_id);
        }
      });
    });
  }
}

Backdrop.behaviors.nodequeueClearTitle = {
  attach: function(context) {
    $('.subqueue-add-nid').focus(function() {
      if (this.value == this.defaultValue) {
        this.value = '';
        $(this).css('color', '#000');
      }
    }).blur(function() {
      if (!this.value.length) {
        this.value = this.defaultValue;
        $(this).css('color', '#999');
      }
    });
  }
}

/**
 * Updates node positions after nodequeue has been rearranged.
 * It cares about the reverse order and populates nodes the other way round.
 */
function nodequeueUpdateNodePositions(table_id) {
  // Check if reverse option is set.
  var reverse = Backdrop.settings.nodequeue.reverse[table_id.replace(/-/g, '_')];
  var size = reverse ? $('#' + table_id + ' .node-position').size() : 1;

  $('#' + table_id + ' tr').filter(":visible").find('select.node-position').each(function(i) {
    $(this).val(size);
    $(this).find("option[value='" + size + "']").attr('selected', 'selected');
    reverse ? size-- : size++;
  });
}

/**
 * Restripe the nodequeue table after removing an element or changing the
 * order of the elements.
 */
function nodequeueRestripeTable(table_id) {
  $('#' + table_id + ' tbody tr:not(:hidden)')
  .filter(':odd')
    .removeClass('odd').addClass('even')
      .end()
  .filter(':even')
    .removeClass('even').addClass('odd')
      .end();

  $('#' + table_id + ' tr:visible td.position').each(function(i) {
    $(this).html(i + 1);
  });
}

/**
 * Add a row to the nodequeue table explaining that the queue is empty.
 */
function nodequeuePrependEmptyMessage(table_id) {
  $('#' + table_id + ' tbody').prepend('<tr class="odd"><td colspan="6">'+Backdrop.t('No nodes in this queue.')+'</td></tr>');
}

/**
 * Display a warning reminding the user to save the nodequeue.
 */
function nodequeueInsertChangedWarning(table_id) {
  if (Backdrop.tableDrag[table_id].changed == false) {
    $(Backdrop.theme('tableDragChangedWarning')).insertBefore('#' + table_id).hide().fadeIn('slow');
    Backdrop.tableDrag[table_id].changed = true;
  }
}

})(jQuery);
