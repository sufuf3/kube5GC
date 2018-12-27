## Process this file with automake to produce Makefile.in

pkglib_LTLIBRARIES = libpfcp.la

libpfcp_la_SOURCES = \
	pfcp_message.h pfcp_types.h pfcp_conv.h pfcp_node.h pfcp_path.h pfcp_xact.h \
	pfcp_message.c pfcp_types.c pfcp_conv.c pfcp_node.c pfcp_path.c pfcp_xact.c \
	$(NULL)

libpfcp_la_DEPENDENCIES = \
	$(top_srcdir)/lib/nextepc/core/src/libcore.la \
	$(NULL)

libpfcp_la_LIBADD = \
	$(top_srcdir)/lib/nextepc/core/src/libcore.la \
	$(NULL)

AM_LDFLAGS = \
	-version-info @LIBVERSION@ \
	$(NULL)

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in 
MOSTLYCLEANFILES = *.stackdump
