#
# Copyright (c) 2015, EURECOM (www.eurecom.fr)
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice, this
#    list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and documentation are those
# of the authors and should not be interpreted as representing official policies,
# either expressed or implied, of the FreeBSD Project.


# ARG is REALM
# BY DEFAULT REALM IS "eur"

DEFAULTREALMVALUE="eur"
REALM=${1:-$DEFAULTREALMVALUE}

rm -rf demoCA
mkdir demoCA
echo 01 > demoCA/serial
touch demoCA/index.txt

user=$(whoami)
HOSTNAME=$(hostname -f)

echo "Creating HSS certificate for user '$HOSTNAME'.'$REALM'"
# 
# # CA self certificate
# openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out cacert.pem -keyout cakey.pem -subj /CN=test.fr/C=FR/ST=Biot/L=Aix/O=test.fr/OU=mobiles
# 
# openssl genrsa -out hss.key.pem 1024
# openssl req -new -batch -out hss.csr.pem -key hss.key.pem -subj /CN=hss.test.fr/C=FR/ST=Biot/L=Aix/O=test.fr/OU=mobiles
# openssl ca -cert cacert.pem -keyfile cakey.pem -in hss.csr.pem -out hss.cert.pem -outdir . -batch

# Create a Root Certification Authority Certificate
openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out hss.cacert.pem -keyout hss.cakey.pem -subj /CN=$REALM/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM

# Generate a Private Key
openssl genrsa -out hss.key.pem 1024

# Generate a CSR (Certificate Signing Request) that will be self-signed
openssl req -new -batch -out hss.csr.pem -key hss.key.pem -subj /CN=$HOSTNAME.$REALM/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM

# Certification authority
openssl ca -cert hss.cacert.pem -keyfile hss.cakey.pem -in hss.csr.pem -out hss.cert.pem -outdir . -batch

if [ ! -d /usr/local/etc/freeDiameter ]
then
    echo "Creating non existing directory: /usr/local/etc/freeDiameter/"
    sudo mkdir /usr/local/etc/freeDiameter/
fi

sudo cp -upv hss.cakey.pem hss.cert.pem hss.cacert.pem hss.key.pem /usr/local/etc/freeDiameter/

# openssl genrsa -out $hss.key.pem 1024
# openssl req -new -batch -out $hss.csr.pem -key $hss.key.pem -subj /CN=$hss.test.fr/C=FR/ST=Biot/L=Aix/O=test.fr/OU=mobiles
# openssl ca -cert cacert.pem -keyfile cakey.pem -in $hss.csr.pem -out $hss.cert.pem -outdir . -batch
