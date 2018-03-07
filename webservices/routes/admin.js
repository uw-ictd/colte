var express = require('express');
var router = express.Router();
var users = require('../models/users');

router.get('/', function(req, res, next) {
  var list = users;
  users.then((data) => {
    res.render('admin', { 
      title: 'Home',
      user_list: data,
    });
  });
});

module.exports = router;
