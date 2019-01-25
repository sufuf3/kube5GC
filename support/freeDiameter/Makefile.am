configfiles = \
	amf.conf \
	hss.conf \
	smf.conf \
	pcrf.conf \
	$(NULL)

pemfiles = \
	cacert.pem \
	amf.key.pem \
	amf.cert.pem \
	hss.key.pem \
	hss.cert.pem \
	smf.key.pem \
	smf.cert.pem \
	pcrf.key.pem \
	pcrf.cert.pem \
	$(NULL)

freeDiameterdir = ${sysconfdir}/free5gc/freeDiameter

freeDiameter_DATA = ${configfiles} ${pemfiles}

MAINTAINERCLEANFILES = Makefile.in 
MOSTLYCLEANFILES = *.stackdump
