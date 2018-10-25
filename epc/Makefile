
USER_EMAIL=$(shell git config --get user.email)

HSS_VERSION=0.9.6
HSS_LOCATION=/usr/local/bin/oai_hss
HSS_CONF_LOCATION=./package/hss/hss.conf
HSS_FREEDIAMETER_LOCATION=./package/hss/freeDiameter
HSS_SERVICE_LOCATION=./package/hss/colte-hss.service
hss:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files $(HSS_CONF_LOCATION) \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface HSS" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--deb-systemd ./package/hss/colte-hss.service \
		--name colte-hss \
		--version $(HSS_VERSION) \
		--depends 'default-libmysqlclient-dev, libconfig9, libsctp1, colte-freediameter, colte-db' \
		--after-install ./package/hss/postinst \
		--after-remove ./package/hss/postrm \
		$(HSS_LOCATION)=/usr/bin/ \
		$(HSS_CONF_LOCATION)=/usr/local/etc/oai/hss.conf \
		$(HSS_FREEDIAMETER_LOCATION)=/usr/local/etc/oai/freeDiameter

MME_VERSION=0.9.3
MME_LOCATION=/usr/local/bin/mme
MME_CONF_LOCATION=./package/mme/mme.conf
MME_FREEDIAMETER_LOCATION=./package/mme/freeDiameter
MME_SERVICE_LOCATION=./package/mme/colte-mme.service
mme:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files $(MME_CONF_LOCATION) \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface MME" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--deb-systemd ./package/mme/colte-mme.service \
		--name colte-mme \
		--version $(MME_VERSION) \
		--depends 'libsctp1, libconfig9, colte-freediameter, colte-liblfds' \
		--after-install ./package/mme/postinst \
		--after-remove ./package/mme/postrm \
		$(MME_LOCATION)=/usr/bin/ \
		$(MME_CONF_LOCATION)=/usr/local/etc/oai/mme.conf \
		$(MME_FREEDIAMETER_LOCATION)=/usr/local/etc/oai/freeDiameter

SPGW_VERSION=0.9.7
SPGW_LOCATION=/usr/local/bin/spgw
SPGW_CONF_LOCATION=./package/spgw/spgw.conf
SPGW_SERVICE_LOCATION=./package/spgw/colte-spgw.service
SPGW_NAT_LOCATION=./package/spgw/spgw_nat.sh
spgw:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--config-files $(SPGW_CONF_LOCATION) \
		--config-files /usr/bin/spgw_nat.sh \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface SPGW" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--deb-systemd ./package/spgw/colte-spgw.service \
		--deb-systemd ./package/spgw/colte-spgw_nat.service \
		--name colte-spgw \
		--version $(SPGW_VERSION) \
		--depends 'default-libmysqlclient-dev, libconfig9, colte-liblfds, colte-libgtpnl, colte-db' \
		--after-install ./package/spgw/postinst \
		--after-remove ./package/spgw/postrm \
		$(SPGW_LOCATION)=/usr/bin/ \
		$(SPGW_CONF_LOCATION)=/usr/local/etc/oai/spgw.conf \
		$(SPGW_NAT_LOCATION)=/usr/bin/

EPC_VERSION=0.9.5
epc:
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer sevilla@cs.washington.edu \
		--description "The OpenAirInterface EPC" \
		--url "https://github.com/uw-ictd/colte" \
		--deb-compression xz \
		--deb-systemd ./package/epc/colte-epc.service \
		--name colte-epc \
		--version $(EPC_VERSION) \
		--depends 'colte-hss, colte-mme, colte-spgw, colte-conf' \
		--after-install ./package/epc/postinst \
		--after-remove ./package/epc/postrm \
# ADD A LINK: ln -s /usr/local/etc/oai /usr/local/etc/colte/oai

COLTE_VERSION=0.9.5
colte:
	fpm --input-type dir \
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

DB_VERSION=0.9.5
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

CONF_VERSION=0.9.5
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
		--deb-systemd ./package/conf/colte-tcpdump.service \
		--name colte-conf \
		--version $(CONF_VERSION) \
		--depends 'ansible, python-mysqldb, colte-db' \
		./package/conf/colteconf=/usr/bin/ \
		./package/conf/coltedb=/usr/bin/ \
		./package/conf/colte=/usr/bin/
# ADD A LINK: ln -s /usr/bin/colte/roles/configure/vars/main.yml /usr/local/etc/colte/config.yml

OAI_DEPS_VERSION=0.9.0
oai-deps:
	fpm --input-type dir \
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
