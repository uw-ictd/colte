const test_request = require('supertest');
const app = require('../app');
const Knex = require('colte-common-models').TestKnex;
const purchase_router = require('../routes/transfer');

const databaseName = `colte_test_for_worker_${process.env.JEST_WORKER_ID}`;

beforeAll(async () => {
  Knex.raw(`DROP DATABASE IF EXISTS ${databaseName}`);
  Knex.raw(`CREATE DATABASE ${databaseName}`);
  // TODO(matt9j) Add support for creating a clean test DB, possibly from knex
  // migrations and seeds.
});

afterAll(async () =>{
  Knex.raw(`DROP DATABASE ${databaseName}`);
  Knex.destroy();
});

describe ("transfer API", function() {
  it('Get main page valid address', async (done) => {
    const res = await test_request(app)
      .get("/transfer")
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(200);
    done();
  });
  it('Get main page invalid address', async (done) => {
    const res = await test_request(app)
      .get("/transfer")
      .set('X-Forwarded-For', '255.255.255.255');
    expect(res.statusCode).toEqual(403);
    done();
  });
  it('Post main page (invalid verb)', async (done) => {
    const res = await test_request(app)
      .post("/transfer")
      .set('X-Forwarded-For', '192.168.151.2')
      .send("fish");
    expect(res.statusCode).toEqual(405);
    done();
  });
  it('Get transfer api (invalid verb)', async (done) => {
    const res = await test_request(app)
      .get("/transfer/transfer")
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(405);
    done();
  });
  it('Post transfer api, invalid source address', async (done) => {
    const res = await test_request(app)
      .post("/transfer/transfer")
      .set('X-Forwarded-For', '0.0.0.0');
    expect(res.statusCode).toEqual(403);
    done();
  });
  it('Post transfer api, invalid dest address', async (done) => {
    const res = await test_request(app)
      .post("/transfer/transfer")
      .send("1337") //TODO
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(400);
    done();
  });
  it('Post transfer api, insufficient source funds', async (done) => {
    const res = await test_request(app)
      .post("/transfer/transfer")
      .send("1337") //TODO
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(400);
    done();
  });
  it('Post transfer api, self-transfer', async (done) => {
    const res = await test_request(app)
      .post("/transfer/transfer")
      .send("1337") //TODO
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(400);
    done();
  });
  it('Post transfer api, valid transfer', async (done) => {
    const res = await test_request(app)
      .post("/transfer/transfer")
      .send({"package":"10000000"}) //TODO
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(302);
    done();
  });
  it('Post transfer api, transfer stress consistency test', async (done) => {
    // TODO
    expect(true).toEqual(true);
    done();
  });
});
