## Process this file with automake to produce Makefile.in

INCLUDES = \
	-I. -I../mxml/\
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I../string_to_keycode \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = keytouch-editor-bin

AM_CFLAGS=@CFLAGS@ -Wall

keytouch_editor_bin_SOURCES = \
	main.c keytouch-editor.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	memory.c \
	set_data.c set_data.h \
	error.c \
	evdev.c evdev.h \
	get_scancode.c \
	key_list.c \
	keycodes.c \
	plugins.c \
	config.c \
	email.c email.h \
	default_key_settings.c \
	check_syntax.c check_syntax.h \
	acpi.c acpi.h \
	ud_socket.c ud_socket.h \
	gui.h
	

keytouch_editor_bin_LDADD = ../mxml/libmxml.a ../string_to_keycode/string_to_keycode.o @PACKAGE_LIBS@ $(INTLLIBS)

