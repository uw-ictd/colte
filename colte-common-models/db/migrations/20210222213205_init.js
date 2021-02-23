
exports.up = function(knex) {
  return knex.schema
    .createTable("customers", (table) => {
        table.string("imsi", 16).notNullable().primary();
        table.string("username", 50).notNullable();
        table.bigInteger("raw_down").notNullable().unsigned().defaultTo(0);
        table.bigInteger("raw_up").notNullable().unsigned().defaultTo(0);
        table.bigInteger("data_balance").notNullable().defaultTo(10000000);
        table.decimal("balance", 15,4).notNullable().defaultTo(0);
        table.boolean("bridged").notNullable().defaultTo(true);
        table.boolean("enabled").notNullable().defaultTo(true);
        table.boolean("admin").notNullable().defaultTo(false);
        table.string("msisdn", 16).notNullable();
    })
    .createTable("static_ips", (table) => {
        table.string("imsi", 16).notNullable().primary();
        table.string("ip", 16).notNullable().unique();
        table.foreign("imsi").references("customers.imsi");
    });
};

exports.down = function(knex) {
  return knex.schema
    .table("static_ips", (table) => {
        table.dropForeign("imsi");
    })
    .dropTable("customers")
    .dropTable("static_ips")
};
