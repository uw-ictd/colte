var express = require('express');
var router = express.Router();
var colte_models = require('colte-common-models');
var customer = colte_models.Customer;
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
      admin: data[0].admin,
      pack: app.pricing.packages,
      services: app.services,
    });
  });
});

// Root fallback for all other unsupported verbs.
router.all('/', (req, res) => {
  return res.sendStatus(405);
});

router.post('/purchase', function(req,res) {
  var ip = app.generateIP(req.ip);
  var bytes = req.body.package;
  var cost = 0;
  customer.find_by_ip(ip).then((data) => {
    
    for (var i in app.pricing.packages) {
      if (app.pricing.packages[i].bytes == bytes) {
        cost = app.pricing.packages[i].cost;
        break;
      }
    }
    // handle no match here
    if (cost == 0) {
      console.log("Package Not Found?!?");
      res.redirect('/purchase');
      return;
    }

    customer.purchase_package(data[0].imsi, cost, bytes).catch((error) => {
      console.log("Purchase error: " + error);
    })
    .then(function() {
      res.redirect('/purchase');
    });
  }).catch(() => {
    return res.sendStatus(403);
  });
});

// Purchase fallback for all other unsupported verbs.
router.all('/purchase', (req, res) => {
  return res.sendStatus(405);
});

module.exports = router;
