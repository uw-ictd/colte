var express = require('express');
var router = express.Router();
var app = require('../app');

router.get('/', function(req, res, next) {
  res.redirect('settings/network');
});

router.get('/network', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("System Settings"),
    network: "active"
  });
});

router.get('/epc', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("EPC Settings"),
    epc: "active"
  });
});

router.get('/running-services', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Running Services Settings"),
    runningServices: "active"
  });
});

router.get('/web-services', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Web Services Settings"),
    webServices: "active"
  });
});

module.exports = router;
