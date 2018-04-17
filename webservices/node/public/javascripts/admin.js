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

    $(".activated").change(function () {
      var parents = $(this).closest('tr');
      var msisdn = $(parents).attr('id');
      var isChecked = $("#"+msisdn+" .activated").is(':checked');
      var isActivated = isChecked ? 1 : 0;
      var message = isChecked ? "Activation" : "Deactivation";

      $.post("/admin/activation", {msisdn: msisdn, isActivated: isActivated})
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