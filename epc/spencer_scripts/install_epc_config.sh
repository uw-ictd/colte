cd ..
sudo mkdir -p /usr/local/etc/oai
sudo cp -rp spencer_configs/*.conf /usr/local/etc/oai/
sudo cp -rp spencer_configs/freeDiameter /usr/local/etc/oai/
source oaienv
./scripts/check_hss_s6a_certificate /usr/local/etc/oai/freeDiameter
./scripts/check_mme_s6a_certificate /usr/local/etc/oai/freeDiameter
