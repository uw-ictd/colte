exports.seed = function (knex) {
  // Deletes ALL existing entries
  return knex("audit_customers")
    .del()
    .then(() => {
      return knex("audit_customers").del();
    })
    .then(() => {
      return knex("customers").del();
    })
    .then(() => {
      return knex("currencies").del();
    })
    .then(() => {
      return knex("actions").del();
    })
    .then(() => {
      // Inserts seed entries
      return knex("currencies").insert([
        {code: "USD", name: "US Dollars", symbol: "$"},
        {code: "IDR", name: "Indonesian Rupiah", symbol: "Rp"},
      ]);
    })
    .then(() => {
      return knex("currencies")
        .where({code: "USD"})
        .select("id")
        .then((id) => {
          const unpacked_id = id[0].id;
          return knex("customers").insert([
            {
              imsi: "000000000000001",
              username: "User1",
              balance: 0,
              currency: unpacked_id,
              enabled: true,
              admin: false,
              msisdn: "1",
            },
            {
              imsi: "000000000000002",
              username: "User2",
              balance: 2500,
              currency: unpacked_id,
              enabled: true,
              admin: false,
              msisdn: "2",
            },
            {
              imsi: "000000000000003",
              username: "User3",
              balance: 0,
              currency: unpacked_id,
              enabled: true,
              admin: false,
              msisdn: "3",
            },
            {
              imsi: "000000000000004",
              username: "User4",
              balance: 0,
              currency: unpacked_id,
              enabled: true,
              admin: false,
              msisdn: "4",
            },
            {
              imsi: "000000000000005",
              username: "User5",
              balance: 25000000,
              currency: unpacked_id,
              enabled: true,
              admin: false,
              msisdn: "5",
            },
          ]);
        });
    });
};
