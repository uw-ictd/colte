
COLTE_FULL_VERSION=0.10.1
COLTE_VERSION=0.9.12
CONF_VERSION=0.9.13
WEBGUI_VERSION=0.9.11
WEBADMIN_VERSION=0.9.1

TARGET_DIR=./BUILD/

.PHONY: webadmin webgui all

all: full colte conf webgui webadmin

build_deps:
	sudo apt-get install ruby ruby-dev rubygems build-essential
	sudo gem install --no-ri --no-rdoc fpm

web_deps_ubuntu:
	sudo apt-get install npm nodejs

target:
	mkdir -p $(TARGET_DIR)

new_colte: target
	cd webgui; npm install
	cd webadmin; npm install
	cd webgui; cp production.env .env
	cd webadmin; cp production.env .env
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /etc/colte/config.yml \
		--config-files /usr/bin/colte-webgui/.env \
		--config-files /usr/bin/colte-webadmin/.env \
		--after-install ./package/postinst \
		--after-remove ./package/postrm \
		--maintainer sevilla@cs.washington.edu \
		--description "The Community LTE Project" \
		--url "https://github.com/uw-ictd/colte" \
		--name colte \
		--version $(COLTE_NEW_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'open5gs, haulage, python, nodejs (>= 8.0.0), colte-db (>= 0.9.11)' \
		./package/haulage.yml=/etc/colte/haulage.yml \
		./conf/colteconf.py=/usr/bin/ \
		./conf/config.yml=/etc/colte/config.yml \
		./webgui/=/usr/bin/colte-webgui \
		./package/colte-webgui.service=/etc/systemd/system/colte-webgui.service \
		./package/colte-webadmin.service=/etc/systemd/system/colte-webadmin.service \
		./package/webgui.env=/usr/local/etc/colte/webgui.env \
		./package/pricing.json=/usr/local/etc/colte/pricing.json \
		./package/transaction_log.txt=/var/log/colte/transaction_log.txt \
		./webadmin/=/usr/bin/colte-webadmin \
		./package/webadmin.env=/usr/local/etc/colte/webadmin.env
