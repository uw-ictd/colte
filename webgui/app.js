var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var fs = require('fs');

// variables loaded through .env file
var locale = process.env.LOCALE || "en";
var transaction_log = process.env.TRANSACTION_LOG || "/var/log/colte/transaction_log.txt";

// main app and view engine setup (we use express)
var app = express();
app.enable('trust proxy');
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'hbs');

app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(express.static(path.join(__dirname, 'public')));

// introduce localization code via a "translate" function
// translation files are found in localize/ directory
var Localize = require('localize');
var myLocalize = new Localize('./localize/');
myLocalize.setLocale(locale);
module.exports.translate = function (x) {
  var str = myLocalize.translate(x);
  return str;
}

// global helper function convertBytes:
// takes int value like "1500" and outputs string "1.0KB"
module.exports.convertBytes = function (size) {
    if (size < 100) {
      return "0.0 KB";
    }
    var i = -1;
    var byteUnits = [' KB', ' MB', ' GB', ' TB']
    do {
          size = size / 1000;
          i++;
        } while (size > 1000 && i < 3);

    return Math.max(size, 0.1).toFixed(1) + byteUnits[i];
};

// global helper function generateIP:
// takes an IP address string, does a bit of error-handling,
// parses localhost IPv6, and returns a string.
module.exports.generateIP = function (ip) {
  if (ip.substr(0,7) == "::ffff:") {
    ip = ip.substr(7)
  } else if (ip.substr(0,3) == "::1") {
    ip = "127.0.0.1"
  }
  return ip;
}

// ensure that we can create/read/open the transaction log file
fs.appendFile(transaction_log, "", function(err) {
  if(err) {
      console.log("Error: Cannot open transaction_log file " + transaction_log);
      process.exit(1);
  }
});

// read JSON file pricing.json; spells out the package deals.
var content = fs.readFileSync("pricing.json");
module.exports.pricing = JSON.parse(content);

// read JSON file with links to free local services (optional)
if (process.env.ENABLED_SERVICES) {
  var file = fs.readFileSync(process.env.ENABLED_SERVICES);
  module.exports.services = JSON.parse(file);
}

// setup routes (i.e. a request for /home goes to /routes/home.js)
fs.readdirSync('./routes').forEach(function(file) {
  if (file.substr(-3) == '.js') {
    var route = require('./routes/' + file);
    var path = file.slice(0, -3);
    app.use('/' + path, route);
  }
});

// homepage: redirect "/" to "/status"
const redirectTo = (req, res, next) => {
  res.redirect('/status');
}
app.use('/', redirectTo);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler (error.hbs)
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error', {
    layout: false,
  });
});

module.exports = app;
