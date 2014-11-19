# Disable brp-compress because lineak does this on its own
%define __spec_install_post :

# Set up the main info portions of the spec
Summary:       These are KDE related plugins for the lineakd daemon.
Name:          lineak_kdeplugins
Version:       0.8
Release:       1mdk
License:       GPL
Vendor:        Sheldon Lee Wen <leewsb@hotmail.com>
Url:           http://lineak.sourceforge.net

Packager:      Sheldon Lee Wen <leewsb@hotmail.com>
Group:         Utilities/System
Source:        %{name}-%{version}.tar.gz
BuildRoot:     %{_tmppath}/%{name}-%{version}-root

Requires:      lineakd >= %{ver}
BuildRequires: lineakd >= %{ver}

%description
  This package contains the following plugins:
  juk_plugin  
  kscd_plugin
  kdesktop_plugin  
  kmail_plugin  
  kmix_plugin  
  konqueror_plugin

  These plugins support the following macros:
  juk_plugin:      JUK_PLAY, JUK_PAUSE, JUK_STOP, JUK_PLAYPAUSE, JUK_BACK,
                   JUK_FORWARD, JUK_SEEKBACK, JUK_SEEKFORWARD, JUK_VOLUMEUP,
                   JUK_VOLUMEDOWN, JUK_VOLUMEMUTE, JUK_STARTPLAYINGPLAYLIST,
                   JUK_OPENFILE
  kscd_plugin:     KSCD_PLAY, KSCD_PAUSE, KSCD_STOP, KSCD_PREVIOUS, KSCD_NEXT, KSCD_EJECT,
                   KSCD_TOGGLE_LOOP, KSCD_TOGGLE_SHUFFLE, KSCD_TOGGLE_TIME_DISPLAY,
                   KSCD_CURRENT_TRACK, KSCD_CURRENT_ALBUM, KSCD_CURRENT_ARTIST
  kdesktop_plugin: KDE_LOCK_DESKTOP, KMENU, KDESKTOP_NEXT, KDESKTOP_PREVIOUS,
                   KDESKTOP_EXECUTE, KDE_LOGOUT
  kmail_plugin:    KMAIL_COMPOSE
  kmix_plugin:     KMIX_VOLUP, KMIX_VOLDOWN, KMIX_MUTE
  konq_plugin:     KONQUEROR_BACK, KONQUEROR_FORWARD, KONQUEROR_HOME, KONQUEROR_STOP,
                   KONQUEROR_REFRESH, KONQUEROR_RELOAD, KONQUEROR_NEWTAB, KONQUEROR_NEXTTAB,
                   KONQUEROR_PREVTAB, KONQUEROR_BOOKMARK

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" LDFLAGS=-s %{configure}
%{__make}

%install
[  %{buildroot} != "/" ] && rm -rf %{buildroot}
%{makeinstall}

# make install doesn't seem to understand the install root, so install these manually
install -D -m 644 juk_plugin/.libs/jukplugin.so           %{buildroot}%{_libdir}/lineakd/plugins/jukplugin.so
install -D -m 644 kscd_plugin/.libs/kscdplugin.so         %{buildroot}%{_libdir}/lineakd/plugins/kscdplugin.so
install -D -m 644 kdesktop_plugin/.libs/kdesktopplugin.so %{buildroot}%{_libdir}/lineakd/plugins/ktopplugin.so
install -D -m 644 kmail_plugin/.libs/kmailplugin.so       %{buildroot}%{_libdir}/lineakd/plugins/kmailplugin.so
install -D -m 644 kmix_plugin/.libs/kmixplugin.so         %{buildroot}%{_libdir}/lineakd/plugins/kmixplugin.so
install -D -m 644 konqueror_plugin/.libs/konqplugin.so    %{buildroot}%{_libdir}/lineakd/plugins/konqplugin.so

cd %{buildroot}
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' \
        > $RPM_BUILD_DIR/file.list.%{name}

find . -type f | sed -e 's,^\.,\%attr(-\,root\,root) ,' \
        >> $RPM_BUILD_DIR/file.list.%{name}

find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' \
        >> $RPM_BUILD_DIR/file.list.%{name}

%clean
[  %{buildroot} != "/" ] && rm -rf %{buildroot}
rm -f $RPM_BUILD_DIR/file.list.%{name}

%files -f ../file.list.%{name}

%doc AUTHORS COPYING ChangeLog README TODO
%doc %{_mandir}

%changelog
* Tue Mar 6  2004 Chris Petersen <rpm@forevermore.net>
- Updated spec file to a better template
