exports.up = function (knex) {
  return knex.transaction((trx) => {
    return trx.insert([{code: "XXX", name: "UNKNOWN", symbol: "$"}]).into("currencies");
  });
};

exports.down = function (knex) {
  return knex.transaction((trx) => {
    return trx("currencies").where("code", "=", "XXX").del();
  });
};
