var express = require('express');
var router = express.Router();
var customer = require('../models/customer');
var app = require('../app');

router.get('/', function(req, res, next) {
  var ip = app.generateIP(req.ip);
  console.log("Web Request From: " + ip)

  customer.find_by_ip(ip).then((data) => {
    var raw_up_str = app.convertBytes(data[0].raw_up);
    var raw_down_str = app.convertBytes(data[0].raw_down);
    var data_balance_str = app.convertBytes(data[0].data_balance);

    res.render('home', {
      translate: app.translate,
      title: app.translate("Home"),
      raw_up_str: raw_up_str,
      raw_down_str: raw_down_str,
      balance: data[0].balance,
      data_balance_str: data_balance_str,
      msisdn: data[0].msisdn,
      admin: data[0].admin,
    });
  });
});

router.post('/transfer', function(req,res) {
  // var ip = app.generateIP(req.ip);
  var source = req.body.source;
  var dest = req.body.dest;
  var amount = req.body.amount;

  console.log("TRANSFER source=" + source + " dest=" + dest + " amount=" + amount);
  customer.transfer_balance_imsi(source, dest, amount).catch((error) => {
    console.log("Transfer Error: " + error);
  })
  .then(function() {
    res.redirect('/home');
  });
});

router.post('/topup', function(req,res) {
  // var ip = app.generateIP(req.ip);
  var imsi = req.body.imsi;
  var amount = req.body.amount;

  if (amount < 0) {
    console.log("NEGATIVE!!!");
  }

  console.log("TOPUP imsi=" + imsi + ", amount =" + amount);
  customer.top_up(imsi, amount).catch((error) => {
    console.log("Transfer Error: " + error);
  })
  .then(function() {
    res.redirect('/home');
  });
});

router.post('/details', function(req, res, next) {
  var imsi = req.body.imsi;

  console.log("DETAILS" + imsi);
  res.redirect('/details/' + imsi);
});



module.exports = router;
