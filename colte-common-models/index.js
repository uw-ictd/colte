'use strict'
const env = process.env.NODE_ENV || 'development';

function buildCustomer() {
    console.log("Am building");
    console.log(process.env.DB_NAME);
    const knex = require('knex')({
        client: 'mysql',
        connection: {
          host: "localhost",
          database: process.env.DB_NAME,
          user: process.env.DB_USER,
          password: process.env.DB_PASSWORD
        }
      });
    const customer = require('./customer.js');

    customer.register_knex(knex)
    return customer
}
module.exports.buildCustomer = buildCustomer;

module.exports.getKnexInstance = require('knex');
module.exports.knexFile = require("./knexfile.js");