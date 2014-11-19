METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = konqplugin.la
plugindir = $(PLUGINDIR)

konqplugin_la_METASOURCES = AUTO
konqplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lX11 -llineak
konqplugin_la_SOURCES = konqplugin.cpp konqclient.cpp konqplugin.h konqclient.h
konqplugin_la_LDFLAGS = -module -avoid-version -rdynamic
