#DEFS = @DEFS@ -DCONFDIR=\"@sysconfdir@\" -DROOTDIR=\"@prefix@\"
all_includes = @all_includes@ -I. -I..
USER_INCLUDES = @USER_INCLUDES@ -I. -I..

####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = klineakconfig

## INCLUDES were found outside kdevelop specific part

klineakconfig_SOURCES = klmailclient.cpp klprefs.cpp kprefs.cpp klineakdef.cpp \
		klineakdefui.ui klineak.cpp klineakconfigui.ui klineakconfig.cpp main.cpp \
		advancedkbd.cpp frmadvancedkbd.ui frmadvancedkeyoptionsui.ui advancedkeyoptions.cpp
klineakconfig_LDADD   = -L../klineakconfig $(LIB_KFILE) $(LIB_KDEUI) $(LIB_KDECORE) $(LIB_QT) -lX11 $(LIBSOCKET) -llineak


EXTRA_DIST = main.cpp klineakconfig.cpp klineakconfig.h klineakconfig.desktop lo32-app-klineakconfig.png lo16-app-klineakconfig.png klineakconfigui.ui intl.h klineak.cpp klineak.h hi16-app-klineakconfig.png hi32-app-klineakconfig.png klineakdefui.ui klineakdef.cpp klineakdef.h kprefs.cpp kprefs.h klprefs.cpp klprefs.h klmailclient.cpp klmailclient.h advancedkb.h

install-data-local:
	$(mkinstalldirs) $(kde_appsdir)/Applications/
	$(INSTALL_DATA) $(srcdir)/klineakconfig.desktop $(kde_appsdir)/Applications/klineakconfig.desktop
	$(mkinstalldirs) $(kde_icondir)/locolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/lo32-app-klineakconfig.png $(kde_icondir)/locolor/32x32/apps/klineakconfig.png
	$(mkinstalldirs) $(kde_icondir)/locolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/lo16-app-klineakconfig.png $(kde_icondir)/locolor/16x16/apps/klineakconfig.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/16x16/apps/
	$(INSTALL_DATA) $(srcdir)/hi16-app-klineakconfig.png $(kde_icondir)/hicolor/16x16/apps/klineakconfig.png
	$(mkinstalldirs) $(kde_icondir)/hicolor/32x32/apps/
	$(INSTALL_DATA) $(srcdir)/hi32-app-klineakconfig.png $(kde_icondir)/hicolor/32x32/apps/klineakconfig.png

uninstall-local:
	-rm -f $(kde_appsdir)/Applications/klineakconfig.desktop
	-rm -f $(kde_icondir)/locolor/32x32/apps/klineakconfig.png
	-rm -f $(kde_icondir)/locolor/16x16/apps/klineakconfig.png
	-rm -f $(kde_icondir)/hicolor/16x16/apps/klineakconfig.png
	-rm -f $(kde_icondir)/hicolor/32x32/apps/klineakconfig.png

####### kdevelop will overwrite this part!!! (end)############
# These paths are KDE specific. Use them:
# kde_appsdir         Where your application's menu entry (.desktop) should go to.
# kde_icondir         Where your icon should go to - better use KDE_ICON.
# kde_sounddir        Where your sounds should go to.
# kde_htmldir         Where your docs should go to. (contains lang subdirs)
# kde_datadir         Where you install application data. (Use a subdir)
# kde_locale          Where translation files should go to. (contains lang subdirs)
# kde_cgidir          Where cgi-bin executables should go to.
# kde_confdir         Where config files should go to (system-wide ones with default values).
# kde_mimedir         Where mimetypes .desktop files should go to.
# kde_servicesdir     Where services .desktop files should go to.
# kde_servicetypesdir Where servicetypes .desktop files should go to.
# kde_toolbardir      Where general toolbar icons should go to (deprecated, use KDE_ICON).
# kde_wallpaperdir    Where general wallpapers should go to.
# kde_templatesdir    Where templates for the "New" menu (Konqueror/KDesktop) should go to.
# kde_bindir          Where executables should go to. Use bin_PROGRAMS or bin_SCRIPTS.
# kde_libdir          Where shared libraries should go to. Use lib_LTLIBRARIES.
# kde_moduledir       Where modules (e.g. parts) should go to. Use kde_module_LTLIBRARIES.
# kde_styledir        Where Qt/KDE widget styles should go to (new in KDE 3).
# kde_designerdir     Where Qt Designer plugins should go to (new in KDE 3).

# set the include path for X, qt and KDE
INCLUDES= $(all_includes)

METASOURCES = AUTO

# the library search path. 
klineakconfig_LDFLAGS = $(all_libraries) $(KDE_RPATH)

# Uncomment the following two lines if you add a ui.rc file for your application to make use of
# KDEs XML GUI builing
#rcdir = $(kde_datadir)/klineakconfig
#rc_DATA = klineakconfigui.rc

#WARNING: if you use a ui.rc file above, use:

# messages: rc.cpp

# instead of 

# messages:

messages: 
	LIST=`find . -name \*.h -o -name \*.hh -o -name \*.H -o -name \*.hxx -o -name \*.hpp -o -name \*.cpp -o -name \*.cc -o -name \*.cxx -o -name \*.ecpp -o -name \*.C`; \
	if test -n "$$LIST"; then \
	 $(XGETTEXT) $$LIST -o $(podir)/klineakconfig.pot; \
	fi

noinst_HEADERS = advancedkeyoptions.h
