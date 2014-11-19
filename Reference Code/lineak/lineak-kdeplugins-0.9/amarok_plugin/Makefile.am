METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes)

plugin_LTLIBRARIES = amarokplugin.la
plugindir = $(PLUGINDIR)

amarokplugin_la_METASOURCES = AUTO
amarokplugin_la_LIBADD = $(LIB_KDECORE) $(LIB_QT) -lX11 -llineak
amarokplugin_la_SOURCES = amarokplugin.cpp amarokclient.cpp amarokplugin.h amarokclient.h
amarokplugin_la_LDFLAGS = -module -avoid-version
