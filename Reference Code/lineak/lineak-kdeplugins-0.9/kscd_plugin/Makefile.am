
METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = kscdplugin.la
plugindir = $(PLUGINDIR)

kscdplugin_la_METASOURCES = AUTO
kscdplugin_la_LIBADD = $(LIB_KDECORE) $(LIB_QT) -lX11 -llineak
kscdplugin_la_SOURCES = kscdplugin.cpp kscdclient.cpp kscdplugin.h kscdclient.h
kscdplugin_la_LDFLAGS = -module -avoid-version

