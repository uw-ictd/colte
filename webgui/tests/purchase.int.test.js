const test_request = require('supertest');
const app = require('../app');
const purchase_router = require('../routes/purchase');

beforeAll(async () => {
  ;
});

afterAll(async () =>{
  ;
})

describe ("purchase API", function() {
  it('Get main page, no db', async (done) => {
    const res = await test_request(app)
      .get("/purchase")
      .set('X-Forwarded-For', '192.168.2.1');
    expect(res.statusCode).toEqual(503);
    done();
  })
  it('Post main page', async (done) => {
    const res = await test_request(app)
      .post("/purchase")
      .set('X-Forwarded-For', '192.168.2.1')
      .send("fish");
    expect(res.statusCode).toEqual(405);
    done();
  })
  it('Post main page', async (done) => {
    const res = await test_request(app)
      .get("/purchase/purchase")
      .set('X-Forwarded-For', '192.168.2.1');
    expect(res.statusCode).toEqual(405);
    done();
  })

})
