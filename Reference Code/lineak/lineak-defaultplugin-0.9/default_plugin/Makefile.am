METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugins_LTLIBRARIES = defaultplugin.la mediadetectplugin.la
pluginsdir = $(PLUGINDIR)
plugindir = $(PLUGINDIR)

defaultplugin_la_METASOURCES = AUTO
defaultplugin_la_LIBADD = -llineak $(XTESTLIB) -lXext -lX11 -lXtst
defaultplugin_la_SOURCES = defaultplugin.cpp soundctrl.cpp defaultplugin.h soundctrl.h
defaultplugin_la_LDFLAGS = -module -avoid-version

mediadetectplugin_la_METASOURCES = AUTO
mediadetectplugin_la_LIBADD = -llineak $(XTESTLIB) -lXext -lX11 -lXtst
mediadetectplugin_la_SOURCES = mediadetectplugin.cpp mediadetectplugin.h mdloader.cpp mdloader.h
mediadetectplugin_la_LDFLAGS = -module -avoid-version

man1_MANS = lineak_defaultplugin.1
DEFS = @DEFS@ -DCONFDIR=\"@sysconfdir@\" 

install-data-local:
	$(mkinstalldirs) ${DESTDIR}$(sysconfdir)/
	$(INSTALL_DATA) $(top_srcdir)/media-detect.conf ${DESTDIR}$(sysconfdir)/media-detect.conf

uninstall-local:
	-rm -f ${DESTDIR}$(sysconfdir)/media-detect.conf

