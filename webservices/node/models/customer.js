// database connection
var env = process.env.NODE_ENV || 'development';
var knex = require('knex')(require('../knexfile')[env]);

var customer = {
  all() {
    return knex.select('imsi', 'msisdn', 'ip', 'raw_down', 'raw_up', 'balance').from('customers');
  },
  find(ip) {
    return knex.select('raw_up', 'raw_down', 'balance').where('ip', ip).from('customers');
  },
  create() {

  }
}

module.exports = customer;