METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = kmailplugin.la
plugindir = $(PLUGINDIR)

kmailplugin_la_METASOURCES = AUTO
kmailplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lX11 $(LIBSOCKET) -llineak
kmailplugin_la_SOURCES = kmailplugin.cpp klmailclient.cpp kmailplugin.h klmailclient.h
kmailplugin_la_LDFLAGS = -module -avoid-version
