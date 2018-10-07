var express = require('express');
var router = express.Router();
var customer = require('../models/customer');
var app = require('../app');

router.get('/', function(req, res, next) {

  var ip = app.generateIP(req.ip);
  console.log("Web Request From: " + ip)
  customer.find_by_ip(ip).then((data) => {
    var data_balance_str = app.convertBytes(data[0].data_balance);

    res.render('purchase', {
      translate: app.translate,
      title: app.translate('Purchase'),
      data_balance_str: data_balance_str,
      balance: data[0].balance,
      admin: data[0].admin
    });
  });
});
  
router.post('/purchase', function(req,res) {
  var ip = app.generateIP(req.ip);
  var purchase = req.body.package;
  customer.find_by_ip(ip).then((data) => {
    
    if (purchase == 0) {
      var cost = 2560;
      var bytes_purchased = 10485760;
    } else if (purchase == 1) {
      var cost = 25600;
      var bytes_purchased = 104857600;
    } else if (purchase == 2) {
      var cost = 262144;
      var bytes_purchased = 1073741824;
    } else {
      console.log("Invalid PackageNo: " + purchase);
      res.redirect('/purchase');
      return;
    }

    customer.purchase_package(data[0].imsi, cost, bytes_purchased).catch((error) => {
      console.log("Purchase error: " + error);
    })
    .then(function() {
      res.redirect('/purchase');
    });
  });
});

module.exports = router;
