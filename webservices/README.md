# Webservices
This folder is for essential webservices. Same basic architecture as emergency_webservices (i.e. microservice docker containers) but different in that it will contain things pertaining to network billing, management, topping up, etc. If you want to deploy a commercial/production network, use this. Otherwise (home router? local testing?) you can leave it out.

## Running Locally
- Clone this repo
- `npm install` to install the dependencies
- Make sure you have a local copy of the database
- Create .env file which stores the environment variables. Please include the following:
```
DB_HOST=
DB_USER=
DB_PASSWORD=
DB_NAME=
```
- `npm start` to run the app

The app should be running on [localhost:3000](http://localhost:3000/)
