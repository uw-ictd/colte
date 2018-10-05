var express = require('express');
var router = express.Router();
var customer = require('../models/customer');

function convertBytes(size) {
	var i = -1;
	var byteUnits = [' kB', ' MB', ' GB', ' TB']
	do {
		size = size / 1000;
		i++;
	} while (size > 1000 && i < 3);

	return Math.max(size, 0.1).toFixed(1) + byteUnits[i];
};

router.get('/', function(req, res, next) {

  var ip = req.ip
  if (ip.substr(0,7) == "::ffff:") {
    ip = ip.substr(7)
  } else if (ip.substr(0,3) == "::1") {
    ip = "127.0.0.1"
  }
  console.log("Web Request From: " + ip)

  customer.find_by_ip(ip).then((data) => {
    // console.log(data);

    var raw_up_str = convertBytes(data[0].raw_up);
    var raw_down_str = convertBytes(data[0].raw_down);
    var data_balance_str = convertBytes(data[0].data_balance);

    res.render('status', {
      title: 'Home',
      raw_up_str: raw_up_str,
      raw_down_str: raw_down_str,
      balance: data[0].balance,
      data_balance_str: data_balance_str,
      msisdn: data[0].msisdn,
      admin: data[0].admin,
    });
  });
});

module.exports = router;
