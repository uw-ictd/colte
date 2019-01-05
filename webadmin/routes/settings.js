var express = require('express');
var router = express.Router();
var app = require('../app');
var handlebars = require('hbs');

const yaml = require('js-yaml');
const fs   = require('fs');

var configFile = process.env.SETTINGS_VARS;
var vars = yaml.safeLoad(fs.readFileSync(configFile, 'utf8'));

handlebars.registerHelper('ifTrue', function(value, options) {
  if(value == true) {
    return options.fn(this);
  }
  return options.inverse(this);
});

router.get('/', function(req, res, next) {
  res.redirect('settings/network');
});

router.post('/', function(req, res, next) {
  console.log("HELLO")
  next();
});

// the variables file lives at /usr/local/etc/colte/config.yml,
// and you can run the configuration script by running “colteconf update”.

// vars: package/colteconf/usr/bin/colte/roles/configure/vars/main.yml
// script: package/colteconf/usr/bin/colte/roles/configure/tasks/main.yml
router.get('/network', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("System Settings"),
    network: "active",

    networkName: vars["network_name"],
    enbInterface: vars["enb_iface"],
    enbInterfaceAddress: vars["enb_iface_addr"],
    wanInterface: vars["wan_iface"],
    lteSubnet: vars["lte_subnet"]
  });
});

router.get('/epc', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("EPC Settings"),
    epc: "active",

    maxEnb: vars["max_enb"],
    maxUe: vars["max_ue"],
    plmn: vars["plmn"],
    local_dns: vars["local_dns"],
    dnssec: vars["dnssec"],
    dns: vars["dns"],
    maxUl: vars["max_dl"],
    maxDl: vars["max_dl"]
  });
});

router.get('/running-services', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Running Services Settings"),
    runningServices: "active",

    epc: vars["epc"],
    haualge: vars["haulage"],
    webGui: vars["web_gui"],
    webServices: vars["web_services"]
  });
});

router.get('/web-services', function(req, res, next) {
  console.log()
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Web Services Settings"),
    webServices: "active",

    mediaServer: vars["media_server"],
    wikipedia: vars["wikipedia"],
    mappingServer: vars["mapping_server"],
    chatServer: vars["chat_server"]
  });
});

module.exports = router;
