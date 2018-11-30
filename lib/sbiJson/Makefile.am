pkglib_LTLIBRARIES = libsbiJson.la

libsbiJson_la_SOURCES = \
	commonJsonKey.h \
	JsonTransform.h JsonTransform.c \
	$(NULL)

libsbiJson_la_DEPENDENCIES = \
    $(top_srcdir)/lib/core/src/libcore.la \
	$(NULL)

libsbiJson_la_LIBADD = \
    $(top_srcdir)/lib/core/src/libcore.la \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

AM_CPPFLAGS = \
    -I$(top_srcdir)/lib/core/include \
	-I$(top_srcdir)/lib/cJSON \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
