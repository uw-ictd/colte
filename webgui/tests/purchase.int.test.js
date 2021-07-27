require("dotenv").config();
const test_request = require("supertest");
const common_models = require("colte-common-models");

// The app will be instantiated after database prep.
let app = null;
let db_specific_knex = null;
let admin_knex = null;

const databaseName = `colte_purchase_test_for_worker_${process.env.JEST_WORKER_ID}`;

beforeAll(async () => {
  // Setup knex to run seeds and migrations.
  let config = common_models.knexFile[process.env.NODE_ENV];

  // Normalize directories to be arrays
  if (!Array.isArray(config.migrations.directory)) {
    config.migrations.directory = [config.migrations.directory];
  }
  if (!Array.isArray(config.seeds.directory)) {
    config.seeds.directory = [config.seeds.directory];
  }

  // Set the path for all entries
  config.migrations.directory = config.migrations.directory.map(
    (path_i) => "../colte-common-models/" + path_i
  );
  config.seeds.directory = config.seeds.directory.map(
    (path_i) => "../colte-common-models/" + path_i
  );

  admin_knex = common_models.getKnexInstance(config);

  await admin_knex
    .raw(`DROP DATABASE IF EXISTS ${databaseName};`)
    .then(() => {
      return admin_knex.raw(`CREATE DATABASE ${databaseName};`);
    })
    .then(() => {
      process.env.DB_NAME = databaseName;
      app = require("../app");
    })
    .then(() => {
      // Update the config to point to the newly created database
      config.connection.database = databaseName;
      db_specific_knex = require("colte-common-models").getKnexInstance(config);
      return db_specific_knex.migrate.latest().then(() => {
        return db_specific_knex.seed.run();
      });
    });
});

afterAll(async () => {
  await db_specific_knex.destroy();
  await admin_knex.raw(`DROP DATABASE IF EXISTS ${databaseName} WITH (FORCE)`);
});

describe("purchase API", function () {
  it("Get main page valid address", async () => {
    const res = await test_request(app).get("/purchase").set("X-Forwarded-For", "192.168.151.2");
    expect(res.statusCode).toEqual(200);
    expect(res.text).toEqual(expect.stringContaining("Current Balance: $2500"));
  });
  it("Get main page invalid address", async () => {
    const res = await test_request(app).get("/purchase").set("X-Forwarded-For", "255.255.255.255");
    expect(res.statusCode).toEqual(403);
  });
  it("Post main page (invalid verb)", async () => {
    const res = await test_request(app)
      .post("/purchase")
      .set("X-Forwarded-For", "192.168.151.2")
      .send("fish");
    expect(res.statusCode).toEqual(405);
  });
  it("Get purchase api (invalid verb)", async () => {
    const res = await test_request(app)
      .get("/purchase/purchase")
      .set("X-Forwarded-For", "192.168.151.2");
    expect(res.statusCode).toEqual(405);
  });
  it("Post purchase api, invalid address", async () => {
    const res = await test_request(app)
      .post("/purchase/purchase")
      .send({package: "10000000"})
      .set("X-Forwarded-For", "0.0.0.0");
    expect(res.statusCode).toEqual(403);
  });
  it("Post purchase api, invalid amount", async () => {
    const res = await test_request(app)
      .post("/purchase/purchase")
      .send("1337")
      .set("X-Forwarded-For", "192.168.151.2");
    expect(res.statusCode).toEqual(400);
  });
  it("Post purchase api", async () => {
    const res = await test_request(app)
      .post("/purchase/purchase")
      .send({package: "10000000"})
      .set("X-Forwarded-For", "192.168.151.2");
    expect(res.statusCode).toEqual(302);
    expect(res.text).toEqual("Found. Redirecting to /purchase");

    // Check that the balance was correctly deducted and converted to data.
    const status = await test_request(app).get("/purchase").set("X-Forwarded-For", "192.168.151.2");
    expect(status.statusCode).toEqual(200);
    expect(status.text).toEqual(expect.stringContaining("Current Balance: $0"));
    expect(status.text).toEqual(expect.stringContaining("Data Balance: 110.0 MB"));
  });
  it("Post purchase api, stress consistency test", async () => {
    // Generate many concurrent requests, and ensure the results match expected.
    let promises = [];
    for (i = 0; i < 100; ++i) {
      promises.push(
        test_request(app)
          .post("/purchase/purchase")
          .send({package: "10000000"})
          .set("X-Forwarded-For", "192.168.151.5")
      );
    }
    results = await Promise.all(promises);

    // Validate request results
    for (i = 0; i < 100; ++i) {
      expect(results[i].statusCode).toEqual(302);
    }

    // Validate Balance
    const status = await test_request(app).get("/purchase").set("X-Forwarded-For", "192.168.151.5");
    expect(status.statusCode).toEqual(200);
    expect(status.text).toEqual(expect.stringContaining("Current Balance: $24750000"));
    expect(status.text).toEqual(expect.stringContaining("Data Balance: 1.1 GB"));
  });
});
