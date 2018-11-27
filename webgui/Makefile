
USER_EMAIL=$(shell git config --get user.email)

WEBGUI_VERSION=0.9.8
TARGET_DIR=./

fpm:
	sudo apt-get install ruby ruby-dev rubygems build-essential
	sudo gem install --no-ri --no-rdoc fpm

# this is UBUNTU
npm_ubuntu:
	# step 1: install node/npm
	sudo apt-get install npm nodejs

# this is DEBIAN
npm_debian:
	curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -
	sudo apt-get install nodejs

webgui:
	npm install
	cp production.env .env
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
		--depends 'nodejs, colte-db (>= 0.9.2), colte-conf' \
		--after-install ./package/postinst \
		--after-remove ./package/postrm \
		./=/usr/bin/colte-webgui \
		./package/colte-webgui.service=/etc/systemd/system/colte-webgui.service \
		./package/webgui.env=/usr/local/etc/colte/webgui.env 
