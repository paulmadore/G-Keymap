## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = keytouch

keytouch_SOURCES = \
	keytouch.h \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	config.c \
	handle_error.c \
	set_data.c set_data.h \
	keyboard.c \
	keys.c \
	plugin.c \
	keyboard_config.c \
	keyboard_config.h \
	keyboard_list.c \
	keyboard_list.h \
	xmodmap_list.c \
	xmodmap_list.h \
	plugin_list.c \
	plugin_list.h \
	run_browser.c \
	check_desktop.c \
	check_desktop.h

keytouch_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs