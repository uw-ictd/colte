var express = require('express');
var router = express.Router();
var customer = require('../models/customer');

router.get('/', function(req, res, next) {

  var ip = req.ip
  if (ip.substr(0,7) == "::ffff:") {
    ip = ip.substr(7)
  } else if (ip.substr(0,3) == "::1") {
    ip = "127.0.0.1"
  }
  console.log("Web Request From: " + ip)

  customer.find_by_ip(ip).then((data) => {
    // console.log(data);

    // raw_up_str = convertBytes(data[0].raw_up);
    // raw_down_str = convertBytes(data[0].raw_down);
    // data_balance_str = convertBytes(data[0].data_balance);

    res.render('services', {
      title: 'Free Services',
      // raw_up_str: raw_up_str,
      // raw_down_str: raw_down_str,
      // balance: data[0].balance,
      // data_balance_str: data_balance_str,
      // msisdn: data[0].msisdn,
    });
  });
});

module.exports = router;
