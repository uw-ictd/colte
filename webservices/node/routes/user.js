var express = require('express');
var router = express.Router();
var customer = require('../models/customer');

router.get('/', function(req, res, next) {
  var ip = '192.168.151.2';//req.ip;
  customer.find(ip).then((data) => {
    // console.log(data);
    res.render('user', {
      title: 'Home',
      raw_up: data[0].raw_up,
      raw_down: data[0].raw_down,
      balance: data[0].balance
    });
  });
});
  
router.post('/transfer', function(req,res) {
  var ip = '192.168.151.2';
  var amount = req.body.amount;
  var msisdn = req.body.msisdn;
  customer.transfer_balance(ip, msisdn, amount).catch((error) => {
    console.log("Transfer error: " + error);
  });
  res.redirect('/user');
});
module.exports = router;
