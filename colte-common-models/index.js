'use strict'

const Customer = require('./customer.js');
console.log("Importing common lib index file");

const TestKnex = require('knex')(require('./knexfile')["test"]);

module.exports.Customer = Customer;
module.exports.TestKnex = TestKnex;
