 
bin_PROGRAMS = keytouchd

AM_CPPFLAGS =	-I. -I../string_to_keycode/ -I../mxml \
		-DSYSCONF_DIR=\""$(sysconfdir)"\" \
		-DPACKAGE_LIB_DIR=\""$(libdir)"\" \
		-DPACKAGE_DATA_DIR=\""$(datadir)"\"

AM_CFLAGS=@CFLAGS@ @X_CFLAGS@ -Wall

keytouchd_SOURCES =	main.c keytouchd.h\
			config.c \
			plugin.c plugin.h \
			error.c \
			memory.c \
			key_settings.c \
			grab_keys.c \
			handle_key.c 

keytouchd_LDADD = ../mxml/libmxml.a ../string_to_keycode/string_to_keycode.o  @X_LIBS@

install-data-local:
	@$(NORMAL_INSTALL)
	XSESSION_KEYTOUCHD_LINE="keytouchd &"; \
	XSESSION_DIR="$(DESTDIR)/etc/X11/Xsession.d"; \
	if [ -d $$XSESSION_DIR ]; then \
		echo '# $$Id: 92keytouchd_launch' > $$XSESSION_DIR/92keytouchd_launch; \
		echo "$$XSESSION_KEYTOUCHD_LINE" >> $$XSESSION_DIR/92keytouchd_launch; \
		chmod 644 $$XSESSION_DIR/92keytouchd_launch; \
	else \
		XSESSION_FILE="/etc/X11/Xsession"; \
		if [ "`grep \"$$XSESSION_KEYTOUCHD_LINE\" \"$$XSESSION_FILE\" -m 1 -h`" != "$$XSESSION_KEYTOUCHD_LINE" ]; then \
			XSESSION_TMP=`cat "$$XSESSION_FILE"`; \
			echo "$$XSESSION_KEYTOUCHD_LINE" > $$XSESSION_FILE; \
			echo "$$XSESSION_TMP" >> $$XSESSION_FILE; \
		fi \
	fi
