
VIDEO_WEBAPP_VERSION=0.1

TARGET_DIR=./BUILD/

target:
	mkdir -p $(TARGET_DIR)

webapp: target
	fpm --input-type dir \
		--output-type deb \
		--force \
		--vendor uw-ictd \
		--maintainer durandn@cs.washington.edu \
		--description "Web application for Local App" \
		--url "https://github.com/uw-ictd/colte" \
		--name video_webapp \
		--version $(VIDEO_WEBAPP_VERSION) \
		--package $(TARGET_DIR) \
		--depends 'python3.7, python3-pip' \
		--after-install ./package/video_webapp/postinst \
		--after-remove ./package/video_webapp/postrm \
		./video_webapp/=/usr/bin/video_webapp \
		./package/video_webapp/video_webapp.service=/etc/systemd/system/video_webapp.service \

