
USER_EMAIL=$(shell git config --get user.email)

HSS_VERSION=0.9.6
MME_VERSION=0.9.3
SPGW_VERSION=0.9.7
EPC_VERSION=0.9.5
COLTE_VERSION=0.9.5
DB_VERSION=0.9.5
CONF_VERSION=0.9.5
OAI_DEPS_VERSION=0.9.0

hss:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/local/etc/oai/hss.conf \
		--config-files /usr/local/etc/oai/freeDiameter/acl.conf \
		--config-files /usr/local/etc/oai/freeDiameter/hss_fd.conf \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface HSS" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-hss \
		--version $(HSS_VERSION) \
		--depends 'default-libmysqlclient-dev, libconfig9, libsctp1, colte-freediameter, colte-db' \
		--after-install ./package/hss/postinst \
		--after-remove ./package/hss/postrm \
		/usr/local/bin/oai_hss=/usr/bin/ \
		./package/hss/hss.conf=/usr/local/etc/oai/hss.conf \
		./package/hss/colte-hss.service=/etc/systemd/system/colte-hss.service \
		./package/hss/freeDiameter=/usr/local/etc/oai/

mme:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/local/etc/oai/mme.conf \
		--config-files /usr/local/etc/oai/freeDiameter/mme_fd.conf \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface MME" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-mme \
		--version $(MME_VERSION) \
		--depends 'libsctp1, libconfig9, colte-freediameter, colte-liblfds' \
		--after-install ./package/mme/postinst \
		--after-remove ./package/mme/postrm \
		/usr/local/bin/mme=/usr/bin/ \
		./package/mme/mme.conf=/usr/local/etc/oai/mme.conf \
		./package/mme/colte-mme.service=/etc/systemd/system/colte-mme.service \
		./package/mme/freeDiameter=/usr/local/etc/oai/

spgw:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files /usr/local/etc/oai/spgw.conf \
		--config-files /usr/bin/spgw_nat.sh \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface SPGW" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-spgw \
		--version $(SPGW_VERSION) \
		--depends 'default-libmysqlclient-dev, libconfig9, colte-liblfds, colte-libgtpnl, colte-db' \
		--after-install ./package/spgw/postinst \
		--after-remove ./package/spgw/postrm \
		/usr/local/bin/spgw=/usr/bin/ \
		./package/spgw/spgw_nat.sh=/usr/bin/ \
		./package/spgw/spgw.conf=/usr/local/etc/oai/spgw.conf \
		./package/spgw/colte-spgw.service=/etc/systemd/system/colte-spgw.service \
		./package/spgw/colte-spgw_nat.service=/etc/systemd/system/colte-spgw_nat.service

epc:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface EPC" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-epc \
		--version $(EPC_VERSION) \
		--depends 'colte-hss, colte-mme, colte-spgw, colte-conf' \
		--after-install ./package/epc/postinst \
		--after-remove ./package/epc/postrm \
		./package/epc/colte-epc.service=/etc/systemd/system/colte-epc.service \
		./package/epc/oai=/usr/local/etc/colte/oai

colte:
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
		--depends 'colte-epc (>= 0.9.3), colte-webservices, haulage'

db:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "Sample database for use with CoLTE" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name colte-db \
		--version $(DB_VERSION) \
		--depends 'mysql-server, mysql-client' \
		--after-install ./package/db/postinst \
		--after-remove ./package/db/postrm \
		./package/db/sample_db.sql=/usr/local/etc/colte

conf:
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
		--depends 'ansible, python-mysqldb, colte-db' \
		--after-install ./package/conf/postinst \
		--after-remove ./package/conf/postrm \
		./package/conf/colteconf=/usr/bin/ \
		./package/conf/coltedb=/usr/bin/ \
		./package/conf/colte=/usr/bin/ \
		./package/conf/colte-tcpdump.service=/etc/systemd/system/colte-tcpdump.service \
		./package/conf/config.yml=/usr/local/etc/colte/config.yml

oai-deps:
	fpm --input-type empty \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "All dependencies needed to build the OpenAirInterface EPC" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--name oai-deps \
		--version $(OAI_DEPS_VERSION) \
		--depends 'autoconf, automake, bison, build-essential, cmake, cmake-curses-gui, doxygen, doxygen-gui, flex, pkg-config, git, libconfig-dev, libgcrypt11-dev, libidn2-0-dev, libidn11-dev, default-libmysqlclient-dev, libpthread-stubs0-dev, libsctp1, libsctp-dev, libssl-dev, libtool, openssl, nettle-dev, nettle-bin, php, python-pexpect, castxml, guile-2.0-dev, libgmp-dev, libhogweed4, libgtk-3-dev, libxml2, libxml2-dev, mscgen, check, python, libgnutls28-dev, python-dev, unzip, libmnl-dev, colte-freediameter, colte-liblfds, colte-libgtpnl, colte-asn1c'

all: hss mme spgw epc colte db conf oai-deps

package-clean:
	rm colte*\.deb

build-clean:
	echo "build-clean does nothing right now..."
	# ./scripts/build_hss -c
	# ./scripts/build_mme -c
	# ./scripts/build_spgw -c

clean: package-clean build-clean
