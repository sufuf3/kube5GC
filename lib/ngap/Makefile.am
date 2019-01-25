## Process this file with automake to produce Makefile.in

SUBDIRS = asn1c

pkglib_LTLIBRARIES = libngap.la

libngap_la_SOURCES = \
	ngap_decoder.c ngap_encoder.c ngap_free.c \
	$(NULL)

libngap_la_DEPENDENCIES = \
	$(top_srcdir)/lib/nextepc/core/src/libcore.la \
	$(top_srcdir)/lib/ngap/asn1c/libngapasn1c.la \
	$(NULL)

libngap_la_LIBADD = \
	$(top_srcdir)/lib/nextepc/core/src/libcore.la \
	$(top_srcdir)/lib/ngap/asn1c/libngapasn1c.la \
	$(NULL)

AM_LDFLAGS = \
	-version-info @LIBVERSION@

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	-I$(top_srcdir)/lib/ngap/asn1c \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in 
MOSTLYCLEANFILES = *.stackdump
