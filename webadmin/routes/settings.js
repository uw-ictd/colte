var express = require('express');
var router = express.Router();
var app = require('../app');
var handlebars = require('hbs');

const yaml = require('js-yaml');
const fs   = require('fs');

var toBoolean = function(num) {
  return parseInt(num) === 1
}

var configFile = process.env.SETTINGS_VARS;

handlebars.registerHelper('ifTrue', function(value, options) {
  if(value == true) {
    return options.fn(this);
  }
  return options.inverse(this);
});

router.get('/', function(req, res, next) {
  res.redirect('settings/basic');
});

// the variables file lives at /usr/local/etc/colte/config.yml,
// and you can run the configuration script by running “colteconf update”.

// vars: package/colteconf/usr/bin/colte/roles/configure/vars/main.yml
// script: package/colteconf/usr/bin/colte/roles/configure/tasks/main.yml
router.get('/basic', function(req, res, next) {
  var vars = yaml.safeLoad(fs.readFileSync(configFile, 'utf8'));

  res.render('basic', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("Network Settings"),
    route: "basic",
    basic: "active",

    // basic: network
    enbInterface: vars["enb_iface"],
    enbInterfaceAddress: vars["enb_iface_addr"],
    wanInterface: vars["wan_iface"],
    mcc: vars["mcc"],
    mnc: vars["mnc"],
    maxEnb: vars["max_enb"],
    maxUe: vars["max_ue"],

    // basic: services
    epc: vars["enable_epc"],
    haulage: vars["enable_haulage"],
    webGui: vars["enable_webgui"],
  });
});

router.post('/basic', function(req, res, next) {
  var vars = yaml.safeLoad(fs.readFileSync(configFile, 'utf8'));

  vars['enb_iface'] = req.body["enb-interface"];
  vars['enb_iface_addr'] = req.body["enb-interface-address"];
  vars["wan_iface"] = req.body["wan-interface"];
  vars["mcc"] = req.body["mcc"];
  vars["mnc"] = req.body["mnc"];
  vars["max_enb"] = req.body["max-enb"];
  vars["max_ue"] = req.body["max-ue"];

  vars["enable_epc"] = toBoolean(req.body["epc"]);
  vars["enable_haulage"] = toBoolean(req.body["haulage"]);
  vars["enable_webgui"] = toBoolean(req.body["web-gui"]);

  fs.writeFile(process.env.SETTINGS_VARS, yaml.safeDump(vars), () => {
    next();
  });
});

router.get('/epc', function(req, res, next) {
  var vars = yaml.safeLoad(fs.readFileSync(configFile, 'utf8'));

  res.render('epc', {
    translate: app.translate,
    title: app.translate("Settings"),
    subtitle: app.translate("EPC Settings"),
    route: "epc",
    epc: "active",

    lteSubnet: vars["lte_subnet"],
    localDns: vars["local_dns"],
    dnssec: vars["dnssec"],
    dns: vars["dns"],
    maxUl: vars["max_ul"],
    maxDl: vars["max_dl"]
  });
});

router.post('/epc', function(req, res, next) {
  var vars = yaml.safeLoad(fs.readFileSync(configFile, 'utf8'));

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

// router.get('/web-services', function(req, res, next) {

//   res.render('settings', {
//     translate: app.translate,
//     title: app.translate("Settings"),
//     subtitle: app.translate("Web Services Settings"),
//     route: "web-services",
//     webServices: "active",

//     localDns: vars["local_dns"],

//     mediaServer: vars["media_server"],
//     wikipedia: vars["wikipedia"],
//     mappingServer: vars["mapping_server"],
//     chatServer: vars["chat_server"]
//   });
// });

// router.post('/web-services', function(req, res, next) {
//   vars['media_server'] = toBoolean(req.body["media-server"]);
//   vars['wikipedia'] = toBoolean(req.body["wiki"]);
//   vars["mapping_server"] = toBoolean(req.body["mapping-server"]);
//   vars["chat_server"] = toBoolean(req.body["chat-server"]);

//   fs.writeFile(process.env.SETTINGS_VARS, yaml.safeDump(vars), () => {
//     next();
//   });
// });

module.exports = router;
