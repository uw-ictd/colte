const express = require('express');
const webservices = require('../core/webservices');

const router = express.Router();

/* GET home page. */
router.get('/', (req, res, next) => {
  webservices.status()
    .then(status => {
			status.title = 'CoLTE System Status';
			console.log(status);
			res.render('index', status);
    })
    .catch(error => {
			console.log('error');
      res.render('error');
    });

});

module.exports = router;
