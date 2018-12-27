## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = lib src support test

bin_PROGRAMS = nextepc-hssd nextepc-pcrfd open5gc-amfd open5gc-smfd open5gc-upfd open5gc-ngcd

dist_nextepc_hssd_SOURCES = main.c
nextepc_hssd_LDADD = $(top_srcdir)/src/libhss.la

dist_nextepc_pcrfd_SOURCES = main.c
nextepc_pcrfd_LDADD = $(top_srcdir)/src/libpcrf.la

dist_open5gc_amfd_SOURCES = main.c
open5gc_amfd_LDADD = $(top_srcdir)/src/libamf.la

dist_open5gc_smfd_SOURCES = main.c
open5gc_smfd_LDADD = $(top_srcdir)/src/libsmf.la

dist_open5gc_upfd_SOURCES = main.c
open5gc_upfd_LDADD = $(top_srcdir)/src/libupf.la

dist_open5gc_ngcd_SOURCES = main.c
open5gc_ngcd_LDADD = $(top_srcdir)/src/libngc.la

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	-I$(top_srcdir)/src/app \
	$(NULL)

install-data-hook:
	$(MKDIR_P) $(DESTDIR)/$(localstatedir)/log/open5gc
	$(MKDIR_P) $(DESTDIR)/$(localstatedir)/run/nextepc-hssd
	$(MKDIR_P) $(DESTDIR)/$(localstatedir)/run/nextepc-pcrfd
	$(MKDIR_P) $(DESTDIR)/$(localstatedir)/run/open5gc-amfd
	$(MKDIR_P) $(DESTDIR)/$(localstatedir)/run/open5gc-smfd
	$(MKDIR_P) $(DESTDIR)/$(localstatedir)/run/open5gc-upfd
	$(MKDIR_P) $(DESTDIR)/$(localstatedir)/run/open5gc-ngcd

CLEANFILES = -R data
DISTCLEANFILES = $(DIST_ARCHIVES)
MAINTAINERCLEANFILES = \
	configure config.in \
	aclocal.m4 m4/ltsugar.m4 m4/libtool.m4 m4/ltversion.m4 \
	m4/lt~obsolete.m4 m4/ltoptions.m4 \
	build-aux/ar-lib build-aux/config.guess build-aux/depcomp \
	build-aux/ltmain.sh build-aux/test-driver build-aux/compile \
	build-aux/config.sub build-aux/missing build-aux/install-sh \
	Makefile.in
MOSTLYCLEANFILES = core *.stackdump
