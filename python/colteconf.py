#!/usr/bin/env python3

import logging
import ruamel.yaml
import os

log = logging.getLogger(__name__)

# This version saves comments/edits in YAML files
yaml = ruamel.yaml.YAML()
yaml.indent(sequence=4, mapping=2, offset=2)

# Input
colte_vars = "/etc/colte/config.yml"

if __name__ == "__main__":
    if os.geteuid() != 0:
        log.error("Must run as root!")
        raise PermissionError("The current implementation must run as root")

    # Collect all installed subcomponents
    config_modules = []
    try:
        import colteconf_cn_4g as colte_cn
        config_modules.append(colte_cn)
    except ModuleNotFoundError as e:
        log.info("colte-cn-4g configuration not installed, skipping")

    try:
        import colteconf_prepaid as colte_prepaid
        config_modules.append(colte_prepaid)
    except ModuleNotFoundError as e:
        log.info("colte-prepaid configuration not installed, skipping")

    # Read old vars and update yaml
    with open(colte_vars, "r") as file:
        colte_data = yaml.load(file.read())

        for module in config_modules:
            module.update_all_components(colte_data)

    # Restart everything to pick up new configurations, and don't restart
    # networkd while the EPC or metering are running.
    for module in config_modules:
        module.stop_all_services()

    os.system("systemctl restart systemd-networkd")

    for module in config_modules:
        module.sync_service_state(colte_data)
