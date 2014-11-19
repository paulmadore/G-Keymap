METASOURCES = AUTO
AM_CPPFLAGS = -I.. -I. $(all_includes) $(ARTS_CFLAGS)
INCLUDES= $(all_includes)

lib_LTLIBRARIES = libklineak.la

libklineak_la_METASOURCES = AUTO
libklineak_la_LIBADD = $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lX11 $(LIBSOCKET) @ARTS_LIBS@ -llineak
libklineak_la_SOURCES = klineaklib.cpp dcopfind.cpp dcopstart.cpp
libklineak_la_LDFLAGS = -module -avoid-version

lineak_HEADERS = klineaklib.h
lineakdir = $(includedir)/lineak
