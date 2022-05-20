# Webgui

This folder is for the basic customer-facing gui for interacting with the
network. The webgui provides an interface for each subscriber to manage their
own billing, with automatic subscriber identification based on their static IP
address enforced by the cellular network.

## Installation

### Package

The customer-facing webgui and all needed dependencies are installed and
configured by default as part of the colte-prepaid package. See the [overall
CoLTE project readme](../README.md) for details on how to install via package
manager.

### Manual Installation & Setup

To play around with this code on your local machine, with a manual installation,
you must follow several steps to get the required dependencies.

1.First install and configure postgres. This process will vary wildly depending
on your exact platform/context: in Ubuntu/Debian you can just `apt-get install postgresql`, in OSX the process is more complicated for some reason. Once
installed, issue the following commands as a postgres user with admin
privileges:

```sql
CREATE DATABASE haulage_db;
CREATE ROLE haulage_db WITH LOGIN ENCRYPTED PASSWORD 'haulage_db';
GRANT ALL PRIVILEGES ON DATABASE haulage_db TO haulage_db;
```

2. Some webadmin features require the installation of
   [haulage](https://github.com/uw-ictd/haulage/). See the haulage documentation
   for details on how to install it. You can operate without haulage, but will
   need to manually create fake haulage database tables if so, or see some
   commands fail to execute.

3. You must also install `npm` and `node`. Again, this varies wildly depending
   on your platform. You can find more information for your distribution via
   [nodesource here](https://github.com/nodesource/distributions). We recommend
   using the latest stable nodejs and npm versions from nodesource instead of the
   version shipped in your particular distribution's repository.

4. Use npm to install all required javascript dependencies.

```shell
cd webadmin
npm ci
```

## Configuration and Running

- Create a .env file for environment variables. If you're just following the
  standard install scripts, the best way to do this is to just copy
  "production.env" to ".env" and look it over to make sure the DB\_ variables
  are correct.

- The WebAdmin username will always be "admin" and you can change the password
  in the .env file.

- `npm start` will run the app. It should be running on
  [localhost:7998](http://localhost:7998/) unless you've changed the PORT number
  in .env.

## Development

If you want to run the webadmin tool without haulage, you can seed the database
with fake data and run the emulate_haulage migrations. See
[colte-common-models/knexfile.js](../colte-common-models/knexfile.js). Running
the seed files and migrations looks like this:

```shell
cd colte-common-models
DB_USER=haulage_db DB_PASSWORD=haulage_db DB_NAME=haulage_db DB_HOST=localhost npx -- knex migrate:latest
DB_USER=haulage_db DB_PASSWORD=haulage_db DB_NAME=haulage_db DB_HOST=localhost npx -- knex seed:run
```

## Configuration and Running

- Create a .env file for environment variables. If you're just following the
  standard install scripts, the best way to do this is to just copy
  "production.env" to ".env" and look it over to make sure the DB\_ variables
  are correct.

- `npm start` will run the app. It should be running on
  [localhost:7999](http://localhost:7999/) unless you've changed the PORT number
  in .env.
