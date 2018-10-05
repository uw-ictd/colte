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
    res.render('transfer', {
      title: 'Home',
      raw_up: data[0].raw_up,
      raw_down: data[0].raw_down,
      balance: data[0].balance,
      admin: data[0].admin
    });
  });
});
  
router.post('/transfer', function(req,res) {

  var ip = req.ip
  var amount = req.body.amount;
  var msisdn = req.body.msisdn;

  if (ip.substr(0,7) == "::ffff:") {
    ip = ip.substr(7)
  } else if (ip.substr(0,3) == "::1") {
    ip = "127.0.0.1"
  }
  customer.find_by_ip(ip).then((data) => {

    customer.transfer_balance(data[0].imsi, msisdn, amount).catch((error) => {
      console.log("Transfer error: " + error);
    })
    .then(function() {
      res.redirect('/transfer');
    });
  });
});

module.exports = router;
