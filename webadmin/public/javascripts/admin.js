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