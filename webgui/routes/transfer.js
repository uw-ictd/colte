var express = require('express');
var router = express.Router();
var colte_models = require('colte-common-models');
var customer = colte_models.buildCustomer();
var app = require('../app');
const { body, validationResult } = require('express-validator');

router.get('/', function(req, res, next) {

  var ip = app.generateIP(req.ip);
  console.log("Web Request From: " + ip)

  customer.find_by_ip(ip).then((data) => {
    if (data.length == 0) {
      return res.sendStatus(403);
    } else if (data.length > 1) {
      throw new Error(`Multiple database entries for ${ip}`);
    }

    return res.render('transfer', {
      translate: app.translate,
      title: app.translate('Transfer'),
      raw_up: data[0].raw_up,
      raw_down: data[0].raw_down,
      balance: data[0].balance,
      admin: data[0].admin,
      services: app.services,
    });
  }).catch((error) => {
    console.error(error);
    return res.sendStatus(500);
  });
});

// Root fallback for all other unsupported verbs.
router.all('/', (req, res) => {
  return res.sendStatus(405);
});

router.post(
  '/transfer',
  body("amount").isNumeric(),
  body("msisdn").isNumeric(),
  (req, res) => {
    const errors = validationResult(req);
    if (!errors.isEmpty()){
        return res.status(400).json(errors);
    }
    var ip = app.generateIP(req.ip);
    var amount = req.body.amount;
    var msisdn = req.body.msisdn;

    customer.find_by_ip(ip).then((data) => {
      if (data.length == 0) {
        return res.sendStatus(403);
      } else if (data.length > 1) {
        throw new Error(`Multiple database entries for ${ip}`);
      }

      customer.transfer_balance_msisdn(data[0].imsi, msisdn, amount).catch((error) => {
        console.log("Transfer error: " + error);
      })
      .then(function() {
        res.redirect('/transfer');
      });
    }).catch((error) => {
      console.error(error);
      return res.sendStatus(500);
    });
  }
);

// Transfer fallback for all other unsupported verbs.
router.all('/transfer', (req, res) => {
  return res.sendStatus(405);
});

module.exports = router;
