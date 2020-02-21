var express = require('express');
var router = express.Router();
var customer = require('../models/customer');
var app = require('../app');

router.get('/', function(req, res, next) {

  var ip = app.generateIP(req.ip);
  console.log("Web Request From: " + ip)

  customer.find_by_ip(ip).then((data) => {
    res.render('transfer', {
      translate: app.translate,
      title: app.translate('Transfer'),
      raw_up: data[0].raw_up,
      raw_down: data[0].raw_down,
      balance: data[0].balance,
      admin: data[0].admin,
      services: app.services,
    });
  });
});
  
router.post('/transfer', function(req,res) {

  var ip = app.generateIP(req.ip);
  var amount = req.body.amount;
  var msisdn = req.body.msisdn;

  customer.find_by_ip(ip).then((data) => {

    customer.transfer_balance_msisdn(data[0].imsi, msisdn, amount).catch((error) => {
      console.log("Transfer error: " + error);
    })
    .then(function() {
      res.redirect('/transfer');
    });
  });
});

module.exports = router;
