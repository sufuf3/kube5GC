#!/bin/sh

if [ 1 -ne $# ]
then
    echo You must specify output directory : ./make_certs.sh ./freeDiameter

    exit;
fi

rm -rf demoCA
mkdir demoCA
echo 01 > demoCA/serial
touch demoCA/index.txt

# CA self certificate
openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out $1/cacert.pem -keyout cakey.pem -subj /CN=ca.localdomain/C=TW/ST=Taiwan/L=HsinChu/O=free5GC/OU=Tests

#amf
openssl genrsa -out $1/amf.key.pem 1024
openssl req -new -batch -out amf.csr.pem -key $1/amf.key.pem -subj /CN=amf.localdomain/C=TW/ST=Taiwan/L=HsinChu/O=free5GC/OU=Tests
openssl ca -cert $1/cacert.pem -keyfile cakey.pem -in amf.csr.pem -out $1/amf.cert.pem -outdir . -batch

#hss
openssl genrsa -out $1/hss.key.pem 1024
openssl req -new -batch -out hss.csr.pem -key $1/hss.key.pem -subj /CN=hss.localdomain/C=TW/ST=Taiwan/L=HsinChu/O=free5GC/OU=Tests
openssl ca -cert $1/cacert.pem -keyfile cakey.pem -in hss.csr.pem -out $1/hss.cert.pem -outdir . -batch

#smf
openssl genrsa -out $1/smf.key.pem 1024
openssl req -new -batch -out smf.csr.pem -key $1/smf.key.pem -subj /CN=smf.localdomain/C=TW/ST=Taiwan/L=HsinChu/O=free5GC/OU=Tests
openssl ca -cert $1/cacert.pem -keyfile cakey.pem -in smf.csr.pem -out $1/smf.cert.pem -outdir . -batch

#pcrf
openssl genrsa -out $1/pcrf.key.pem 1024
openssl req -new -batch -out pcrf.csr.pem -key $1/pcrf.key.pem -subj /CN=pcrf.localdomain/C=TW/ST=Taiwan/L=HsinChu/O=free5GC/OU=Tests
openssl ca -cert $1/cacert.pem -keyfile cakey.pem -in pcrf.csr.pem -out $1/pcrf.cert.pem -outdir . -batch

rm -rf demoCA
rm -f 01.pem 02.pem 03.pem 04.pem
rm -f cakey.pem
rm -f amf.csr.pem hss.csr.pem smf.csr.pem pcrf.csr.pem
