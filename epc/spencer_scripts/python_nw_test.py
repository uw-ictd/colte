import os
import socket

HSS_REMOVE_IMSI_COMMAND = 0
HSS_ADD_IMSI_COMMAND = 1
HSS_SERVICE_PORT = 62880
HSS_SERVICE_ADDR = "127.0.0.1"

SPGW_COMMAND_REQUEST_IMSI_ANSWER_OK = "\0"
SPGW_COMMAND_REQUEST_IMSI = "\1"
SPGW_COMMAND_REQUEST_IMSI_ANSWER_ERROR = "\2"
SPGW_SERVICE_PORT = 62881
SPGW_SERVICE_ADDR = "127.0.0.1"

ZERO_IMSI = "000000000000000"

def get_imsi_from_ip(ip_addr):

	# Network Code
	rawip = socket.inet_aton(ip_addr)
#        print int(rawip)
	message = SPGW_COMMAND_REQUEST_IMSI + "\0\0\0" + rawip + "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(message, (SPGW_SERVICE_ADDR, SPGW_SERVICE_PORT))
	data = sock.recv(24)

        print ':'.join(x.encode('hex') for x in data)

	# Check for errors and validate response before continuing
	if (data[0] == SPGW_COMMAND_REQUEST_IMSI_ANSWER_ERROR):
		print "ERROR: SPGW could not return an IMSI"
		return ZERO_IMSI

        if (data[0] != SPGW_COMMAND_REQUEST_IMSI_ANSWER_OK):
		print "ERROR: Unknown error?!?"
		return ZERO_IMSI

	raw_ip_response = data[4:8]
	if (raw_ip_response != rawip):
		print "ERROR: IP addresses don't match?!?"
                print "Origin IP: " + socket.inet_ntoa(rawip)
                print "Received IP: " + socket.inet_ntoa(raw_ip_response)
		return ZERO_IMSI

	imsi = str(data[8:24])
	print "IMSI : " + imsi
	# return "DONE"

def disable_user(imsi):
	print "CUTTING OFF USER " + str(imsi)
	# 1: send text?!?
	# 2: cut user off!
	message = "\0" + imsi
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))
	# 3: flip user bit

def enable_user(imsi):
	print "REENABLING USER " + str(imsi)
	# 1: add user back to HSS!
	message = "\1" + imsi
	sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
	sock.sendto(message, (HSS_SERVICE_ADDR, HSS_SERVICE_PORT))
	# 2: flip user bit
	# 3: send text?!?

imsi = get_imsi_from_ip("192.168.151.2")
