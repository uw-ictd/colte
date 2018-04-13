(function() {
  'use strict';

  window.onload = function() {
    $(".balance").hover(function() {
      $(".balance").css('cursor', 'pointer');
    });
    $(".confirm").click(function() {
      var parents = $(this).closest('tr');
      var msisdn = $(parents).attr('id');
      var newBalance = $("#"+msisdn+" .balance").val();
      
      $.post("/admin", {msisdn: msisdn, newBalance: newBalance})
        .done(function(data) {
          alert("Success");
          document.location.reload();
        })
        .fail(function() {
          alert("Failed");
        });
    });
  }

})();