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

router.post('/updatebalance', function(req, res) {
  var msisdn = req.body.msisdn;
  var delta = req.body.delta;

  customer.change_balance(msisdn, delta).then((data) => {
    // todo : handle error
    // res.status(404);
  })
  res.end();
})

router.post('/activation', function(req, res) {
  var msisdn = req.body.msisdn;
  var isActivated = req.body.isActivated;
  // console.log(msisdn + " " + isActivated);
  
  customer.change_activation(msisdn, isActivated).then((data) => {
    console.log(data);
  })
  .catch((error) => {
    console.log(error);
  })
  res.end();
})

module.exports = router;
