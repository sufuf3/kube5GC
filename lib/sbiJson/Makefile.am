pkglib_LTLIBRARIES = libsbiJson.la

libsbiJson_la_SOURCES = \
	commonJsonKey.h \
	JsonTransform.h JsonTransform.c \
	$(NULL)

libsbiJson_la_DEPENDENCIES = \
	$(top_srcdir)/lib/nextepc/core/src/libcore.la \
	$(top_srcdir)/lib/cJSON/libcJSON.la \
	$(NULL)

libsbiJson_la_LIBADD = \
	$(top_srcdir)/lib/nextepc/core/src/libcore.la \
	$(top_srcdir)/lib/cJSON/libcJSON.la \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	-I$(top_srcdir)/lib/cJSON \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
