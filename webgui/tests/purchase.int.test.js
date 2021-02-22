const test_request = require('supertest');
const app = require('../app');
const Knex = require('colte-common-models').TestKnex;
const purchase_router = require('../routes/purchase');

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
})

describe ("purchase API", function() {
  it('Get main page', async (done) => {
    const res = await test_request(app)
      .get("/purchase")
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(200);
    done();
  })
  it('Post main page', async (done) => {
    const res = await test_request(app)
      .post("/purchase")
      .set('X-Forwarded-For', '192.168.151.2')
      .send("fish");
    expect(res.statusCode).toEqual(405);
    done();
  })
  it('Post main page', async (done) => {
    const res = await test_request(app)
      .get("/purchase/purchase")
      .set('X-Forwarded-For', '192.168.151.2');
    expect(res.statusCode).toEqual(405);
    done();
  })

})
