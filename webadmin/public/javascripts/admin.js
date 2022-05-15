(function () {
  "use strict";

  window.onload = function () {
    $(".balance").hover(function () {
      $(".balance").css("cursor", "pointer");
    });
    $(".confirm").click(function () {
      var parents = $(this).closest("tr");
      var msisdn = $(parents).attr("id");
      var delta = $("#" + msisdn + " .balance").val();

      $.post("/admin/updatebalance", {msisdn: msisdn, delta: delta})
        .done(function (data) {
          alert("Top up successful");
          document.location.reload();
        })
        .fail(function () {
          alert("Top up failed");
          document.location.reload();
        });
    });

    $(".enabled").change(function () {
      var parents = $(this).closest("tr");
      var msisdn = $(parents).attr("id");
      var isChecked = $("#" + msisdn + " .enabled").is(":checked");
      var isEnabled = isChecked ? 1 : 0;
      var message = isChecked ? "Enable" : "Disable";

      $.post("/admin/enabled", {msisdn: msisdn, isEnabled: isEnabled})
        .done(function () {
          alert(message + " successful");
          document.location.reload();
        })
        .fail(function () {
          alert(message + " failed");
          document.location.reload();
        });
    });
  };
})();

$(document).ready(function () {
  $('[data-toggle="popover"]').popover({sanitize: false});

  $(function () {
    $('[data-toggle="popover"]').popover({
      animation: true,
      container: "body",
      trigger: "click focus",
    });
  });

  $("body").click(function (event) {
    if (event.target.classList) {
      var node = event.target.parentNode;
      var hide = false;
      while (node != null) {
        if (node.classList && node.classList.contains("popover")) {
          hide = true;
        }
        node = node.parentNode;
      }

      if ($(event.target).attr("data-toggle") === "popover") {
        hide = true;
      }

      if (!hide) {
        $('[data-toggle="popover"]').popover("hide");
      }
    }
  });

  $('[data-toggle="popover"]').click(function (event) {
    // Close all other popovers and open this one
    $('[data-toggle="popover"]')
      .not("#" + event.target.id)
      .popover("hide");
    $("#" + event.target.id).popover("show");
  });
});

// Populate form matching the imsi with the default (given) information about the username, data balance, balance.
// Update the desired field with user input.
var submit = function (type, imsi, username, dataBalance, balance) {
  document.getElementById(imsi + "-" + type + "-input").value = document.getElementById(
    imsi + "-" + "new-" + type
  ).value;
  submitHelper(imsi, username, dataBalance, balance);
};

var submitHelper = function (imsi, username, dataBalance, balance) {
  if (username) {
    document.getElementById(imsi + "-username-input").value = username;
  }

  if (dataBalance) {
    document.getElementById(imsi + "-data-balance-input").value = dataBalance;
  }

  if (balance) {
    document.getElementById(imsi + "-balance-input").value = balance;
  }

  document.getElementById(imsi + "-submit").click();
};

// Update username
var usernameSubmit = function (imsi) {
  var dataBalance = document.getElementById(imsi + "-data-balance-input").value.trim();
  var balance = document.getElementById(imsi + "-balance-input").value.trim();
  submit("username", imsi, undefined, dataBalance, balance);
};

// Update data balance
var balanceSubmit = function (imsi) {
  var username = document.getElementById(imsi + "-username").textContent.trim();
  var dataBalance = document.getElementById(imsi + "-data-balance-input").value.trim();
  submit("balance", imsi, username, dataBalance, undefined);
};

// Update balance
var dataBalanceSubmit = function (imsi) {
  var username = document.getElementById(imsi + "-username").textContent.trim();
  var balance = document.getElementById(imsi + "-balance-input").value.trim();
  submit("data-balance", imsi, username, undefined, balance);
};

var checkboxSubmit = function (imsi) {
  var username = document.getElementById(imsi + "-username").textContent.trim();
  var balance = document.getElementById(imsi + "-balance-input").value.trim();
  var dataBalance = document.getElementById(imsi + "-data-balance-input").value.trim();
  submitHelper(imsi, username, dataBalance, balance);
};

var policyEditSubmit = function (imsi) {
  var newPositivePolicy = document
    .getElementById(imsi + "-positive-balance-policy-select")
    .value.trim();
  var newZeroPolicy = document.getElementById(imsi + "-zero-balance-policy-select").value.trim();

  document.getElementById(imsi + "-positive-balance-policy-input").value = newPositivePolicy;
  document.getElementById(imsi + "-zero-balance-policy-input").value = newZeroPolicy;
  document.getElementById(imsi + "-submit").click();
  console.log("Submitting policy edit" + imsi);
};
