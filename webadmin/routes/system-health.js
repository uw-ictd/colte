var express = require('express');
var router = express.Router();
var app = require('../app');

const fs = require('fs');

const file = process.env.SYSTEM_HEALTH_INFO;
const NUM_SERVICES = 5;

var getStats = function (callback) {
    return fs.readFile(file, callback);
}

router.get('/', function(req, res, next) {
    res.redirect('/system-health/overview');
});

router.get('/overview', function(req, res, next) {
    var data = {};
    getStats(function(err, data) {
        if (err) {
            throw err;
        }
        
        var dataAsString = data.toString();
        var services = dataAsString.split('-----');
        console.log(services);
    });


    res.render('system-health', {
        translate: app.translate,
        title: app.translate("System Health"),
        overview: "active",
    });
});

router.get('/services', function(req, res, next) {
    res.render('system-health', {
        translate: app.translate,
        title: app.translate("System Health"),
        services: "active",
    });
});



module.exports = router;
