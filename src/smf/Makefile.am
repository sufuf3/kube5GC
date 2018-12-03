## Process this file with automake to produce Makefile.in.

SUBDIRS = http_server

noinst_LTLIBRARIES = libsmf.la

libsmf_la_SOURCES = \
	smf_context.h smf_context.c \
	smf_sm.h smf_sm.c \
	smf_event.h smf_event.c \
	smf_gtp_path.h smf_gtp_path.c \
	smf_pfcp_path.h smf_pfcp_path.c \
	smf_fd_path.h smf_fd_path.c \
	smf_n4_build.h smf_n4_build.c \
	smf_n4_handler.h smf_n4_handler.c \
	smf_gx_handler.h smf_gx_handler.c \
	smf_s11_handler.h smf_s11_handler.c \
	smf_s11_build.h smf_s11_build.c \
	smf_sbi_path.h smf_sbi_path.c \
	smf_ipfw.h smf_ipfw.c \
	smf_init.c \
	$(NULL)

libsmf_la_DEPENDENCIES = \
    $(top_srcdir)/lib/fd/libfd.la \
    $(top_srcdir)/lib/gtp/libgtp.la \
    $(top_srcdir)/lib/pfcp/libpfcp.la \
    $(top_srcdir)/lib/ipfw/libipfw.la \
    $(top_srcdir)/src/app/libapp.la \
	$(NULL)

libsmf_la_LIBADD = \
    $(top_srcdir)/lib/fd/libfd.la \
    $(top_srcdir)/lib/gtp/libgtp.la \
    $(top_srcdir)/lib/pfcp/libpfcp.la \
    $(top_srcdir)/lib/ipfw/libipfw.la \
    $(top_srcdir)/src/app/libapp.la \
	$(NULL)

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/core/include \
    -I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/lib/@FREEDIAMETER_DIR@/include \
	@MONGOC_CFLAGS@ \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = core *.stackdump
