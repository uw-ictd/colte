(function() {
  'use strict';

  window.onload = function() {
    $(".balance").hover(function() {
      $(".balance").css('cursor', 'pointer');
    });
    $(".confirm").click(function() {
      var parents = $(this).closest('tr');
      var msisdn = $(parents).attr('id');
      var delta = $("#"+msisdn+" .balance").val();
      
      $.post("/admin/updatebalance", {msisdn: msisdn, delta: delta})
        .done(function(data) {
          alert("Top up successful");
          document.location.reload();
        })
        .fail(function() {
          alert("Top up failed");
          document.location.reload();
        });
    });

    $(".enabled").change(function () {
      var parents = $(this).closest('tr');
      var msisdn = $(parents).attr('id');
      var isChecked = $("#"+msisdn+" .enabled").is(':checked');
      var isEnabled = isChecked ? 1 : 0;
      var message = isChecked ? "Enable" : "Disable";

      $.post("/admin/enabled", {msisdn: msisdn, isEnabled: isEnabled})
        .done(function() {
          alert(message + " successful");
          document.location.reload();
        })
        .fail(function() {
          alert(message + " failed");
          document.location.reload();
        })
    });
  }
})();

$(document).ready(function() {
  $('[data-toggle="popover"]').popover();

  $(function() {
    $('[data-toggle="popover"]').popover({
      animation: true,
      container: 'body',
      trigger: 'click focus'
    });
  });

  $('body').click(function(event) {
    if (event.target.classList) {
      var node = event.target.parentNode;
      var hide = false;
      while (node != null) {
          if (node.classList && node.classList.contains('popover')) {
            hide = true;
          }
          node = node.parentNode;
      }

      if ($(event.target).attr('data-toggle') === 'popover') {
        hide = true;
      }

      if (!hide) {
        $('[data-toggle="popover"]').popover('hide');
      }
    }
  });

  $('[data-toggle="popover"]').click(function(event) {
    $(event.target.id).popover('show');
  });
});

var submit = function(imsi) {
  console.log(imsi);
  document.getElementById(imsi + '-submit').click();
}