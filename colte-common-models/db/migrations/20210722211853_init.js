exports.up = function (knex) {
  return knex.schema
    .createTable("currencies", (table) => {
      table.increments("id").notNullable().primary();
      table.string("code", 3).notNullable().unique();
      table.string("name", 32).notNullable();
      table.string("symbol", 8).notNullable();
    })
    .createTable("customers", (table) => {
      table.increments("id").notNullable().primary();
      table.string("imsi", 16).notNullable().unique();
      table.string("username", 50).notNullable();
      table.decimal("balance", 15, 4).notNullable().defaultTo(0);
      table.integer("currency").notNullable().references("currencies.id");
      table.boolean("enabled").notNullable().defaultTo(true);
      table.boolean("admin").notNullable().defaultTo(false);
      table.string("msisdn", 16).notNullable();
      table.bigInteger("pending_data_balance");
      table.uuid("pending_data_balance_txn");
    })
    .createTable("actions", (table) => {
      table.increments("id").notNullable().primary();
      table.string("action").notNullable();
    })
    .createTable("audit_customers", (table) => {
      table.timestamp("time").notNullable();
      table.integer("customer").notNullable().references("customers.id");
      table.integer("action").notNullable().references("actions.id");
      table.decimal("new_balance", 15, 4).notNullable();
      table.boolean("new_enabled").notNullable();
      table.boolean("new_admin").notNullable();
      table.boolean("new_msisdn").notNullable();
      table.json("action_payload").notNullable();
      table.primary(["time", "customer"]);
    });
};

exports.down = function (knex) {
  return knex.schema
    .dropTable("audit_customers")
    .dropTable("customers")
    .dropTable("currencies")
    .dropTable("actions");
};
