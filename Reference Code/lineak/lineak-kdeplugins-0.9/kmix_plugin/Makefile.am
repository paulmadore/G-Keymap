METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = kmixplugin.la
plugindir = $(PLUGINDIR)

kmixplugin_la_METASOURCES = AUTO
kmixplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lX11 $(LIBSOCKET) -llineak
kmixplugin_la_SOURCES = kmixplugin.cpp kmixclient.cpp kmixplugin.h kmixclient.h
kmixplugin_la_LDFLAGS = -module -avoid-version
