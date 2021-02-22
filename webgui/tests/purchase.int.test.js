const test_request = require('supertest');
const app = require('../app');
const Knex = require('colte-common-models').TestKnex;

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

describe ("purchase API", function() {
  it('Get main page valid address', async (done) => {
    const res = await test_request(app)
      .get("/purchase")
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(200);
    done();
  });
  it('Get main page invalid address', async (done) => {
    const res = await test_request(app)
      .get("/purchase")
      .set('X-Forwarded-For', '255.255.255.255');
    expect(res.statusCode).toEqual(403);
    done();
  });
  it('Post main page (invalid verb)', async (done) => {
    const res = await test_request(app)
      .post("/purchase")
      .set('X-Forwarded-For', '192.168.151.2')
      .send("fish");
    expect(res.statusCode).toEqual(405);
    done();
  });
  it('Get purchase api (invalid verb)', async (done) => {
    const res = await test_request(app)
      .get("/purchase/purchase")
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(405);
    done();
  });
  it('Post purchase api, invalid address', async (done) => {
    const res = await test_request(app)
      .post("/purchase/purchase")
      .send({"package":"10000000"})
      .set('X-Forwarded-For', '0.0.0.0');
    expect(res.statusCode).toEqual(403);
    done();
  });
  it('Post purchase api, invalid amount', async (done) => {
    const res = await test_request(app)
      .post("/purchase/purchase")
      .send("1337")
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(400);
    done();
  });
  it('Post purchase api', async (done) => {
    const res = await test_request(app)
      .post("/purchase/purchase")
      .send({"package":"10000000"})
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(302);
    done();
  });

});
