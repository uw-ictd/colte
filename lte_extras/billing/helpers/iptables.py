import logging
import subprocess


def enable_forward_filter(ip_address):
    """Adds a forwarding filter to the system's IPTables configuration.

    This prevents traffic from the provided ip address from being forwarded.

    Args:
        ip_address: The IP Address to filter out.

    Raises:
        IOError: the IPTables filter was unable to be set
    """
    p = subprocess.Popen(["iptables", "-I", "FORWARD", "-s", ip_address, "-j", "REJECT"], stdout=subprocess.PIPE)
    output, err = p.communicate()
    logging.debug(output)
    if err:
        raise IOError(err)


def disable_forward_filter(ip_address):
    """Removes a forwarding filter from the system's IPTables configuration.

    Args:
        ip_address: The IP Address to not filter.

    Raises:
        IOError: the IPTables filter was unable to be removed.
    """
    p = subprocess.Popen(["iptables", "-D", "FORWARD", "-s", ip_address, "-j", "REJECT"], stdout=subprocess.PIPE)
    output, err = p.communicate()
    logging.debug(output)
    if err:
        raise IOError(err)
