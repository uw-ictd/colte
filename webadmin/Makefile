
USER_EMAIL=$(shell git config --get user.email)

WEBADMIN_VERSION=0.9.0
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

webadmin:
	npm install
	cp production.env .env
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
		--depends 'nodejs, colte-conf' \
		--after-install ./package/postinst \
		--after-remove ./package/postrm \
		./=/usr/bin/colte-webadmin \
		./package/colte-webadmin.service=/etc/systemd/system/colte-webadmin.service \
		./package/webadmin.env=/usr/local/etc/colte/webadmin.env 
