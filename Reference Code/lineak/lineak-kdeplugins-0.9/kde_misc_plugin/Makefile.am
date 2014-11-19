METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes) $(ARTS_CFLAGS)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = kdemiscplugin.la
plugindir = $(PLUGINDIR)

kdemiscplugin_la_METASOURCES = AUTO
kdemiscplugin_la_LIBADD = $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lX11 $(LIBSOCKET) @ARTS_LIBS@ -llineak -lsoundserver_idl
kdemiscplugin_la_LDFLAGS = -module -avoid-version
kdemiscplugin_la_SOURCES = kdemiscplugin.cpp artsclient.cpp kdemiscplugin.h artsclient.h
