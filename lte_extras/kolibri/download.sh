TEMP_DEB="$(mktemp)" && wget -O "$TEMP_DEB" 'https://learningequality.org/r/kolibri-deb-latest' && sudo  dpkg -i "$TEMP_DEB"
rm -f "$TEMP_DEB"
# After running through the initial setup, run kolibri status and log on to the server to complete first time setup
# To download videos run videos.sh
