## Process this file with automake to produce Makefile.in.

SUBDIRS = app smf upf amf nextepc

noinst_LTLIBRARIES = libamf.la libsmf.la libupf.la libngc.la

libamf_la_SOURCES = amf.c
libamf_la_DEPENDENCIES = $(top_srcdir)/src/amf/libamf.la 
libamf_la_LIBADD = $(top_srcdir)/src/amf/libamf.la

libsmf_la_SOURCES = smf.c
libsmf_la_DEPENDENCIES = $(top_srcdir)/src/smf/libsmf.la 
libsmf_la_LIBADD = $(top_srcdir)/src/smf/libsmf.la

libupf_la_SOURCES = upf.c
libupf_la_DEPENDENCIES = $(top_srcdir)/src/upf/libupf.la
libupf_la_LIBADD = $(top_srcdir)/src/upf/libupf.la

libngc_la_SOURCES = ngc.c
libngc_la_DEPENDENCIES = \
	$(top_srcdir)/src/nextepc/hss/libhss.la \
	$(top_srcdir)/src/nextepc/pcrf/libpcrf.la \
	$(top_srcdir)/src/amf/libamf.la \
	$(top_srcdir)/src/upf/libupf.la \
	$(top_srcdir)/src/smf/libsmf.la \
	$(NULL)
libngc_la_LIBADD = \
	$(top_srcdir)/src/nextepc/hss/libhss.la \
	$(top_srcdir)/src/nextepc/pcrf/libpcrf.la \
	$(top_srcdir)/src/amf/libamf.la \
	$(top_srcdir)/src/upf/libupf.la \
	$(top_srcdir)/src/smf/libsmf.la \
	$(NULL)

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	-I$(top_srcdir)/src \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = core *.stackdump
