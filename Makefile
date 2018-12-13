
COLTE_VERSION=0.9.10
CONF_VERSION=0.9.10
WEBSERVICES_VERSION=0.9.10
TARGET_DIR=./BUILD/

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
		--depends 'colte-webgui'

webgui:
	make -C ./webgui webgui
