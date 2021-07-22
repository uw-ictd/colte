var express = require("express");
var router = express.Router();
var colte_models = require("colte-common-models");
var customer = colte_models.buildCustomer();
var app = require("../app");

/* Gets services currently enabled by admin */
function getEnabledServices(servicesObject) {
  var array = [];

  for (var service in servicesObject) {
    if (servicesObject[service].enabled) {
      array.push({
        serviceName: service,
        url: servicesObject[service].url,
      });
    }
  }

  return array;
}

router.get("/", function (req, res, next) {
  var ip = app.generateIP(req.ip);
  console.log("Web Request From: " + ip);
  customer.find_by_ip(ip).then((data) => {
    var enabledServices = getEnabledServices(app.services);

    res.render("services", {
      translate: app.translate,
      title: app.translate("Home"),
      admin: data[0].admin,
      services: enabledServices,
    });
  });
});

module.exports = router;
