// Right now we break isolation between haulage and CoLTE by directly
// interacting with the haulage tables. In the future this could be better
// mediated by some kind of API...
//
// For the present time, this migration sets up the equivalent tables that would
// be owned by haulage for the sake of integration testing CoLTE in CI.

exports.up = function (knex) {
  return knex.schema
    .alterTable("subscribers", (table) => {
      table.integer("zero_balance_policy").notNullable();
      table.integer("positive_balance_policy").notNullable();
      table.integer("current_policy").notNullable();
    })
    .createTable("access_policies", (table) => {
      table.specificType("id", "INT GENERATED ALWAYS AS IDENTITY").primary();
      table.string("name", 100).notNullable().unique();
    });
};

exports.down = function (knex) {
  return knex.schema.dropTable("access_policies").alterTable("subscribers", (table) => {
    table.dropColumn("zero_balance_policy");
    table.dropColumn("positive_balance_policy");
    table.dropColumn("current_policy");
  });
};
