
COLTE_FULL_VERSION=0.10.1
COLTE_VERSION=0.9.12
CONF_VERSION=0.9.13
WEBGUI_VERSION=0.9.11
WEBADMIN_VERSION=0.9.1

YOUTUBE_VERSION=0.9

TARGET_DIR=./BUILD/

.PHONY: webadmin webgui all

all: full colte conf webgui webadmin

build_deps:
	sudo apt-get install ruby ruby-dev rubygems build-essential
	sudo gem install --no-ri --no-rdoc fpm

web_deps_ubuntu:
	sudo apt-get install npm nodejs

web_deps_debian:
	curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -
	sudo apt-get install nodejs

target:
	mkdir -p $(TARGET_DIR)

full: target
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "The Community LTE Project - Full Version (inc. Haulage and Webtools)" \
		--url "https://github.com/uw-ictd/colte" \
		--name colte-full \
		--version $(COLTE_FULL_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'colte-epc, colte-webgui, haulage, colte-conf, colte-webadmin' \
		./package/colte/haulage.yml=/usr/local/etc/colte/haulage.yml

colte: target
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "The Community LTE Project" \
		--url "https://github.com/uw-ictd/colte" \
		--name colte \
		--version $(COLTE_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'colte-epc, colte-webgui, haulage, colte-conf' \
		./package/colte/haulage.yml=/usr/local/etc/colte/haulage.yml

conf: target
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/bin/colte/roles/configure/vars/main.yml \
		--maintainer sevilla@cs.washington.edu \
		--description "Configuration Tools for CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--name colte-conf \
		--version $(CONF_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'ansible, python-mysqldb, colte-db' \
		--after-install ./conf/postinst \
		--after-remove ./conf/postrm \
		./conf/colteconf=/usr/bin/ \
		./conf/coltedb=/usr/bin/ \
		./conf/colte=/usr/bin/ \
		./conf/colte-tcpdump.service=/etc/systemd/system/colte-tcpdump.service \
		./conf/config.yml=/usr/local/etc/colte/config.yml

webgui: target
	cd webgui; npm install
	cd webgui; cp production.env .env
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/bin/colte-webgui/.env \
		--maintainer sevilla@cs.washington.edu \
		--description "WebGUI for CoLTE users to check balance, buy/sell data, etc." \
		--url "https://github.com/uw-ictd/colte" \
		--name colte-webgui \
		--version $(WEBGUI_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'nodejs, colte-db (>= 0.9.2)' \
		--after-install ./package/webgui/postinst \
		--after-remove ./package/webgui/postrm \
		./webgui/=/usr/bin/colte-webgui \
		./package/webgui/colte-webgui.service=/etc/systemd/system/colte-webgui.service \
		./package/webgui/webgui.env=/usr/local/etc/colte/webgui.env \
		./package/webgui/pricing.json=/usr/local/etc/colte/pricing.json \
		./package/webgui/transaction_log.txt=/var/log/colte/transaction_log.txt 

webadmin: target
	cd webadmin; npm install
	cd webadmin; cp production.env .env
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/bin/colte-webadmin/.env \
		--maintainer sevilla@cs.washington.edu \
		--description "Web-based tool for CoLTE network administrators." \
		--url "https://github.com/uw-ictd/colte" \
		--name colte-webadmin \
		--version $(WEBADMIN_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'nodejs (>= 8.0.0), colte-db (>= 0.9.11), colte-conf' \
		--after-install ./package/webadmin/postinst \
		--after-remove ./package/webadmin/postrm \
		./webadmin/=/usr/bin/colte-webadmin \
		./package/webadmin/colte-webadmin.service=/etc/systemd/system/colte-webadmin.service \
		./package/webadmin/webadmin.env=/usr/local/etc/colte/webadmin.env 

### Locally-Hosted Webservices Start Here ###
ourtube: target
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/bin/ourtube_data/conf/ourtube.config \
		--maintainer durandn@cs.washington.edu \
		--description "Locally-Hosted Open Source Video Sharing App" \
		--url "https://github.com/uw-ictd/colte" \
		--name ourtube \
		--version $(YOUTUBE_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'python3, python3-pip' \
		--after-install ./package/ourtube/postinst \
		--after-remove ./package/ourtube/postrm \
		./lte_extras/ourtube/=/usr/bin/ourtube_data \
		./package/ourtube/ourtube=/usr/bin/ourtube \
		./package/ourtube/ourtube.service=/etc/systemd/system/ourtube.service \
		./package/ourtube/ourtube.config=/usr/local/etc/ourtube.config

