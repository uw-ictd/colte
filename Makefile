GIT_VERSION=$(shell git describe --tags | sed s/-g/+g/g)
TARGET_DIR=./BUILD
NFPM_VERSION = 2.2.3
HOST_ARCH=$(shell uname -m)
ifeq ($(HOST_ARCH),aarch64)
	NPM_ARCH=arm64
else ifeq ($(HOST_ARCH),x86_64)
	NPM_ARCH=x86_64
else
	$(error Unsupported host platform arch $(HOST_ARCH))
endif

.PHONY: all get_nfpm install_apt_deps install_deps clean

all: build_webgui build_webadmin build_package

get_nfpm: $(TARGET_DIR)/nfpm/nfpm

$(TARGET_DIR)/nfpm/nfpm:
	mkdir -p $(@D)
	curl -L https://github.com/goreleaser/nfpm/releases/download/v$(NFPM_VERSION)/nfpm_$(NFPM_VERSION)_Linux_$(NPM_ARCH).tar.gz | tar -xz --directory "$(TARGET_DIR)/nfpm"

install_apt_deps:
	sudo apt-get install build-essential default-mysql-client default-mysql-server nodejs npm curl

install_deps: install_apt_deps get_nfpm

build_common_models:
	cd colte-common-models; npm ci

build_webgui: build_common_models
	cd webgui; npm ci

build_webadmin: build_common_models
	cd webadmin; npm ci

build_package: export VERSION := $(GIT_VERSION)
build_package:
	mkdir -p $(TARGET_DIR)
	cd webgui; cp production.env .env
	cd webadmin; cp production.env .env
	$(TARGET_DIR)/nfpm/nfpm pkg --packager deb --target $(TARGET_DIR)

clean:
	rm -r $(TARGET_DIR)
