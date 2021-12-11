#!/usr/bin/env sh

# Install and start web-services
systemctl daemon-reload
systemctl start colte-webgui
systemctl start colte-webadmin
