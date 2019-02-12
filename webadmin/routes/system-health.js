var express = require('express');
var router = express.Router();
var app = require('../app');

const fs = require('fs');
const shell = require('shelljs');

const SERVICES_FILE = process.env.SERVICES_HEALTH_INFO;
const SEARCH_FILE = process.env.SERVICES_SEARCH_INFO;
const SYSTEM_HEALTH_FILE = process.env.SYSTEM_HEALTH_INFO;
const NUM_SERVICES = 5;

router.get('/', function(req, res, next) {
    res.redirect('/system-health/overview');
});

router.get('/services', function(req, res, next) {
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

router.post('/services', function(req, res, next) {
    var toRender = [];
    var service = req.body.service;
    var command = 'systemctl status ' + service + ' > ' + SEARCH_FILE;

    // shell.exec(command, (code, stdout, stderr) => { 

    // });

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

router.get('/overview', function(req, res, next) {
    const cpu = ["model name", "cpu MHz", "cache size", "cpu cores"];
    const memory = ["MemTotal", "MemFree", "MemAvailable", "Active", "Inactive"];
    const disk = ['/'];

    shell.exec('../scripts/bash/system-health.sh', (code, stdout, stderr) => { 
        getStats(SYSTEM_HEALTH_FILE, (err, data) => {
            if (err) {
                throw err;
            }
            var cpuData = [];
            var memoryData = [];
            var diskData = [];

            var dataAsString = data.toString();
            var delimited = dataAsString.split("-----");
            
            cpuData = getCpuMemoryData(delimited[0], cpu);
            memoryData = getCpuMemoryData(delimited[1], memory);
            diskData = getDiskData(delimited[2], disk);

            res.render('system-health', {
                translate: app.translate,
                title: app.translate("System Health"),
                services: "active",

                cpuData: cpuData,
                memoryData: memoryData,
                diskData: diskData
            });
        });
    });
});

var getCpuMemoryData = (all, filter) => {
    var data = all.split("\n");
    var matched = [];

    data.forEach((element) => {
        if (element !== '') {
            var line = element.split(":");
            var type = capitalize(line[0].trim().replace("_", " "));
            var value = line[1].trim();
            if (filter.includes(line[0].trim())) {
                matched.push({type: type, value: value});
            }
        }
    });

    return matched;
}

var getDiskData = (all, filter) => {
    var data = all.split("\n");
    var matched = [];

    for(let i = 1; i < data.length; i++) {
        var line = data[i].split((/\s+/));
        if (filter.includes(line[5].trim())) {
            var obj = {
                filesystem: line[0].trim(),
                blocks: line[1].trim(),
                used: line[2].trim(),
                available: line[3].trim(),
                usePercent: line[4].trim(),
                mounted: line[5].trim()
            }
            matched.push(obj);
        }
    }
    console.log(matched);

    return matched;
}

var capitalize = (str) => {
    var words = str.split(" ");
    var result = "";

    words.forEach(element => {
        var temp = element.trim();
        if (temp.toLowerCase() === 'id' || temp === 'cpu') {
            result += (temp.toUpperCase() + " ");
        } else {
            result += (temp.charAt(0).toUpperCase() + temp.substring(1) + " ");
        }
    });

    return result.trim();
}

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
