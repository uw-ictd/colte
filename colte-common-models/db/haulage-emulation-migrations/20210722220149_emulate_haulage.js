// Right now we break isolation between haulage and CoLTE by directly
// interacting with the haulage tables. In the future this could be better
// mediated by some kind of API...
//
// For the present time, this migration sets up the equivalent tables that would
// be owned by haulage for the sake of integration testing CoLTE in CI.

exports.up = function (knex) {
  return knex.schema
    .createTable("subscribers", (table) => {
      table.specificType("internal_uid", "INT GENERATED ALWAYS AS IDENTITY").primary();
      table.string("imsi", 16).notNullable().unique();
      table.bigInteger("data_balance").notNullable().defaultTo(10000000);
      table.boolean("bridged").notNullable().defaultTo(true);
    })
    .createTable("subscriber_history", (table) => {
      table.integer("subscriber").notNullable().references("subscribers.internal_uid");
      table.timestamp("time").notNullable();
      table.bigInteger("data_balance").notNullable();
      table.boolean("bridged").notNullable();
      table.primary(["subscriber", "time"]);
    })
    .createTable("static_ips", (table) => {
      table.specificType("ip", "inet NOT NULL").primary();
      table.string("imsi", 16).notNullable().references("subscribers.imsi");
    });
};

exports.down = function (knex) {
  return knex.schema
    .dropTable("static_ips")
    .dropTable("subscriber_history")
    .dropTable("subscribers");
};
