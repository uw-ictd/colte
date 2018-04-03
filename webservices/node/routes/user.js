var express = require('express');
var router = express.Router();
var customer = require('../models/customer');

router.get('/', function(req, res, next) {
  var ip = '10.0.0.42';//req.ip;
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
  // validate phone number
  // validate amount 

  // queries - subtract, add 

  // return status
  console.log(req.body);
  res.redirect('/user');
});
module.exports = router;
