## Process this file with automake to produce Makefile.in.

noinst_LTLIBRARIES = libupf.la

libupf_la_SOURCES = \
	upf_ipfw.h \
	upf_event.h upf_context.h upf_sm.h \
	upf_gtp_path.h \
	upf_pfcp_path.h upf_n4_build.h upf_n4_handler.h \
	upf_ipfw.c \
	upf_init.c upf_event.c upf_context.c upf_sm.c \
	upf_gtp_path.c \
	upf_pfcp_path.c upf_n4_build.c upf_n4_handler.c \
	$(NULL)

libupf_la_DEPENDENCIES = \
	$(top_srcdir)/lib/nextepc/gtp/libgtp.la \
	$(top_srcdir)/lib/pfcp/libpfcp.la \
	$(top_srcdir)/lib/ipfw/libipfw.la \
	$(top_srcdir)/src/app/libapp.la \
	$(NULL)

libupf_la_LIBADD = \
	$(top_srcdir)/lib/nextepc/gtp/libgtp.la \
	$(top_srcdir)/lib/pfcp/libpfcp.la \
	$(top_srcdir)/lib/ipfw/libipfw.la \
	$(top_srcdir)/src/app/libapp.la \
	$(NULL)

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	-I$(top_srcdir)/lib/nextepc \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	@MONGOC_CFLAGS@ \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = core *.stackdump
