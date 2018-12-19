
COLTE_VERSION=0.9.10
CONF_VERSION=0.9.10
WEBSERVICES_VERSION=0.9.10
WEBGUI_VERSION=0.9.9
WEBADMIN_VERSION=0.9.0

TARGET_DIR=./BUILD/

build_deps:
	sudo apt-get install ruby ruby-dev rubygems build-essential
	sudo gem install --no-ri --no-rdoc fpm

web_deps_ubuntu:
	sudo apt-get install npm nodejs

web_deps_debian:
	curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -
	sudo apt-get install nodejs

.PHONY: webadmin webgui

target:
	mkdir -p $(TARGET_DIR)

colte: target
	fpm --input-type empty \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "The Community LTE Project" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte \
		--version $(COLTE_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'colte-epc (>= 0.9.3), colte-webservices, haulage, colte-conf'

conf: target
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/bin/colte/roles/configure/vars/main.yml \
		--maintainer sevilla@cs.washington.edu \
		--description "Configuration Tools for CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
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

webservices: target
	fpm --input-type empty \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "CoLTE Locally-Hosted Webservices" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-webservices \
		--version $(WEBSERVICES_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'colte-webgui, colte-webadmin'

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
		--deb-compression xz \
		--name colte-webgui \
		--version $(WEBGUI_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'nodejs, colte-db (>= 0.9.2)' \
		--after-install ./package/webgui/postinst \
		--after-remove ./package/webgui/postrm \
		./webgui/=/usr/bin/colte-webgui \
		./package/webgui/colte-webgui.service=/etc/systemd/system/colte-webgui.service \
		./package/webgui/webgui.env=/usr/local/etc/colte/webgui.env 


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
		--deb-compression xz \
		--name colte-webadmin \
		--version $(WEBADMIN_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'nodejs, colte-db (>= 0.9.2), colte-conf' \
		--after-install ./package/webadmin/postinst \
		--after-remove ./package/webadmin/postrm \
		./webadmin/=/usr/bin/colte-webadmin \
		./package/webadmin/colte-webadmin.service=/etc/systemd/system/colte-webadmin.service \
		./package/webadmin/webadmin.env=/usr/local/etc/colte/webadmin.env 
