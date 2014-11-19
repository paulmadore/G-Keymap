PLUGINS = 	WWW \
		filemanager \
		start_prog \
		lock_screen \
		XMMS \
		Aumix_mute \
		Aumix_volume

SO_FILES =	WWW_Home.so \
		WWW_Search.so \
		my_documents.so \
		E-mail.so \
		Messenger.so \
		lock_screen.so \
		XMMS.so \
		XMMS-PlayPause.so \
		XMMS-Stop.so \
		XMMS-Previous.so \
		XMMS-Next.so \
		aumix-mute.so \
		aumix-volume_increase.so \
		aumix-volume_decrease.so

CFLAGS=-I. -O2 -Wall -shared -fPIC
CC=gcc
COMPILE = $(CC) $(CFLAGS)

DESTDIR = ../config/plugin.d

all: $(PLUGINS)

clean:
	cd $(DESTDIR) && $(RM) $(SO_FILES);

WWW: WWW.c
	$(COMPILE) -DKIND_OF_SITE=HOME -o $(DESTDIR)/$@_Home.so $@.c
	$(COMPILE) -DKIND_OF_SITE=SEARCH -o $(DESTDIR)/$@_Search.so $@.c

filemanager: filemanager.c check_desktop.c check_desktop.h
	$(COMPILE) -L/usr/X11R6/lib/ -lX11 -o $(DESTDIR)/my_documents.so $@.c check_desktop.c
	

start_prog: start_prog.c
	$(COMPILE) -DKIND_OF_PROG=EMAIL -o $(DESTDIR)/E-mail.so $@.c
	$(COMPILE) -DKIND_OF_PROG=MESSENGER -o $(DESTDIR)/Messenger.so $@.c

lock_screen: lock_screen.c check_desktop.c check_desktop.h
	$(COMPILE) -L/usr/X11R6/lib/ -lX11 -o $(DESTDIR)/$@.so $@.c check_desktop.c

XMMS: XMMS.c
	$(COMPILE) -o $(DESTDIR)/XMMS.so $@.c
	$(COMPILE) -DACTION=PLAYPAUSE -o $(DESTDIR)/XMMS-PlayPause.so $@.c
	$(COMPILE) -DACTION=STOP -o $(DESTDIR)/XMMS-Stop.so $@.c
	$(COMPILE) -DACTION=PREVIOUS -o $(DESTDIR)/XMMS-Previous.so $@.c
	$(COMPILE) -DACTION=NEXT -o $(DESTDIR)/XMMS-Next.so $@.c

Aumix_mute: aumix-mute.c
	$(COMPILE) -o $(DESTDIR)/aumix-mute.so aumix-mute.c

Aumix_volume: aumix-volume.c
	$(COMPILE) -DVOLUME=INCREASE -o $(DESTDIR)/aumix-volume_increase.so aumix-volume.c
	$(COMPILE) -DVOLUME=DECREASE -o $(DESTDIR)/aumix-volume_decrease.so aumix-volume.c
