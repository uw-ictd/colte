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

  // authorize the request?

  // validate msisdn and isActivated
  // throw res.status(400) if parameters are invalid
  
  customer.change_activation(msisdn, isActivated).then((data) => { 
    // data is the number of row affected

    // need to be printed in log file
    console.log(msisdn + "'s activation is now " + isActivated);

    res.status(200).end();
  })
  .catch((error) => {
    // need to be printed in log file
    console.log(error.code);
    console.log(error.sqlMessage);
    
    res.status(500).end();
  })
})

module.exports = router;
