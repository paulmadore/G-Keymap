METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes)
INCLUDES= $(all_includes) $(XOSD_CFLAGS)

plugin_LTLIBRARIES = xosdplugin.la
plugindir = $(PLUGINDIR)

xosdplugin_la_METASOURCES = AUTO
xosdplugin_la_LIBADD = -lX11 -llineak $(XOSD_LIBS)
xosdplugin_la_SOURCES = xosdplugin.cpp xosdctrl.cpp xosdplugin.h xosdctrl.h
xosdplugin_la_LDFLAGS = -module -avoid-version

man1_MANS = lineak_xosdplugin.1
