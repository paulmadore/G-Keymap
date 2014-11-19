####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = lineak_logo.xpm noimage.xpm warning.xpm

install-data-local:
	$(mkinstalldirs) $(kde_datadir)/klineakconfig/pics/
	$(INSTALL_DATA) $(srcdir)/lineak_logo.xpm $(kde_datadir)/klineakconfig/pics/lineak_logo.xpm
	$(INSTALL_DATA) $(srcdir)/noimage.xpm $(kde_datadir)/klineakconfig/pics/noimage.xpm
	$(INSTALL_DATA) $(srcdir)/warning.xpm $(kde_datadir)/klineakconfig/pics/warning.xpm
	$(INSTALL_DATA) $(srcdir)/stop.gif $(kde_datadir)/klineakconfig/pics/stop.gif

uninstall-local:
	-rm -f $(kde_datadir)/klineakconfig/pics/lineak_logo.xpm
	-rm -f $(kde_datadir)/klineakconfig/pics/noimage.xpm
	-rm -f $(kde_datadir)/klineakconfig/pics/warning.xpm
	-rm -f $(kde_datadir)/klineakconfig/pics/stop.gif

####### kdevelop will overwrite this part!!! (end)############
##  LinEAK - Linux support for Easy Access and Internet Keyboards      
##  Copyright (C) 2001,2002 Mark Smulders <Mark@PIRnet.nl>             
##  http://lineak.sourceforge.net

##lineakconfigpixmapsdir = ${prefix}/share/lineakconfig/pixmaps
#lineakconfigpixmapsdir = ${PACKAGE_PIXMAPS_DIR}
#lineakconfigpixmaps_DATA = \
#	lineak_logo.xpm \
#	noimage.xpm \
#	warning.xpm \
#	CEAK8.xpm \
#	CIK13.xpm \
#	CIK18g.xpm \
#	CIK18b.xpm \
#	LTCDP.xpm \
#	LTDAK.xpm \
#	LTIK.xpm \
#	MIK.xpm \
#	MIKpro.xpm \
#	MNKpro.xpm \
#	CBLCB.xpm \
#	SVEN2500.xpm
#
#EXTRA_DIST = $(lineakconfigpixmaps_DATA)

