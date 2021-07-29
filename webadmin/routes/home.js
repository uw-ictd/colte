var express = require("express");
var router = express.Router();
var colte_models = require("colte-common-models");
var customer = colte_models.buildCustomer();
var app = require("../app");

router.get("/", function (req, res, next) {
  res.render("home", {
    translate: app.translate,
    title: app.translate("Home"),
  });
});

router.post("/transfer", function (req, res) {
  var source = req.body.source;
  var dest = req.body.dest;
  var amount = req.body.amount;

  console.log("TRANSFER source=" + source + " dest=" + dest + " amount=" + amount);

  customer
    .admin_transfer_balance(source, dest, amount)
    .catch((error) => {
      console.log("Transfer Error: " + error);
    })
    .then(function () {
      res.redirect("/home");
    });
});

router.post("/topup", function (req, res) {
  var imsi = req.body.imsi;
  var amount = req.body.amount;

  if (amount < 0) {
    console.log("NEGATIVE!!!");
  }

  console.log("TOPUP imsi=" + imsi + ", amount =" + amount);

  customer
    .top_up(imsi, amount)
    .catch((error) => {
      console.log("Transfer Error: " + error);
    })
    .then(function () {
      res.redirect("/home");
    });
});

router.post("/details", function (req, res, next) {
  var imsi = req.body.imsi;

  console.log("DETAILS" + imsi);
  res.redirect("/details/" + imsi);
});

module.exports = router;
