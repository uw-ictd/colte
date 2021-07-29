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

async function ensureLatestMigrations(migration_relative_path) {
  let config = require("./knexfile.js")[env];
  config.connection.database = process.env.DB_NAME;

  // Normalize config migration directory to be an array
  if (!Array.isArray(config.migrations.directory)) {
    config.migrations.directory = [config.migrations.directory];
  }

  // Set the path for all entries
  config.migrations.directory = config.migrations.directory.map(
    (path_i) => migration_relative_path + path_i
  );

  const knex = require("knex")(config);
  await knex.migrate.latest();
}

module.exports.buildCustomer = buildCustomer;
module.exports.ensureLatestMigrations = ensureLatestMigrations;

module.exports.getKnexInstance = require("knex");
module.exports.knexFile = require("./knexfile.js");
