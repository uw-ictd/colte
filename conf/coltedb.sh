#!/bin/bash
version=0.1.0

display_help() {
    echo "COMMANDS:" >&2
    echo "   add {imsi msisdn ip key opc}: adds a user to the network"
    echo "   remove {imsi}: removes a user from the network"
    echo "   topup {imsi} {money}: adds money to a user's account"
    echo "   topup_data {imsi} {data}: adds bytes to a user's account"
    echo "   admin {imsi}: gives a user administrative privileges"
    echo "   noadmin {imsi}: removes a user's administrative privileges"
    echo "   help: displays this message and exits"
}

echo "coltedb: CoLTE Database Configuration Tool ($version)"

if [ "$#" -lt 1 ]; then
	display_help
	exit 1
fi

if [ "$1" = "help" ]; then
	display_help
	exit 1
fi

if [ "$EUID" -ne 0 ]; then
	echo "coltedb: Must run as root!"
	exit 1
fi

if [ "$1" = "add" ]; then
	if [ "$#" -ne 6 ]; then
		echo "coltedb: incorrect number of args, format is \"coltedb add imsi msisdn ip key opc\""
		exit 1
	fi

	imsi=$1
	msisdn=$2
	ip=$3
	ki=$4
	opc=$5
	open5gsdb add $imsi $ip $ki $opc
	if [ $? = 0 ]; then
		haulagedb add $imsi $msisdn $ip
	fi

	exit 0
fi

if [ "$1" = "remove" ]; then
	if [ "$#" -ne 2 ]; then
		echo "coltedb: incorrect number of args, format is \"coltedb remove imsi\""
		exit 1
	fi

	imsi=$1
	open5gsdb remove $imsi
	haulagedb remove $imsi

	exit 0
fi

if [ "$1" = "topup" ]; then
	if [ "$#" -ne 3 ]; then
		echo "coltedb: incorrect number of args, format is \"coltedb topup imsi money\""
		exit 1
	fi

	imsi=$1
	money=$2
	python /etc/colte/scripts.py db topup $imsi $money

	exit 0
fi

if [ "$1" = "topup_data" ]; then
	if [ "$#" -ne 3 ]; then
		echo "coltedb: incorrect number of args, format is \"coltedb topup_data imsi data\""
		exit 1
	fi

	imsi=$1
	data=$2
	haulagedb topup $imsi $data

	exit 0
fi

if [ "$1" = "admin" ]; then
	if [ "$#" -ne 2 ]; then
		echo "coltedb: incorrect number of args, format is \"coltedb admin imsi\""
		exit 1
	fi

	imsi=$1	
	python /etc/colte/scripts.py db admin $imsi
	exit 0
fi

if [ "$1" = "noadmin" ]; then
	if [ "$#" -ne 2 ]; then
		echo "coltedb: incorrect number of args, format is \"coltedb noadmin imsi\""
		exit 1
	fi

	imsi=$1	
	python /etc/colte/scripts.py db noadmin $imsi
	exit 0
fi

display_help
