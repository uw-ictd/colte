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

    res.render('status', {
      translate: app.translate,
      title: app.translate("Home"),
      raw_up_str: raw_up_str,
      raw_down_str: raw_down_str,
      balance: data[0].balance,
      data_balance_str: data_balance_str,
      msisdn: data[0].msisdn,
      admin: data[0].admin,
      services: app.services,
    });
  });
});

module.exports = router;
