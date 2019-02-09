var express = require('express');
var router = express.Router();
var app = require('../app');

const fs = require('fs');
const shell = require('shelljs');

const file = process.env.SYSTEM_HEALTH_INFO;
const NUM_SERVICES = 5;

router.get('/', function(req, res, next) {
    res.redirect('/system-health/overview');
});

router.get('/overview', function(req, res, next) {
    var toRender = [];
    shell.exec('../scripts/bash/test.sh', (code, stdout, stderr) => { 
        getStats((err, data) => {
            if (err) {
                throw err;
            }
            var dataAsString = data.toString();
            var services = dataAsString.split('-----');
            services = cleanData(services);
    
            services.forEach((element) => {
                var service = {};
                var overview = element[0].split(' - ');
                var loaded = getLoadedData(element[1]);
                var active = getActiveData(element[2]);
    
                service["service"] = overview[0].trim();
                service["name"] = overview[1].trim();
                console.log(loaded[1]);
                service["loaded"] = loaded[1].trim() === 'disabled' ? 'red dot' : 'green dot';
                service["active"] = active[0].trim();
                service["uptime"] = active[1] ? active[1].trim() : active[1];
    
                toRender.push(service);
            });
        });

        res.render('system-health', {
            translate: app.translate,
            title: app.translate("System Health"),
            overview: "active",
            data: toRender
        });
    })
});

router.post('/overview', function(req, res, next) {

});

router.get('/services', function(req, res, next) {
    res.render('system-health', {
        translate: app.translate,
        title: app.translate("System Health"),
        services: "active",
    });
});
var getStats = function (callback) {
    return fs.readFile(file, callback);
}

var getLoadedData = (service) => {
    var index = service.indexOf('loaded');
    service = service.replace('(', '').replace(')', '').substring(index + 'loaded '.length);

    service = service.replace(/\s+/, '').split(";");
    return service;
}

var getActiveData = (service) => {
    var data = [];
    var index = service.indexOf('Active: ');
    service = service.substring(index + 'Active: '.length);
    data.push(service.substring(0, service.indexOf(')') + 1));
    data.push(service.split(';')[1]);

    return data;
}

var cleanData = (services) => {
    services.splice(NUM_SERVICES, services.length - NUM_SERVICES);
        
    services.forEach((element, index) => {
        var cleaned = element.replace('●', '').split('\n').filter(data => data !== '');
        cleaned.forEach((element, i) => {
            var trimmed = element.trim();
            cleaned[i] = trimmed;
        });
        services[index] = cleaned;
    });
    return services;
}


module.exports = router;
