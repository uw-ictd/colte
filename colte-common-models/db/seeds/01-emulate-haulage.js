// Right now we break isolation between haulage and CoLTE by directly
// interacting with the haulage tables. In the future this could be better
// mediated by some kind of API...
//
// For the present time, this seed fills the haulage-owned tables with known
// data for development and testing.

exports.seed = function (knex) {
  // Deletes ALL existing entries
  return knex("static_ips")
    .del()
    .then(() => {
      return knex("subscriber_history").del();
    })
    .then(() => {
      return knex("subscribers").del();
    })
    .then(() => {
      return knex("subscribers").insert([
        {
          imsi: "000000000000001",
          data_balance: 100000000,
          bridged: true,
          zero_balance_policy: 2,
          positive_balance_policy: 1,
          current_policy: 2,
        },
        {
          imsi: "000000000000002",
          data_balance: 100000000,
          bridged: true,
          zero_balance_policy: 2,
          positive_balance_policy: 1,
          current_policy: 2,
        },
        {
          imsi: "000000000000003",
          data_balance: 100000000,
          bridged: true,
          zero_balance_policy: 2,
          positive_balance_policy: 1,
          current_policy: 2,
        },
        {
          imsi: "000000000000004",
          data_balance: 100000000,
          bridged: true,
          zero_balance_policy: 3,
          positive_balance_policy: 1,
          current_policy: 2,
        },
        {
          imsi: "000000000000005",
          data_balance: 100000000,
          bridged: true,
          zero_balance_policy: 2,
          positive_balance_policy: 3,
          current_policy: 2,
        },
      ]);
    })
    .then(() => {
      // Inserts seed entries
      return knex("static_ips").insert([
        {imsi: "000000000000001", ip: "127.0.0.1"},
        {imsi: "000000000000002", ip: "192.168.151.2"},
        {imsi: "000000000000003", ip: "192.168.151.3"},
        {imsi: "000000000000004", ip: "192.168.151.4"},
        {imsi: "000000000000005", ip: "192.168.151.5"},
      ]);
    });
};
