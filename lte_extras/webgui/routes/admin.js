var express = require('express');
var router = express.Router();
var customer = require('../models/customer');

router.get('/', function(req, res, next) {
  customer.all().then((data) => {
    res.render('admin', { 
      title: 'Home',
      customers_list: data,
      layout: 'admin_layout',
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

router.post('/enabled', function(req, res) {
  var msisdn = req.body.msisdn;
  var isEnabled = req.body.isEnabled;

  if (isEnabled != 1 && isEnabled != 0) {
    res.status(400).end();
  }
  
  customer.change_enabled(msisdn, isEnabled).then((data) => { 
    console.log(msisdn + "'s enabled is now " + isEnabled);
    res.status(200).end();
  })
  .catch((error) => {
    console.log(error);
    res.status(500).end();
  })
})

module.exports = router;
