// database connection
var env = process.env.NODE_ENV || 'development';
var knex = require('knex')(require('../knexfile')[env]);

var customer = {
  all() {
    return knex.select('imsi', 'msisdn', 'ip', 'raw_down', 'raw_up', 'balance').from('customers');
  },
  find(ip) {
    return knex.select('raw_up', 'raw_down', 'balance').where('ip', ip).from('customers');
  },
  create() {

  },
  change_balance(ip, delta) {
    // IP should be switched with IMSI
    async function executor(resolve, reject) {
      var result = {
        balance: -1.0,
        success: false
      };
      await knex.select('balance').where('ip', ip).from('customers').then((data) => {
        if (data.length != 1) return;
        result.balance = data[0].balance;
      });
      // handle no result found better, exception?
      if (result.balance < 0.0) {
        resolve(result);
        return;
      }
      if (result.balance + delta >= 0.0) {
        // should ensure database update success before setting return values?
        result.balance += delta;
        result.success = true;
        await knex.update({ balance: result.balance }).where('ip', ip).from('customers');
      }
      resolve(result);
    }
    return new Promise(executor);
  }
}

module.exports = customer;
