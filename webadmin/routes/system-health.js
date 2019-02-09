var express = require('express');
var router = express.Router();
var app = require('../app');

const fs = require('fs');
const shell = require('shelljs');

const SERVICES_FILE = process.env.SERVICES_HEALTH_INFO;
const SEARCH_FILE = process.env.SERVICES_SEARCH_INFO;
const NUM_SERVICES = 5;

router.get('/', function(req, res, next) {
    res.redirect('/system-health/overview');
});

router.get('/overview', function(req, res, next) {
    var toRender = [];
    shell.exec('../scripts/bash/CHANGEME', (code, stdout, stderr) => { 
        getStats(SERVICES_FILE, (err, data) => {
            if (err) {
                throw err;
            }
            getPageData(data, toRender);
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
    var toRender = [];
    var service = req.body.service;
    var command = 'systemctl status ' + service + ' > ' + SEARCH_FILE;
    
    // shell.exec(command, (code, stdout, stderr) => { 

    // });

    console.log(command);
    getStats(SEARCH_FILE, (err, data) => {
        if (err) {
            throw err;
        }
        getPageData(data, toRender);

        res.render('system-health', {
            translate: app.translate,
            title: app.translate("System Health"),
            overview: "active",
            data: toRender
        });
    });
});

router.get('/services', function(req, res, next) {
    res.render('system-health', {
        translate: app.translate,
        title: app.translate("System Health"),
        services: "active",
    });
});

var getPageData = (data, toRender) => {
    var dataAsString = data.toString();
    if (!dataAsString.includes("could not be found")) {
        var services = dataAsString.split('-----');
        services = cleanData(services);
    
        services.forEach((element) => {
            var service = getServiceData(element);
            toRender.push(service);
        });
    }
}

var getServiceData = (element) => {
    var service = {};
    var overview = element[0].split(' - ');
    var loaded = getLoadedData(element[1]);
    var active = getActiveData(element[2]);

    service["service"] = overview[0].trim();
    service["name"] = overview[1].trim();
    service["loaded"] = loaded[1].trim() === 'disabled' ? 'red dot' : 'green dot';
    service["active"] = active[0].trim();
    service["uptime"] = active[1] ? active[1].trim() : active[1];

    return service;
}

var getStats = function (file, callback) {
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
