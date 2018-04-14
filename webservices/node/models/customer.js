// database connection
var env = process.env.NODE_ENV || 'development';
var knex = require('knex')(require('../knexfile')[env]);

var customer = {
  all() {
    return knex.select('imsi', 'msisdn', 'ip', 'raw_down', 'raw_up', 'balance', 'activated').from('customers');
  },
  find(ip) {
    return knex.select('raw_up', 'raw_down', 'balance').where('ip', ip).from('customers');
  },
  change_activation(msisdn, isActivated) {
    // TODO:
    // verify msisdn count == 1, then update
    return knex.update({activated: isActivated}).where('msisdn', msisdn).from('customers');
  },
  update_balance(imsi, delta) {
    //TODO:
    // should reject (not resolve) on failure?
    // should ensure database update success before setting return values
    async function executor(resolve, reject) {
      var result = {
        success: false,
        balance: null,
        message: null
      };
      await knex.select('balance').where('imsi', imsi).from('customers').then((data) => {
        if (data.length != 1) {
          result.message = "Imsi matched " + data.length + " entries."
          resolve(result);
          return;
        }
        result.balance = Number(data[0]["balance"]);
      });
      var nbalance = result.balance + Number(delta);
      if (nbalance >= 0.0) {
        result.balance = nbalance;
        result.success = true;
        await knex.update({ balance: result.balance }).where('imsi', imsi).from('customers');
      }
      resolve(result);
    }
    return new Promise(executor);
  },
  transfer_balance(sender_ip, receiver_msisdn, amount) {
    // TODO:
    // REFUND SENDER IF RECIEVER BALANCE FAILS TO UPDATE
    //  * or better yet, figure out how to make atomic transactions
    // maybe all these failure cases are overkill?
    // real failure behavior (e.g. negative transfer)
    // log with date & time
    // log to actual file
    // shouldn't chained promises return promises for promises? ask someone why this works
    amount = Number(amount);
    function fetch_imsis() {
      return knex.select('imsi').where('ip', sender_ip).from('customers').then((sender_match) => {
        return knex.select('imsi').where('msisdn', receiver_msisdn).from('customers')
          .then((receiver_match) => { return [sender_match, receiver_match]; });
      });
    }
  
    function transfer(imsi_matches) {
      var logstr = "Transfer ";
      if (imsi_matches[0].length != 1) {
        logstr += "failure: ip " + sender_ip + " matched " + imsi_matches[0].length + " database entries.";
        console.log(logstr);
        return;
      }
      var sender_imsi = imsi_matches[0][0]["imsi"];
      if (imsi_matches[1].length != 1) {
        logstr += "failure: msisdn " + receiver_msisdn + " matched " + imsi_matches[1].length + " database entries.";
        console.log(logstr);
        return;
      }
      var receiver_imsi = imsi_matches[1][0]["imsi"];
      if (amount < 0) {
        logstr += "failure: imsi " + sender_imsi + " tried to steal " + -amount + " from imsi "
          + receiver_imsi + ", that scoundrel.";
        console.log(logstr);
        return;
      }
      customer.update_balance(sender_imsi, -amount).then((result0) => {
        if (!result0.success) {
          logstr += "failure: update sender balance failed with result: " + result0;
          console.log(logstr);
          return;
        }
        console.log("result0: " + result0);
        customer.update_balance(receiver_imsi, amount).then((result1) => {
          if (!result1.success) {
            logstr += "failure: update receiver balance failed with result: " + result1;
            console.log(logstr);
            return;
          }
          console.log("result1: " + result1);
          logstr += "success: imsi " + sender_imsi + " sent imsi " + receiver_imsi + " " + amount + ".";
          console.log(logstr);
        });
      });
    }
  
    fetch_imsis().then(transfer);
  }
}

module.exports = customer;
