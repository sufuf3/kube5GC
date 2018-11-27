## Process this file with automake to produce Makefile.in.

SUBDIRS = app mme hss pcrf smf upf amf

noinst_LTLIBRARIES = libmme.la libhss.la libpcrf.la libsmf.la libupf.la libamf.la libepc.la

libmme_la_SOURCES = mme.c
libmme_la_DEPENDENCIES = $(top_srcdir)/src/mme/libmme.la 
libmme_la_LIBADD = $(top_srcdir)/src/mme/libmme.la

libamf_la_SOURCES = amf.c
libamf_la_DEPENDENCIES = $(top_srcdir)/src/amf/libamf.la 
libamf_la_LIBADD = $(top_srcdir)/src/amf/libamf.la

libhss_la_SOURCES = hss.c
libhss_la_DEPENDENCIES = $(top_srcdir)/src/hss/libhss.la
libhss_la_LIBADD = $(top_srcdir)/src/hss/libhss.la

libpcrf_la_SOURCES = pcrf.c
libpcrf_la_DEPENDENCIES = $(top_srcdir)/src/pcrf/libpcrf.la 
libpcrf_la_LIBADD = $(top_srcdir)/src/pcrf/libpcrf.la

libupf_la_SOURCES = upf.c
libupf_la_DEPENDENCIES = $(top_srcdir)/src/upf/libupf.la
libupf_la_LIBADD = $(top_srcdir)/src/upf/libupf.la

libsmf_la_SOURCES = smf.c
libsmf_la_DEPENDENCIES = $(top_srcdir)/src/smf/libsmf.la 
libsmf_la_LIBADD = $(top_srcdir)/src/smf/libsmf.la 

libepc_la_SOURCES = epc.c
libepc_la_DEPENDENCIES = \
    $(top_srcdir)/src/amf/libamf.la \
    $(top_srcdir)/src/hss/libhss.la \
    $(top_srcdir)/src/smf/libsmf.la \
    $(top_srcdir)/src/pcrf/libpcrf.la \
    $(top_srcdir)/src/upf/libupf.la \
	$(NULL)
libepc_la_LIBADD = \
    $(top_srcdir)/src/amf/libamf.la \
    $(top_srcdir)/src/hss/libhss.la \
    $(top_srcdir)/src/smf/libsmf.la \
    $(top_srcdir)/src/pcrf/libpcrf.la \
    $(top_srcdir)/src/upf/libupf.la \
	$(NULL)

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/core/include \
	-I$(top_srcdir)/src \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = core *.stackdump
