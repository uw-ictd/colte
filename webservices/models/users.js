// database connection
var env = process.env.NODE_ENV || 'development';
var knex = require('knex')(require('../knexfile')[env]);

var users = knex.select('imsi', 'imei').from('users');

module.exports = users;