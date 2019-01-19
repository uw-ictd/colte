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

// the variables file lives at /usr/local/etc/colte/config.yml,
// and you can run the configuration script by running “colteconf update”.

// vars: package/colteconf/usr/bin/colte/roles/configure/vars/main.yml
// script: package/colteconf/usr/bin/colte/roles/configure/tasks/main.yml
router.get('/network', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Basic Settings"),
    route: "network",
    network: "active",
    // basic: network
    enbInterface: vars["enb_iface"],
    enbInterfaceAddress: vars["enb_iface_addr"],
    wanInterface: vars["wan_iface"],
    mcc: vars["mcc"],
    mnc: vars["mnc"],
    maxEnb: vars["max_enb"],
    maxUe: vars["max_ue"],
    // basic: services
    epc: vars["epc"],
    haulage: vars["haulage"],
    webGui: vars["web_gui"],
  });
});

router.post('/network', function(req, res, next) {
  vars['enb_iface'] = req.body["enb-interface"];
  vars['enb_iface_addr'] = req.body["enb-interface-address"];
  vars["wan_iface"] = req.body["wan-interface"];
  vars["mcc"] = req.body["mcc"];
  vars["mnc"] = req.body["mnc"];
  vars["max_enb"] = req.body["max-enb"];
  vars["max_ue"] = req.body["max-ue"];

  fs.writeFile(process.env.SETTINGS_VARS, yaml.safeDump(vars), () => {
    next();
  });
});

router.get('/epc', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("EPC Settings"),
    route: "epc",
    epc: "active",

    localDns: vars["local_dns"],
    dnssec: vars["dnssec"],
    dns: vars["dns"],
    maxUl: vars["max_ul"],
    maxDl: vars["max_dl"]
  });
});

router.post('/epc', function(req, res, next) {
  vars['max_enb'] = parseInt(req.body["max-enb"]);
  vars['max_ue'] = parseInt(req.body["max-ue"]);
  vars["plmn"] = req.body["plmn"];
  vars["local_dns"] = toBoolean(req.body["dns"]);
  vars["max_dl"] = parseInt(req.body["max-dl"]);
  vars["max_ul"] = parseInt(req.body["max-ul"]);

  // Disable all web services if local DNS disabled
  if (!vars["local_dns"]) {
    vars["dnssec"] = req.body["dnssec-address"];
    vars["dns"] = req.body["dns-address"];

    vars["media_server"] = false;
    vars["mapping_server"] = false;
    vars["chat_server"] = false;
    vars["wikipedia"] = false;
  }

  fs.writeFile(process.env.SETTINGS_VARS, yaml.safeDump(vars), () => {
    next();
  });
});

router.get('/running-services', function(req, res, next) {
  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Running Services Settings"),
    route: "running-services",
    runningServices: "active",


    webServices: vars["web_services"]
  });
});

router.post('/running-services', function(req, res, next) {
  vars['epc'] = toBoolean(req.body["epc"]);
  vars['haulage'] = toBoolean(req.body["haulage"]);
  vars["web_gui"] = toBoolean(req.body["web-gui"]);
  vars["web_services"] = toBoolean(req.body["web-services"]);

  fs.writeFile(process.env.SETTINGS_VARS, yaml.safeDump(vars), () => {
    next();
  });
});

router.get('/web-services', function(req, res, next) {

  res.render('settings', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Web Services Settings"),
    route: "web-services",
    webServices: "active",

    localDns: vars["local_dns"],

    mediaServer: vars["media_server"],
    wikipedia: vars["wikipedia"],
    mappingServer: vars["mapping_server"],
    chatServer: vars["chat_server"]
  });
});

router.post('/web-services', function(req, res, next) {
  vars['media_server'] = toBoolean(req.body["media-server"]);
  vars['wikipedia'] = toBoolean(req.body["wiki"]);
  vars["mapping_server"] = toBoolean(req.body["mapping-server"]);
  vars["chat_server"] = toBoolean(req.body["chat-server"]);

  fs.writeFile(process.env.SETTINGS_VARS, yaml.safeDump(vars), () => {
    next();
  });
});

var toBoolean = function(num) {
  return parseInt(num) === 1
}

module.exports = router;
