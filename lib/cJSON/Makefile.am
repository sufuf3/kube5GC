pkglib_LTLIBRARIES = libcJSON.la

libcJSON_la_SOURCES = \
	cJSON.h cJSON.c \
	cJSON_Utils.h cJSON_Utils.c \
	$(NULL)

AM_CFLAGS = \
	-fPIC -std=c89 -pedantic -Wall -Werror -Wstrict-prototypes \
	-Wwrite-strings -Wshadow -Winit-self -Wcast-align -Wformat=2 \
	-Wmissing-prototypes -Wstrict-overflow=2 -Wcast-qual -Wc++-compat \
	-Wundef -Wswitch-default -Wconversion \
	$(NULL)

MAINTAINERCLEANFILES = Makefile.in
MOSTLYCLEANFILES = *.stackdump
