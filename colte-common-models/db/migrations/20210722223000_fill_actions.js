exports.up = function (knex) {
  return knex.transaction((trx) => {
    return trx
      .insert([
        {action: "TRANSFER"},
        {action: "PURCHASE"},
        {action: "ADMIN_TOPUP"},
        {action: "ADMIN_TRANSFER"},
      ])
      .into("actions");
  });
};

exports.down = function (knex) {
  return knex.transaction((trx) => {
    return trx("actions")
      .whereIn("action", ["TRANSFER", "PURCHASE", "ADMIN_TOPUP", "ADMIN_TRANSFER"])
      .del();
  });
};
