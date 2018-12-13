# Webservices
This folder is for essential webservices. Same basic architecture as emergency (i.e. microservice docker containers) but different in that it will contain things pertaining to network billing, management, topping up, etc. If you want to deploy a commercial/production network, use this. Otherwise (home router? local testing?) you can leave it out.

## Running Locally

### Initial Setup
To play around with this code on your local machine, you must first install MySQL. This process will vary wildly depending on your exact platform/context: in Ubuntu/Debian you can just `apt-get install mysql-server mysql-client`, in OSX the process is more complicated now. Once installed, issue the following commands as a MySQL user with admin privileges:
```
CREATE DATABASE colte_db;
CREATE USER colte_db@localhost IDENTIFIED BY 'colte_db';
GRANT ALL PRIVILEGES ON colte_db.* TO colte_db@localhost;
FLUSH PRIVILEGES;
```
Then, in a terminal, run the following command to import the sample database:
```
mysql -u colte_db -pcolte_db colte_db < sample_db.sql
```

You must also install `npm`. Again, this varies wildly depending on your platform; Google is helpful.

### Installation/Configuration and Running
- Use `npm install` to install the dependencies
- Create a .env file for environment variables. If you're just following the standard install scripts, the best way to do this is to just copy "production.env" to ".env" and look it over to make sure the DB_ variables are correct.
- `npm start` will run the app. It should be running on [localhost:7999](http://localhost:7999/) unless you've changed the PORT number in .env.

## Todo
### /admin 
- [x] customer activation/deactivation
- [x] editing customer's balance 
- [ ] password (and IP) verification
- [ ] easier interface (as opposed to table)
### /user 
- [x] credit transfer
- [x] localization
- [ ] pin verification(???)
### /system
- [ ] 