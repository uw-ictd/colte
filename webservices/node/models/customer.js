// database connection
var env = process.env.NODE_ENV || 'development';
var knex = require('knex')(require('../knexfile')[env]);

var customer = {
  all() {
    return knex.select('imsi', 'msisdn', 'raw_down', 'raw_up', 'balance', 'enabled').from('customers');
  },
  find_by_ip(ip) {
    return knex.select('raw_up', 'raw_down', 'balance').where('c.imsi=s.imsi AND s.ip=', ip).from('customers AS c, static_ips AS s');
  },
  find(imsi) {
    return knex.select('raw_up', 'raw_down', 'balance').where('imsi', imsi).from('customers');
  },
  change_enabled(msisdn, isEnabled) {
    return knex.select('enabled').where('msisdn', msisdn).from('customers')
    .catch(function (error) {
      throw new Error(error.sqlMessage);
    })
    .then(function(rows) {
      if (rows.length != 1) {
        console.log("msisdn error");
        throw new Error("msisdn error");
      }
      return rows;
    })
    .then(function(rows) {
      return knex.update({enabled: isEnabled}).where('msisdn', msisdn).from('customers')
      .catch(function(error) {
        throw new Error(error.sqlMessage);
      });
    })
  },
  top_up(msisdn, delta) {
    return knex.select('balance').where('msisdn', msisdn).from('customers')
    .catch(function(error) {
      throw new Error(error.sqlMessage);
    })
    .then(function(rows) {
      if (rows.length != 1) {
        throw new Error("msisdn error");
      }
      return rows;
    })
    .then(function(rows) {
      var newBalance = parseInt(rows[0].balance) + parseInt(delta);
      return knex.update({balance: newBalance}).where('msisdn', msisdn).from('customers')
      .catch(function (error) {
        throw new Error(error.sqlMessage);
      });
    })
  },
  // moves "amount" from the customer with sender_imsi to the customer with receiver_msisdn
  // amount must be non-negative
  // returns promise with no data
  // currently logs success/error to console
  transfer_balance(sender_imsi, receiver_msisdn, amount) {
    function fetch_bals() {
      return knex.select('balance').where('imsi', sender_imsi).from('customers').then((sender_bal) => {
        return knex.select('balance').where('msisdn', receiver_msisdn).from('customers')
          .then((receiver_bal) => { return [sender_bal, receiver_bal]; });
      });
    }
    function transfer_func(trx) {
      return fetch_bals().then((data) => {
        var err = null;
        var sender_bal;
        var receiver_bal;
        if (data[0].length != 1) {
          err = "Sender IMSI matched " + data[0].length + " entries.";
        } else if (data[1].length != 1) {
          err = "Receiver MSISDN matched " + data[1].length + " entries.";
        } else {
          sender_bal = data[0][0].balance;
          receiver_bal = data[1][0].balance;
          if (Number(sender_bal) - Number(amount) < 0) {
            err = "Sender has " + sender_bal + ", tried to send " + Number(amount);
          } else if (Number(amount) < 0) {
            err = "Sender tried to send " + amount + " (must be positive).";
          }
        }
        if (err) {
          return new Promise((res, rej) => { rej(err) });
        }
        sender_bal = Number(sender_bal) - Number(amount);
        receiver_bal = Number(receiver_bal) + Number(amount);
        return trx.update({ balance: sender_bal }).where('imsi', sender_imsi).from('customers')
        .then((unused_data) => {
          // note we're still using the data argument from the fetch_bals promise
          return knex.update({ balance: receiver_bal }).where('msisdn', receiver_msisdn).from('customers')
        })
        .then((data2) => {
          var result = "Transfered " + amount + ". New balances are " + sender_bal + " and " + receiver_bal;
          console.log(result);
        })
      });
    }
    return knex.transaction(transfer_func);
  }
}

module.exports = customer;
