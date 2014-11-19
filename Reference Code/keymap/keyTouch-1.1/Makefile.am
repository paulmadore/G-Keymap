## Process this file with automake to produce Makefile.in

mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs

SUBDIRS = src po

EXTRA_DIST = \
	autogen.sh \
	keytouch.glade \
	keytouch.gladep

all-local:
	cd keytouch-plugin && $(MAKE);
	cd plugins && $(MAKE);

clean-local:
	cd keytouch-plugin && $(MAKE) clean;
	cd plugins && $(MAKE) clean;
	
install-data-local:
	@$(NORMAL_INSTALL)
	# Install keytouch-plugin
	cd keytouch-plugin && $(MAKE) BINDIR=$(bindir) install;
	# Install the pixmaps
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi
	# Install the config directory
	if test -d $(srcdir)/config; then \
	    cp -v -R $(srcdir)/config $(DESTDIR)$(pkgdatadir)/; \
	    chown -R $$USER $(DESTDIR)$(pkgdatadir)/config; \
	    chmod -R o+rx $(DESTDIR)$(pkgdatadir)/config; \
	fi
	cp ./keytouch.sh $(DESTDIR)/etc/profile.d/
	chmod o+rx $(DESTDIR)/etc/profile.d/keytouch.sh
	cp -v -R ./doc $(DESTDIR)$(pkgdatadir)
	chmod -R o+r $(DESTDIR)$(pkgdatadir)/doc

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

