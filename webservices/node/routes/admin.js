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
  var newBalance = req.body.newBalance;
  console.log(newBalance);
  // call the Model and update the balance

  // res.status(404);
  res.end();
})

module.exports = router;
