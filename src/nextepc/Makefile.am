## Process this file with automake to produce Makefile.in.

SUBDIRS = hss pcrf

noinst_LTLIBRARIES = libhss.la libpcrf.la

libhss_la_SOURCES = hss.c
libhss_la_DEPENDENCIES = $(top_srcdir)/src/nextepc/hss/libhss.la
libhss_la_LIBADD = $(top_srcdir)/src/nextepc/hss/libhss.la

libpcrf_la_SOURCES = pcrf.c
libpcrf_la_DEPENDENCIES = $(top_srcdir)/src/nextepc/pcrf/libpcrf.la
libpcrf_la_LIBADD = $(top_srcdir)/src/nextepc/pcrf/libpcrf.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	-I$(top_srcdir)/src/nextepc \
	-I$(top_srcdir)/src/ \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = core *.stackdump
