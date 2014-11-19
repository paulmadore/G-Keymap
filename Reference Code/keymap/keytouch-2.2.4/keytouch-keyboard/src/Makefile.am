## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DSYSCONF_DIR=\""$(sysconfdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I../../mxml -I../../string_to_keycode \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = keytouch-keyboard-bin

keytouch_keyboard_bin_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	keytouch-keyboard.h \
	config.c \
	set_data.c set_data.h \
	error.c \
	memory.c \
	keyboard.c \
	check_syntax.c check_syntax.h

keytouch_keyboard_bin_LDADD = ../../mxml/libmxml.a ../../string_to_keycode/string_to_keycode.o @PACKAGE_LIBS@ $(INTLLIBS)

