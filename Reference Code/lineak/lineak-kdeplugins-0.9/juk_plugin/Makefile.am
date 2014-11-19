METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = jukplugin.la
plugindir = $(PLUGINDIR)

jukplugin_la_METASOURCES = AUTO
jukplugin_la_LIBADD = $(LIB_KDECORE) $(LIB_QT) -lX11 -llineak
jukplugin_la_SOURCES = jukplugin.cpp jukclient.cpp jukplugin.h jukclient.h
jukplugin_la_LDFLAGS = -module -avoid-version
