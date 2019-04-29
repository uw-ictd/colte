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

function getCall(service, status) {
  let callsData = JSON.parse(fs.readFileSync('public/JSONs/calls.json'));  
  console.log("Reading Calls Data " + JSON.stringify(callsData.calls.ourtube.install));
  if (service == "kolibri") {
    if (status == CHECK_STATUS) {
      return "sudo kolibri status";
    } else if (status == ENABLE) {
      return "sudo kolibri start";
    } else if (status == DISABLE) {
      return "sudo kolibri stop";
    } else {
      return "TEMP_DEB=\"$(mktemp)\" && wget -O \"$TEMP_DEB\" 'https://learningequality.org/r/kolibri-deb-latest' && sudo  dpkg -i \"$TEMP_DEB\" && rm -f \"$TEMP_DEB\"";
    }
  } else {
    if (status == CHECK_STATUS) {
      return "sudo systemctl is-enabled " + service;
    } else if (status == ENABLE) {
      return "sudo systemctl enable " + service + " --now";
    } else if (status == DISABLE) {
      return "sudo systemctl disable " + service + " --now";
    } else {
      return "sudo apt-get " + status;
    }
  }
}

module.exports = router;
