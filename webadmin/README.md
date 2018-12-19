# WebAdmin
The WebAdmin is a tool for network administrators to manage customer accounts, enable/disable specific SIM cards, cofigure CoLTE, etc. It is intended to be your one-stop web interface for managing the network. We are currently building out this tool and adding various features, so please submit any and all requests.

***WARNING: The WebAdmin tool is secured via a basic username/password combo, and can completely manage a CoLTE network. It should be considered a security risk if your network supports untrusted users (i.e. customers) You are responsible for restricting access to WebAdmin based on IP addresses, or taking other steps to secure it, if you are using the WebAdmin tool in a production context.***

### Initial Setup
To play around with this code on your local machine, you must first install MySQL. This process will vary wildly depending on your exact platform/context: in Ubuntu/Debian you can just `apt-get install mysql-server mysql-client`, in OSX the process is more complicated for some reason. Once installed, issue the following commands as a MySQL user with admin privileges:
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
- The WebAdmin username will always be "admin" and you can change the password in the .env file.
- `npm start` will run the app. It should be running on [localhost:7998](http://localhost:7998/) unless you've changed the PORT number in .env.

### Debian Stretch And NodeJS
Currently, the stable branch of Debian 9 (Stretch) supports `nodejs-v4.8.x`, but the WebAdmin tool requires `nodejs-8.x.x`. This version is currently available as a stretch backport, and can be downloaded/installed with the following commands:
```
echo "deb http://ftp.debian.org/debian stretch-backports main" | sudo tee -a /etc/apt/sources.list.d/backport.list
sudo apt-get update
sudo apt-get -t stretch-backports install nodejs
```
