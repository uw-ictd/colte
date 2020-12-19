var express = require('express');
var router = express.Router();
var customer = require('../models/customer');
var app = require('../app');

router.get('/:user_id', function(req, res, next) {
  console.log("GOTHERE " + req.params.user_id);
  var imsi = req.params.user_id;

  customer.find(imsi).then((data) => {
    var raw_up_str = app.convertBytes(data[0].raw_up);
    var raw_down_str = app.convertBytes(data[0].raw_down);
    var data_balance_str = app.convertBytes(data[0].data_balance);

    res.render('details', {
      translate: app.translate,
      title: app.translate("User Details"),
      raw_up_str: raw_up_str,
      raw_down_str: raw_down_str,
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

