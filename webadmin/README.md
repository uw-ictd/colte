# WebAdmin
The WebAdmin is a tool for network administrators to manage customer accounts, enable/disable specific SIM cards, cofigure CoLTE, etc. It is intended to be your one-stop web interface for managing the network. We are currently building out this tool and adding various features, so please submit any and all requests.

***WARNING: The WebAdmin tool is currently not secured in any way/shape/form, and can completely manage a CoLTE network. It should be considered a security risk if you are interacting with untrusted users (i.e. customers) You are responsible for enabling or disabling this tool, or securing it appropriately, if you are using it in a production context.***

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
- `npm start` will run the app. It should be running on [localhost:7998](http://localhost:7998/) unless you've changed the PORT number in .env.
