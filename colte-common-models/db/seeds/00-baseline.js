
exports.seed = function(knex) {
  // Deletes ALL existing entries
  return knex('static_ips').del()
    .then(() => { return knex("customers").del(); })
    .then(() => {
      return knex('customers').insert([
        {imsi: "000000000000001", username: "localAdmin", raw_down: 0, raw_up: 0, data_balance: 100000000, balance: 0, bridged: true, enabled: true, admin: false, msisdn: "99"},
        {imsi: "000000000000002", username: "User2", raw_down: 0, raw_up: 0, data_balance: 100000000, balance: 2500, bridged: true, enabled: true, admin: false, msisdn: "2"},
        {imsi: "000000000000003", username: "User3", raw_down: 0, raw_up: 0, data_balance: 100000000, balance: 0, bridged: true, enabled: true, admin: false, msisdn: "3"},
        {imsi: "000000000000004", username: "User4", raw_down: 0, raw_up: 0, data_balance: 100000000, balance: 0, bridged: true, enabled: true, admin: false, msisdn: "4"},
        {imsi: "000000000000005", username: "User5", raw_down: 0, raw_up: 0, data_balance: 100000000, balance: 25000000, bridged: true, enabled: true, admin: false, msisdn: "5"}
      ]);
    })
    .then(() => {
      // Inserts seed entries
      return knex('static_ips').insert([
        {imsi: "000000000000001", ip: "127.0.0.1"},
        {imsi: "000000000000002", ip: "192.168.151.2"},
        {imsi: "000000000000003", ip: "192.168.151.3"},
        {imsi: "000000000000004", ip: "192.168.151.4"},
        {imsi: "000000000000005", ip: "192.168.151.5"}
      ]);
    });
};
