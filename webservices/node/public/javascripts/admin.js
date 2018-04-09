(function() {
  'use strict';

  window.onload = function() {
    $(".balance").hover(function() {
      $(".balance").css('cursor', 'pointer');
    });
    $(".balance").click(function() {
      console.log($(this).parent()[0].id);
    });
  }

})();