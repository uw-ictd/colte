#!/usr/bin/env python3

import logging
import ruamel.yaml
from ruamel.yaml.comments import CommentedMap
import os
from netaddr import IPNetwork

# This version saves comments/edits in YAML files
yaml = ruamel.yaml.YAML()
yaml.indent(sequence=4, mapping=2, offset=2)

# Input
colte_config_file = "/etc/colte/config.yml"

log = logging.getLogger(__name__)

def update_all_components(colte_data):
    """Update configurations of all billing components to match the colte config
    """
    # Update yaml files
    _update_haulage(colte_data)

    # Update other files
    _update_env_file("/etc/colte/webadmin.env", colte_data)
    _update_env_file("/etc/colte/webgui.env", colte_data)


def stop_all_services():
    """Ensure all billing services are stopped
    """
    _control_metering_services("stop")

def sync_service_state(colte_data):
    """Start enabled services and update enabled/disabled state
    """
    if colte_data["metered"] == True:
        _control_metering_services("start")
        _control_metering_services("enable")
    else:
        _control_metering_services("disable")


def _update_env_file(file_name, colte_data):
    env_data = {}
    with open(file_name, "r") as f:
        env_data = yaml.load(f.read().replace("=", ": "))

        env_data["DB_USER"] = colte_data["mysql_user"]
        env_data["DB_PASSWORD"] = colte_data["mysql_password"]
        env_data["DB_NAME"] = colte_data["mysql_db"]

    # Get data in YAML format
    with open(file_name, "w") as f:
        # Save the results
        yaml.dump(env_data, f)

    # Update data in correct format
    new_text = ""
    with open(file_name, "r") as f:
        new_text = f.read().replace(": ", "=")

    # Save in correct format
    with open(file_name, "w") as f:
        f.write(new_text)


def _update_haulage(colte_data):
    haulage_config_file = "/etc/haulage/config.yml"
    haulage_data = {}
    with open(haulage_config_file, "r") as f:
        haulage_data = yaml.load(f.read())

        # Create fields in the data if they do not yet exist
        _create_field_if_not_exist(haulage_data, ["custom"], CommentedMap())

        haulage_data["userSubnet"] = colte_data["lte_subnet"]
        haulage_data["ignoredUserAddresses"] = [
            str(IPNetwork(colte_data["lte_subnet"])[1])
        ]

        haulage_data["custom"]["dbUser"] = colte_data["mysql_user"]
        haulage_data["custom"]["dbLocation"] = colte_data["mysql_db"]
        haulage_data["custom"]["dbPass"] = colte_data["mysql_password"]

        # Hard-coded values
        haulage_data["interface"] = "ogstun"

    with open(haulage_config_file, "w") as f:
        # Save the results
        yaml.dump(haulage_data, f)


def _create_field_if_not_exist(dictionary, field_path, value):
    current_entry = dictionary
    for i, field in enumerate(field_path):
        if (i == len(field_path) - 1) and (
            field not in current_entry or current_entry[field] is None
        ):
            current_entry[field] = value
        else:
            try:
                current_entry = current_entry[field]
            except KeyError as e:
                log.error("Failed to create key at path %s", str(field_path))
                log.error("Current configuration state is: %s", str(dictionary))
                raise KeyError (
                    "Failed to create key at path {}, with base error {}".format(
                        field_path, e
                    )
                ) from e


def _control_metering_services(action):
    os.system("systemctl {} haulage colte-webgui colte-webadmin".format(action))


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    if os.geteuid() != 0:
        log.error("Must run as root!")
        raise PermissionError("The current implementation must run as root")

    # Read old vars and update yaml
    with open(colte_config_file, "r") as file:
        colte_config_data = yaml.load(file.read())

    update_all_components(colte_config_data)

    # Restart everything to pick up new configurations.
    stop_all_services()
    sync_service_state(colte_config_data)

