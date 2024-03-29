var express = require("express");
var router = express.Router();
var colte_models = require("colte-common-models");
var customer = colte_models.buildCustomer();
var app = require("../app");

router.get("/", function (req, res, next) {
  res.redirect("/users/1");
});

router.get("/:page", function (req, res, next) {
  var page = req.params.page;

  customer.all(page).then((data) => {
    const last_page = data.pagination.lastPage;

    customer.access_policies().then((policies) => {
      var has_previous = 0;
      var has_next = 0;

      augmentDataWithPolicyName(data, policies);

      // ALL OF THE FOLLOWING LOGIC IS TO CREATE A SANE "PAGE LIST" IN ALL CORNER-CASES
      var page_list = [];
      if (last_page == 1) {
        page_list = [];
        has_previous = 0;
        has_next = 0;
      } else if (last_page == 2) {
        page_list = [1, 2];
        has_previous = 0;
        has_next = 0;
      } else if (last_page == 3) {
        page_list = [1, 2, 3];
        has_previous = 0;
        has_next = 0;
      } else if (last_page == 4) {
        if (page < 3) {
          page_list = [1, 2, 3];
          has_previous = 0;
          has_next = 1;
        } else {
          page_list = [2, 3, 4];
          has_previous = 1;
          has_next = 0;
        }
      } else {
        // page_list is at least 5, so this is the generic up-to-N case. check two edges and then do generic case
        if (page < 3) {
          page_list = [1, 2, 3];
          has_previous = 0;
          has_next = 1;
        } else if (page > last_page - 2) {
          page_list = [last_page - 2, last_page - 1, last_page];
          has_previous = 1;
          has_next = 0;
        } else {
          page_list = [page - 1, page, +page + 1];
          has_previous = 1;
          has_next = 1;
        }
      }

      res.render("users", {
        translate: app.translate,
        title: app.translate("Home"),
        customers_list: data.data,
        policies_list: policies,
        layout: "layout",

        current_page: page,
        last_page: last_page,
        has_next: has_next,
        has_previous: has_previous,
        one_next: 5,
        one_prev: 3,
        page_list: page_list,
      });
    });
  });
});

router.post("/update/:user_id", function (req, res) {
  var balance = req.body.balance;
  var data_balance = req.body.data_balance;
  var username = req.body.username;
  var zero_balance_policy_id = req.body.zero_balance_policy;
  var positive_balance_policy_id = req.body.positive_balance_policy;

  var enabled = 0;
  if (req.body.enabled == "on") {
    enabled = 1;
  }

  var imsi = req.params.user_id;

  console.log(
    "UPDATE: imsi = " +
      imsi +
      " username=" +
      username +
      " balance=" +
      balance +
      " data_balance=" +
      data_balance +
      " zero_balance_policy_id=" +
      zero_balance_policy_id +
      " positive_balance_policy_id=" +
      positive_balance_policy_id +
      " enabled=" +
      enabled
  );

  customer
    .update(
      imsi,
      enabled,
      balance,
      data_balance,
      zero_balance_policy_id,
      positive_balance_policy_id,
      username
    )
    .then((data) => {
      res.redirect("/users");
    });
});

router.post("/details", function (req, res, next) {
  var imsi = req.body.imsi;
  res.redirect("/users/details/" + imsi);
});

router.get("/details/:user_id", function (req, res, next) {
  var imsi = req.params.user_id;

  customer.find(imsi).then((data) => {
    res.render("users", {
      admin: 1,
      translate: app.translate,
      title: app.translate("Home"),
      customers_list: data,
      layout: "layout",
    });
  });
});

function augmentDataWithPolicyName(data, policies) {
  let policy_map = new Map();
  for (let policy of policies) {
    policy_map.set(policy.id, policy.name);
  }
  data.data.forEach((element) => {
    const found_name = policy_map.get(element.current_policy_id);
    if (found_name) {
      element.current_policy_name = found_name;
    } else {
      element.current_policy_name = "Unknown Policy Name";
    }
  });
  console.log(data.data);
}

module.exports = router;
