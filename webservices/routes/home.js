var express = require('express');
var router = express.Router();
var app = require('../app');
var exec = require('child_process').exec;
var fs = require('fs'); 

const CHECK_STATUS = 0;
const ENABLE = 1;
const DISABLE = 2;
const INSTALL = 3;

router.get('/', function(req, res, next) {
  res.render('home', {
    translate: app.translate,
    title: app.translate("Home"),
  });
});

// Handles calls to update active statuses
router.post('/checkStatus', function(req, res, next) {
  var service = req.body.service;
  console.log("Request Service: " + JSON.stringify(service));  
  exec(getCall(service, CHECK_STATUS), function(err, out, stderr) {
    if (err && err.message.includes("No such file or directory")) {
      res.status(200).send("not installed");
    } else { // Handle Kolibri edge case separately
      if (service == "kolibri") {
        if (err) {
          console.log("Error: " + err);
          res.status(200).send("disabled");
        } else {
          res.status(200).send("enabled");
        }
      } else { // Most cases (debian packages)
        if (err) {
          console.log("Disabling. Error checking: " + err);
          res.status(200).send("disabled");
        } else {
          console.log("Standard service. Output: " + out);
          if (out.includes("enabled")) {
            res.status(200).send("enabled");
          } else {
            res.status(200).send("disabled");
          }
        } 
      }
    }
  });
});

// Handles calls to enable/ disable services
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
        res.status(200).send("not installed");
      } else {
        res.status(500).send("Something went wrong checking the webservices!");
      }
    } else {
      console.log("Response: " + out); 
      res.status(200).send();
    }
  });
});

// Handles calls to install new services
router.post('/install', function(req, res, next) {
  var service = req.body.service;
  console.log("Install Service: " + JSON.stringify(service));
  exec(getCall(service, INSTALL), function(err, out, stderr) {
    if (err) {
      console.log("Error on installation: " + err);
      res.status(500).send("Something went wrong installing this webservice!");
    } else {
      console.log("Response: " + out); 
      res.status(200).send();
    }
  });
});

// Gathers appropriate calls from public/JSONs/calls.json and returns them
function getCall(service, status) {
  let callsData = JSON.parse(fs.readFileSync('public/JSONs/calls.json'));
  // Add install values to JSON file
  if (status == INSTALL) {
    return callsData.calls[service].install
  }

  // Add to this if statement and JSON file for edge cases. Else, handled normally.
  if (service == "kolibri") { // Kolibri Edge case
    if (status == CHECK_STATUS) {
      return callsData.calls[service].check;
    } else if (status == ENABLE) {
      return callsData.calls[service].enable;
    } else {
      return callsData.calls[service].disable;
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
