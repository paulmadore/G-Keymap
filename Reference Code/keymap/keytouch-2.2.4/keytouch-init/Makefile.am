 
bin_PROGRAMS = keytouch-init

AM_CPPFLAGS =	-I. -I../string_to_keycode/ -I../mxml \
		-DSYSCONF_DIR=\""$(sysconfdir)"\" \
		-DPACKAGE_DATA_DIR=\""$(datadir)"\"

AM_CFLAGS= -Wall

keytouch_init_SOURCES =	main.c keytouch-init.h\
			config.c \
			setkeycodes.c \
			error.c \
			memory.c \
			getfd.c getfd.h

keytouch_init_LDADD = ../mxml/libmxml.a ../string_to_keycode/string_to_keycode.o

install-data-local:
	@$(NORMAL_INSTALL)
	
	if [ `which chkconfig` ]; then \
		sed 's,\(^keytouchinit=\)\(.*\),\1$(bindir)/keytouch-init,' keytouch-init.sh \
			> $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		chmod +x $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		chkconfig --add keytouch-init.sh; \
	elif [ `which rc-update` ]; then \
		echo "$(bindir)/keytouch-init" > $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		chmod +x $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		rc-update add keytouch-init.sh default; \
	elif [ -d /etc/rc2.d/ ]; then \
		echo "$(bindir)/keytouch-init" > $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		chmod +x $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		if [ `which update-rc.d` ]; then \
			update-rc.d keytouch-init.sh defaults; \
		else \
			for i in 2 3 4 5; do \
				ln -sf $(DESTDIR)/etc/init.d/keytouch-init.sh $(DESTDIR)/etc/rc${i}.d/S20keytouch-init.sh; \
			done \
		fi; \
	else \
		echo "$(bindir)/keytouch-init" > $(DESTDIR)/etc/rc.d/keytouch-init.sh; \
		chmod +x $(DESTDIR)/etc/rc.d/keytouch-init.sh; \
	fi

uninstall-local:
	if [ `which chkconfig` ]; then \
		chkconfig --del keytouch-init.sh; \
		rm $(DESTDIR)/etc/init.d/keytouch-init.sh; \
	elif [ `which rc-update` ]; then \
		rm $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		rc-update del keytouch-init.sh default; \
	elif [ -d /etc/rc2.d/ ]; then \
		rm $(DESTDIR)/etc/init.d/keytouch-init.sh; \
		if [ `which update-rc.d` ]; then \
			update-rc.d keytouch-init.sh remove; \
		else \
			for i in 2 3 4 5; do \
				rm $(DESTDIR)/etc/rc${i}.d/S20keytouch-init.sh; \
			done \
		fi \
	else \
		rm $(DESTDIR)/etc/rc.d/keytouch-init.sh; \
	fi
