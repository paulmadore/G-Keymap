## Process this file with automake to produce Makefile.in

SUBDIRS = mxml string_to_keycode src po

EXTRA_DIST = \
	autogen.sh \
	keytouch-editor.glade \
	keytouch-editor.gladep

install-data-local:
	@$(NORMAL_INSTALL)
	cp ./keytouch-editor $(DESTDIR)$(bindir)
	chmod 755 $(DESTDIR)$(bindir)/keytouch-editor
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

