METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = kdesktopplugin.la
plugindir = $(PLUGINDIR)

kdesktopplugin_la_METASOURCES = AUTO
kdesktopplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lX11 $(LIBSOCKET) -llineak
kdesktopplugin_la_SOURCES = kdesktopplugin.cpp kdesktopplugin.h
kdesktopplugin_la_LDFLAGS = -module -avoid-version
