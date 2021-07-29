var express = require("express");
var router = express.Router();
var colte_models = require("colte-common-models");
var customer = colte_models.buildCustomer();
var app = require("../app");

router.get("/:user_id", function (req, res, next) {
  console.log("GOTHERE " + req.params.user_id);
  var imsi = req.params.user_id;

  customer.find(imsi).then((data) => {
    var data_balance_str = app.convertBytes(data[0].data_balance);

    res.render("details", {
      translate: app.translate,
      title: app.translate("User Details"),
      balance: data[0].balance,
      data_balance_str: data_balance_str,
      msisdn: data[0].msisdn,
      imsi: imsi,
      bridged: data[0].bridged,
      admin: data[0].admin,
    });
  });
});

module.exports = router;
