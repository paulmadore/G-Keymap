
SUBDIRS = mxml string_to_keycode keytouchd keytouch-acpid keytouch-init plugins

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(datadir)/keytouch
	cp -R keyboards $(DESTDIR)$(datadir)/keytouch
	chmod 644 $(DESTDIR)$(datadir)/keytouch/keyboards/*
	#mkdir -m 755 $(DESTDIR)$(sysconfdir)/keytouch
	[ -d $(DESTDIR)$(sysconfdir) ] || mkdir -m 755 $(DESTDIR)$(sysconfdir)
	[ -d $(DESTDIR)$(sysconfdir)/keytouch ] || mkdir -m 755 $(DESTDIR)$(sysconfdir)/keytouch
