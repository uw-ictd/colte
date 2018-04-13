var express = require('express');
var router = express.Router();
var customer = require('../models/customer');

router.get('/', function(req, res, next) {
  customer.all().then((data) => {
    res.render('admin', { 
      title: 'Home',
      customers_list: data,
    });
  });
});

router.post('/', function(req, res) {
  var ip = "192.168.151.5";
  var msisdn = req.body.msisdn;
  var newBalance = req.body.newBalance;
  var delta 
  // call the Model and update the balance
  customer.change_balance(ip, newBalance).then((data) => {
    console.log(data);
  })
  // res.status(404);
  res.redirect(req.originalUrl);
  res.end();
})

module.exports = router;
