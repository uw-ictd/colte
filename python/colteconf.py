#!/usr/bin/env python3

import logging
import os

import ruamel.yaml

log = logging.getLogger(__name__)

# This version saves comments/edits in YAML files
yaml = ruamel.yaml.YAML()
yaml.indent(sequence=4, mapping=2, offset=2)

if __name__ == "__main__":
    try:
        import colorlog

        handler = colorlog.StreamHandler()
        handler.setFormatter(
            colorlog.ColoredFormatter(
                "%(log_color)s%(levelname)s(%(name)s): %(message)s"
            )
        )
        log = colorlog.getLogger(__name__)
        log.setLevel(logging.INFO)
        log.addHandler(handler)
    except ModuleNotFoundError as e:
        logging.basicConfig(level=logging.INFO)
        log = logging.getLogger(__name__)
        log.info(
            "System does not support colored logging due to exception:", exc_info=True
        )
        log.info("Continuing operation with standard logging")

    if os.geteuid() != 0:
        log.error("Must run as root!")
        raise PermissionError("The current implementation must run as root")

    # Collect all installed subcomponents
    config_modules = []
    try:
        import colteconf_cn_4g as colte_cn

        config_modules.append(colte_cn)
    except ModuleNotFoundError as e:
        colte_cn = None
        log.info("colte-cn-4g configuration not installed, skipping")

    try:
        import colteconf_prepaid as colte_prepaid

        config_modules.append(colte_prepaid)
    except ModuleNotFoundError as e:
        colte_prepaid = None
        log.info("colte-prepaid configuration not installed, skipping")

    # Read old vars and update yaml
    with open("/etc/colte/config.yml", "r", encoding="utf8") as file:
        colte_data = yaml.load(file.read())

        # Checks for configuration consistency
        if colte_prepaid is None and colte_data["metered"] is True:
            log.warning(
                "Metering is configured, but no metering and accounting package is installed!"
            )

        for module in config_modules:
            module.update_all_components(colte_data)

    # Restart everything to pick up new configurations, and don't restart
    # networkd while the EPC or metering are running.
    for module in config_modules:
        module.stop_all_services()

    os.system("systemctl restart systemd-networkd")

    for module in config_modules:
        module.sync_service_state(colte_data)
