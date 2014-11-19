#COMPILE_FIRST = klineak

SUBDIRS = juk_plugin amarok_plugin kscd_plugin kmix_plugin kdesktop_plugin kmail_plugin konqueror_plugin po
#LDFLAGS = -lX11 -ldl
EXTRA_DIST = lineak_plugins.kdevprj admin AUTHORS COPYING ChangeLog INSTALL README TODO lineak_plugins.lsm 

man1_MANS = lineak_kdeplugins.1

AUTOMAKE_OPTIONS = foreign 

$(top_srcdir)/configure.in: configure.in.in $(top_srcdir)/subdirs
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common configure.in ;

$(top_srcdir)/subdirs:
	cd $(top_srcdir) && $(MAKE) -f admin/Makefile.common subdirs

$(top_srcdir)/acinclude.m4: $(top_srcdir)/admin/acinclude.m4.in $(top_srcdir)/admin/libtool.m4.in $(top_srcdir)/admin/lineak.m4.in
	@cd $(top_srcdir) && cat admin/acinclude.m4.in admin/libtool.m4.in admin/lineak.m4.in > acinclude.m4

MAINTAINERCLEANFILES = subdirs configure.in acinclude.m4 configure.files 

package-messages:
	$(MAKE) -f admin/Makefile.common package-messages
	$(MAKE) -C po merge

dist-hook:
	cd $(top_distdir) && perl admin/am_edit -padmin
	cd $(top_distdir) && $(MAKE) -f admin/Makefile.common subdirs
