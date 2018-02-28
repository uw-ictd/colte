# emergency_webservices
Our Mozilla Challenge proposal was to create a standalone LTE Network-in-a-Box to be deployed in the wake of a natural disaster. This box will provide all sorts of emergency services to their users. Compared to normal LTE networks, there are two main features we need to investigate/create/connect: emergency mode connections and emergency webservices.

<!-- ## Emergency Mode Connections: -->
<!-- Per the LTE spec, normal SIM cards (i.e. issued by a major telecom carrier) will perform authentication upon network join, and will explicitly not connect to non-authenticated towers *EXCEPT* in emergency mode. Emergency mode joins are specifically non-authenticated and *only* initiated if the phone cannot contact any authenticated towers. -->

## Main Architecture:
In the interests of simplicity and ease of development/deployment, we're using a microservice architecture wherein every independent webapp is provisioned in its own independent Docker container. Each container is forwarded a separate host port (e.g. chat runs on localhost:8081->container:80, wiki runs on localhost:8082->container:80).

The main landing page (TODO.local) is static HTML consisting of simple button-links to independent subdomains (e.g. chat.TODO.local) and then each site creates/installs its own apache .conf file in /etc/apache2/sites-available that uses an Apache VirtualHost entry to redirect/port-forward the correct subdomain (e.g. chat.TODO.local -> localhost:8081).

## Creating A Webapp:
Creating/installing a new webapp are pretty straightforward under this system. First, write your webapp in whatever framework/system you choose, and once complete, package it up as a Docker image. Once created, all that's left to do is (1) write a corresponding Apache .conf file for the site, (2) write the correct Ansible .yml file to fetch/install/start the service, and (3) add a pointer in /system_setup/$OS/ansible/emergency_webservices.yml. For great and simple examples of how to do this, check out the rocketchat.* files.