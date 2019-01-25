## Process this file with automake to produce Makefile.in.

SUBDIRS = http_server

noinst_LTLIBRARIES = libamf.la

libamf_la_SOURCES = \
	amf4g_kdf.h kasumi.h snow_3g.h zuc.h \
	amf4g_event.h amf4g_context.h \
	s1ap_build.h s1ap_handler.h s1ap_conv.h s1ap_path.h \
	ngap_build.h ngap_conv.h ngap_path.h\
	ngap_handler.h \
	amf4g_fd_path.h amf4g_s6a_handler.h \
	nas_conv.h nas_security.h nas_path.h \
	emm_handler.h emm_build.h \
	esm_handler.h esm_build.h \
	amf_json_build.h amf_json_handler.h\
	amf_n11_path.h \
	amf4g_sm.h amf4g_path.h \
	amf4g_kdf.c kasumi.c snow_3g.c zuc.c \
	amf4g_init.c amf4g_event.c amf4g_context.c \
	s1ap_sm.c s1ap_build.c s1ap_handler.c s1ap_conv.c s1ap_path.c \
	ngap_sm.c ngap_build.c ngap_conv.c ngap_path.c\
	ngap_handler.c \
	amf4g_fd_path.c amf4g_s6a_handler.c \
	nas_conv.c nas_security.c nas_path.c \
	emm_sm.c emm_handler.c emm_build.c \
	esm_sm.c esm_handler.c esm_build.c \
	amf_json_build.c amf_json_handler.c\
	amf_n11_path.c \
	amf4g_sm.c amf4g_path.c \
	amf_sbi_path.h amf_sbi_path.c \
	JsonTransform.h \
	amf_n11_handler.c amf_n11_handler.h \
	$(NULL)

if USRSCTP
libamf_la_SOURCES += s1ap_usrsctp.c
else
libamf_la_SOURCES += s1ap_sctp.c
libamf_la_SOURCES += ngap_sctp.c
endif

libamf_la_DEPENDENCIES = \
	$(top_srcdir)/lib/nextepc/s1ap/libs1ap.la \
	$(top_srcdir)/lib/ngap/libngap.la \
	$(top_srcdir)/lib/nextepc/nas/libnas.la \
$(top_srcdir)/lib/nextepc/fd/libfd.la \
	$(top_srcdir)/lib/nextepc/gtp/libgtp.la \
	$(top_srcdir)/lib/cJSON/libcJSON.la \
	$(top_srcdir)/lib/sbiJson/libsbiJson.la \
	$(top_srcdir)/src/app/libapp.la \
	$(NULL)

libamf_la_LIBADD = \
	$(top_srcdir)/lib/nextepc/s1ap/libs1ap.la \
	$(top_srcdir)/lib/ngap/libngap.la \
	$(top_srcdir)/lib/nextepc/nas/libnas.la \
	$(top_srcdir)/lib/nextepc/fd/libfd.la \
	$(top_srcdir)/lib/nextepc/gtp/libgtp.la \
	$(top_srcdir)/lib/cJSON/libcJSON.la \
	$(top_srcdir)/lib/sbiJson/libsbiJson.la \
	$(top_srcdir)/src/app/libapp.la \
	$(NULL)

AM_CPPFLAGS = \
	-I$(top_srcdir)/lib/nextepc/core/include \
	-I$(top_srcdir)/lib/nextepc \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/lib/nextepc/s1ap/asn1c \
	-I$(top_srcdir)/lib/ngap/asn1c \
	-I$(top_srcdir)/lib/@FREEDIAMETER_DIR@/include \
	-I$(top_srcdir)/lib/cJSON \
	-I$(top_srcdir)/lib/sbiJson \
	@MONGOC_CFLAGS@ \
	$(NULL)

AM_CFLAGS = \
	-Wall -Werror \
	$(NULL)

DEFS = \
   	@DEFS@ \
   	-DSYSCONF_DIR=\"$(sysconfdir)/\" \
	-DPREFIX=\"$(prefix)/\"
   	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = core *.stackdump
