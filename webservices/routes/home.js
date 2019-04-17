var express = require('express');
var router = express.Router();
var app = require('../app');
var exec = require('child_process').exec;
const CHECK_STATUS = 0;
const ENABLE = 1;
const DISABLE = 2;

router.get('/', function(req, res, next) {
  res.render('home', {
    translate: app.translate,
    title: app.translate("Home"),
  });
});

router.post('/checkStatus', function(req, res, next) {
  var service = req.body.service;
  console.log("Request Service: " + JSON.stringify(service));  
  exec(getCall(service, CHECK_STATUS), function(err, out, stderr) {

    if (err && err.message.includes("No such file or directory")) {
      res.status(200).send("not installed");
    } else {    
      if (service == "kolibri") {
        if (err) {
          console.log("Error: " + err);
          res.status(200).send("disabled");
        } else {
          res.status(200).send("enabled");
        }
      } else {
        if (err) {
          console.log("Error checking: " + err);
          res.status(500).send("Something went wrong checking the webservices!");
        } else {
          if (out == "enabled") {
            res.status(200).send("enabled");
          } else {
            res.status(200).send("disabled");
          }
        } 
      }
    }
  });
});

router.post('/updateStatus', function(req, res, next) {
  var service = req.body.service;
  var checked = req.body.checked;
  console.log("Request Service: " + JSON.stringify(service));
  console.log("Request Checked: " + JSON.stringify(checked));
  console.log("Toggling Service: " + getCall(service, (checked == "true")? ENABLE : DISABLE));
  exec(getCall(service, (checked == "true") ? ENABLE : DISABLE), function(err, out, stderr) {
    if (err) {
      console.log("Error enabling/disabling: " + err);
      if (err.message.includes("No such file or directory")) {
        res.status(200).send("Not Installed");
      } else {
        res.status(500).send("Something went wrong checking the webservices!");
      }
    } else {
      console.log("Response: " + out); 
      res.status(200).send();
    }
  });
});

function getCall(service, status) {
  if (service == "kolibri") {
    if (status == CHECK_STATUS) {
      return "sudo kolibri status";
    } else if (status == ENABLE) {
      return "sudo kolibri start";
    } else {
      return "sudo kolibri stop";
    }
  } else {
    if (status == CHECK_STATUS) {
      return "sudo systemctl is-enabled " + service;
    } else if (status == ENABLE) {
      return "sudo systemctl enable " + service + " --now";
    } else {
      return "sudo systemctl disable " + service + " --now";
    }
  }
}

module.exports = router;
