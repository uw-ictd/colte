var express = require('express');
var router = express.Router();
var customer = require('../models/customer');

router.get('/', function(req, res, next) {
  var imsi = '910540000000999';//req.ip;
  customer.find(imsi).then((data) => {
    // console.log(data);
    res.render('purchase', {
      title: 'Home',
      raw_up: data[0].raw_up,
      raw_down: data[0].raw_down,
      balance: data[0].balance
    });
  });
});
  
// router.post('/transfer', function(req,res) {
//   var imsi = '910540000000999';
//   var amount = req.body.amount;
//   var msisdn = req.body.msisdn;
//   customer.transfer_balance(imsi, msisdn, amount).catch((error) => {
//     console.log("Transfer error: " + error);
//   });
//   res.redirect('/user');
// });
module.exports = router;
