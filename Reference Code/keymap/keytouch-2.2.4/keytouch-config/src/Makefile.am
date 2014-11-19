## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DSYSCONF_DIR=\""$(sysconfdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LIB_DIR=\""$(libdir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I../../mxml \
	@PACKAGE_CFLAGS@ -DGMENU_I_KNOW_THIS_IS_UNSTABLE

bin_PROGRAMS = keytouch

keytouch_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	keytouch.h \
	config.c \
	set_data.c set_data.h \
	error.c \
	memory.c \
	plugin_control.c plugin_control.h \
	key_settings.c key_settings.h \
	plugin.c plugin.h \
	run_browser.c \
	check_desktop.c check_desktop.h \
	application_dialog.c application_dialog.h

keytouch_LDADD = ../../mxml/libmxml.a @PACKAGE_LIBS@ $(INTLLIBS)
