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

  customer.top_up(msisdn, delta).then((data) => {
    console.log(msisdn + "'s balance is update");
    res.status(200).end();
  })
  .catch((error) => {
    console.log(error);
    res.status(500).end();
  })
})

router.post('/activation', function(req, res) {
  var msisdn = req.body.msisdn;
  var isActivated = req.body.isActivated;

  if (isActivated != 1 && isActivated != 0) {
    res.status(400).end();
  }
  
  customer.change_activation(msisdn, isActivated).then((data) => { 
    console.log(msisdn + "'s activation is now " + isActivated);
    res.status(200).end();
  })
  .catch((error) => {
    console.log(error);
    res.status(500).end();
  })
})

module.exports = router;
