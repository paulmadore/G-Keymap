
bin_PROGRAMS = keytouch-acpid

AM_CPPFLAGS =	-I. -I../string_to_keycode/ -I../mxml \
		-DSYSCONF_DIR=\""$(sysconfdir)"\" \
		-DPACKAGE_DATA_DIR=\""$(datadir)"\"

AM_CFLAGS=@CFLAGS@ @X_CFLAGS@ -Wall

keytouch_acpid_SOURCES =main.c keytouch-acpid.h\
			config.c \
			error.c \
			memory.c \
			ud_socket.c ud_socket.h \
			acpi_key_list.c

keytouch_acpid_LDADD = ../mxml/libmxml.a ../string_to_keycode/string_to_keycode.o  @X_LIBS@ -lXtst

install-data-local:
	@$(NORMAL_INSTALL)
	XSESSION_KEYTOUCH_ACPID_LINE="if [ ! \`ps -A | grep keytouch-acpid -m 1 -o\` ]; then (keytouch-acpid &> /dev/null &); fi"; \
	XSESSION_DIR="$(DESTDIR)/etc/X11/Xsession.d"; \
	if [ -d $$XSESSION_DIR ]; then \
	  echo '# $$Id: 91keytouch-acpid_launch' > $$XSESSION_DIR/91keytouch-acpid_launch; \
	  echo "$$XSESSION_KEYTOUCH_ACPID_LINE" >> $$XSESSION_DIR/91keytouch-acpid_launch; \
	  chmod 644 $$XSESSION_DIR/91keytouch-acpid_launch; \
	else \
	  XSESSION_FILE="/etc/X11/Xsession"; \
	  if [ "`grep \"$$XSESSION_KEYTOUCHD_LINE\" \"$$XSESSION_FILE\" -m 1 -h`" != "$$XSESSION_KEYTOUCH_ACPID_LINE" ]; then \
	    XSESSION_TMP=`cat "$$XSESSION_FILE"`; \
	    echo "$$XSESSION_KEYTOUCH_ACPID_LINE" > $$XSESSION_FILE; \
	    echo "$$XSESSION_TMP" >> $$XSESSION_FILE; \
	  fi \
	fi
