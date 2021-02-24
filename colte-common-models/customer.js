// database connection will be injected externally.
let knex = null;
const { attachPaginate } = require('knex-paginate');
attachPaginate();
var fs = require('fs');
var dateTime = require('date-time');
var transaction_log = process.env.TRANSACTION_LOG || "/var/log/colte/transaction_log.txt";

function transfer_balance_impl(sender_imsi, receiver_imsi, amount, kind) {
  function fetch_bals(trx) {
    return trx.select(
      'balance'
    ).forUpdate(
    ).where(
      'imsi', sender_imsi
    ).from(
      'customers'
    ).then((sender_bal) => {
      return trx.select(
        'balance'
      ).where(
        'imsi', receiver_imsi
      ).from(
        'customers'
      ).then((receiver_bal) => {
        return [sender_bal, receiver_bal];
      });
    });
  }

  function transfer_func(trx) {
    return fetch_bals(
      trx
    ).then((data) => {
      var err = null;
      var sender_bal;
      var receiver_bal;
      if (data[0].length != 1) {
        err = "Sender IMSI matched " + data[0].length + " entries.";
      } else if (data[1].length != 1) {
        err = "Receiver IMSI matched " + data[1].length + " entries.";
      } else if (sender_imsi == receiver_imsi) {
        err = "Attempting an invalid transfer";
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

      return trx.update(
        { balance: sender_bal }
      ).where(
        'imsi', sender_imsi
      ).from(
        'customers'
      ).then((unused_data) => {
        // note we're still using the data argument from the fetch_bals promise
        return trx.update({ balance: receiver_bal }).where('imsi', receiver_imsi).from('customers')
      }).then((data2) => {
        var result = "Transfered " + amount + ". New balances are " + sender_bal + " and " + receiver_bal;
        console.log(result);

        fs.appendFile(
          transaction_log, dateTime() + " " + kind + " " + sender_imsi + " " + receiver_imsi + " " + amount + "\n",
          function(err) {
            if(err) {
              return console.log(err);
            }
          }
        );
      })
    });
  }

  return knex.transaction(transfer_func);
}

var customer = {
  register_knex(knex_instance) {
    knex = knex_instance;
  },

  all(page) {
    return knex.select(
      'imsi', 'msisdn', 'raw_down', 'raw_up', 'balance', 'data_balance', 'enabled', 'bridged', 'admin', 'username'
    ).from(
      'customers'
    ).paginate(
      {perPage: 10, currentPage: page, isLengthAware: true}
    );
  },

  find_by_ip(ip) {
    return knex.select(
      'customers.imsi', 'raw_up', 'raw_down', 'balance', 'data_balance', 'msisdn', 'admin', 'username'
    ).from(
      'customers'
    ).join(
      'static_ips', "customers.imsi", "=", "static_ips.imsi"
    ).where(
      'static_ips.ip', ip
    );
  },

  find(imsi) {
    return knex.select(
      'imsi', 'raw_up', 'raw_down', 'balance', 'data_balance', 'msisdn', 'bridged', 'enabled'
    ).where(
      'imsi', imsi
    ).from(
      'customers'
    );
  },

  update(imsi, bridged, enabled, balance, data_balance, username) {
    return knex.update(
      {balance: balance, data_balance: data_balance, bridged: bridged, enabled: enabled, username: username}
    ).where(
      'imsi', imsi
    ).from(
      'customers'
    ).catch(function(error) {
      throw new Error(error.sqlMessage);
    });
  },

  change_enabled(msisdn, isEnabled) {
    return knex.select(
      'enabled'
    ).where(
      'msisdn', msisdn
    ).from(
      'customers'
    ).catch(function (error) {
      throw new Error(error.sqlMessage);
    }).then(
      function(rows) {
        if (rows.length != 1) {
          console.log("msisdn error");
          throw new Error("msisdn error");
        }
        return rows;
      }
    ).then(function(rows) {
      return knex.update(
        {enabled: isEnabled}
      ).where(
        'msisdn', msisdn
      ).from(
        'customers'
      ).catch(function(error) {
        throw new Error(error.sqlMessage);
      });
    })
  },

  top_up(imsi, delta) {
    return knex.select(
      'balance'
    ).where(
      'imsi', imsi
    ).from(
      'customers'
    ).catch(function(error) {
      throw new Error(error.sqlMessage);
    }).then(function(rows) {
      if (rows.length != 1) {
        throw new Error("imsi error");
      }
      return rows;
    }).then(function(rows) {
      var newBalance = parseInt(rows[0].balance) + parseInt(delta);
      var rval = knex.update(
        {balance: newBalance}
      ).where(
        'imsi', imsi
      ).from(
        'customers'
      ).catch(function (error) {
        throw new Error(error.sqlMessage);
      });

      fs.appendFile(
        transaction_log, dateTime() + " TOPUP " + imsi + " " + delta + "\n",
        function(err) {
          if(err) {
            return console.log(err);
          }
        }
      );
      return rval;
    })
  },

  // moves "amount" from the customer with sender_imsi to the customer with receiver_imsi
  // amount must be non-negative
  // returns promise with no data
  // currently logs success/error to console
  transfer_balance(sender_imsi, receiver_imsi, amount) {
    return transfer_balance_impl(sender_imsi, receiver_imsi, amount, "USERTRANSFER");
  },

  admin_transfer_balance(sender_imsi, receiver_imsi, amount) {
    return transfer_balance_impl(sender_imsi, receiver_imsi, amount, "ADMINTRANSFER");
  },

  transfer_balance_msisdn(sender_imsi, receiver_msisdn, amount) {
    return knex.select(
      'imsi'
    ).where(
      'msisdn', receiver_msisdn
    ).from(
      'customers'
    ).catch(function(error) {
      throw new Error(error.sqlMessage);
    }).then(function(rows) {
      if (rows.length != 1) {
        throw new Error("msisdn error");
      }
      return rows;
    }).then(function(rows) {
      return customer.transfer_balance(sender_imsi, rows[0].imsi, amount);
    })
  },

// ASSUMPTION: all three of these values are already sanitized/validated.
// We can cancel the transaction if (for some reason) the user doesn't
// have enough funds, otherwise no logic is really needed.
  purchase_package(imsi, cost, data) {

    function purchase_func(trx) {
      console.log("IMSI = " + imsi + " cost = " + cost + " data = " + data);
      return trx.select(
        'balance', 'data_balance'
      ).forUpdate(
      ).where(
        'imsi', imsi
      ).from(
        'customers'
      ).catch(function(error) {
        throw new Error(error.sqlMessage);
      }).then(function(rows) {
        if (rows.length != 1) {
          throw new Error("IMSI error");
        }
        return rows;
      }).then(function(rows) {
        var newBalance = parseInt(rows[0].balance) - parseInt(cost);
        var newData = parseInt(rows[0].data_balance) + parseInt(data);

        if (newBalance < 0) {
          throw new Error("Insufficient funds for transfer!");
        }

        var rval = trx.update(
          {balance: newBalance, data_balance: newData}
        ).where(
          'imsi', imsi
        ).from(
          'customers'
        ).catch((error) => {
          throw new Error(error.sqlMessage);
        });

        fs.appendFile(
          transaction_log,
          dateTime() + " PURCHASE " + imsi + " " + data + " " + cost + "\n",
          function(err) {
            if(err) {
              return console.log(err);
            }
          });
        return rval;
      });
    }

    return knex.transaction(purchase_func);
  }
}

module.exports = customer;
