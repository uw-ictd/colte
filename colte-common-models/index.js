"use strict";
const env = process.env.NODE_ENV || "development";

function buildCustomer() {
  let config = require("./knexfile.js")[env];
  config.connection.database = process.env.DB_NAME;
  const knex = require("knex")(config);
  const customer = require("./customer.js");

  customer.register_knex(knex);
  return customer;
}
module.exports.buildCustomer = buildCustomer;

module.exports.getKnexInstance = require("knex");
module.exports.knexFile = require("./knexfile.js");
