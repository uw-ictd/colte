from helpers import constants

import socket


HSS_REMOVE_IMSI_COMMAND = 0
HSS_ADD_IMSI_COMMAND = 1
HSS_SERVICE_PORT = 62880
HSS_SERVICE_ADDR = "127.0.0.1"

SPGW_COMMAND_REQUEST_IMSI_ANSWER_OK = b'0x00'
SPGW_COMMAND_REQUEST_IMSI = b'0x01'
SPGW_COMMAND_REQUEST_IMSI_ANSWER_ERROR = b'0x02'
SPGW_SERVICE_PORT = 62881
SPGW_SERVICE_ADDR = "127.0.0.1"


def query_imsi_from_ip(ip_addr):
     # Network Code
     rawip = socket.inet_aton(ip_addr)
     print(int(rawip))
     message = SPGW_COMMAND_REQUEST_IMSI + "\0\0\0" + rawip + "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
     sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
     sock.sendto(message, (SPGW_SERVICE_ADDR, SPGW_SERVICE_PORT))
     data = sock.recv(24)

     # Check for errors and validate response before continuing
     if (data[0] == SPGW_COMMAND_REQUEST_IMSI_ANSWER_ERROR):
         print("get_imsi_from_ip ERROR: SPGW has no IMSI for IP " + ip_addr)
         return constants.ZERO_IMSI

     if (data[0] != SPGW_COMMAND_REQUEST_IMSI_ANSWER_OK):
         print("get_imsi_from_ip ERROR: Unknown error?!?")
         return constants.ZERO_IMSI

     raw_ip_response = data[4:8]
     if (raw_ip_response != rawip):
         print("get_imsi_from_ip ERROR: IP address in SPGW response doesn't match query?!?")
         print("Origin IP: " + socket.inet_ntoa(rawip))
         print("Received IP: " + socket.inet_ntoa(raw_ip_response))
         return constants.ZERO_IMSI

     imsi = str(data[8:24])
     print("Translated IP address " + ip_addr + " to IMSI " + imsi)

     return imsi


def hss_disable_user(imsi):
    message = "\0" + imsi
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))


# def hss_enable_user(imsi):
    # print "REENABLING USER " + str(imsi)
    # 1: add user back to HSS!
    # message = "\1" + imsi
    # sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))
    # 2: flip user bit
    # 3: send text?!?