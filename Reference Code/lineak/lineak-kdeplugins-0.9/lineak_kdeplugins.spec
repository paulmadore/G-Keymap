Summary:       These are KDE related plugins for the lineakd daemon.
Name:          lineak_kdeplugins
Version:       0.8.3
Release:       1
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
  amarok_plugin
  juk_plugin
  kdesktop_plugin
  kmail_plugin
  kmix_plugin
  konqueror_plugin

  These plugins support the following macros:
  amarok_plugin:   AMAROK_PLAY, AMAROK_PAUSE, AMAROK_STOP, AMAROK_PLAYPAUSE, 
                   AMAROK_BACK, AMAROK_FORWARD, AMAROK_SEEK, AMAROK_ADDMEDIA, 
                   AMAROK_VOLUMEUP, AMAROK_VOLUMEDOWN, AMAROK_VOLUMEMUTE, 
                   AMAROK_TOGGLEPLAYLIST, AMAROK_ENABLERANDOM
  juk_plugin:      JUK_PLAY, JUK_PAUSE, JUK_STOP, JUK_PLAYPAUSE, JUK_BACK,
                   JUK_FORWARD, JUK_SEEKBACK, JUK_SEEKFORWARD, JUK_VOLUMEUP,
                   JUK_VOLUMEDOWN, JUK_VOLUMEMUTE, JUK_STARTPLAYINGPLAYLIST,
                   JUK_OPENFILE
  kdesktop_plugin: KDE_LOCK_DESKTOP, KMENU, KDESKTOP_NEXT, KDESKTOP_PREVIOUS,
                   KDESKTOP_EXECUTE, KDE_LOGOUT
  kmail_plugin:    KMAIL_COMPOSE
  kmix_plugin:     KMIX_VOLUP, KMIX_VOLDOWN, KMIX_MUTE
  konq_plugin:     KONQUEROR

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" LDFLAGS=-s %{configure}
%{__make}

%install
[  %{buildroot} != "/" ] && rm -rf %{buildroot}
%{makeinstall} prefix= DESTDIR=%{buildroot} mandir=%{_mandir}

%clean
[  %{buildroot} != "/" ] && rm -rf %{buildroot}
rm -f $RPM_BUILD_DIR/file.list.%{name}

%files
%defattr(-,root,root,-)
%{_libdir}/lineakd/plugins/*

%doc AUTHORS COPYING ChangeLog README TODO
%doc %{_mandir}/*/*

%changelog
* Wed Sep 15  2004 Chris Petersen <rpm@forevermore.net>
- Rework how files are picked, and add paths to allow the makeinstall macro to work
* Tue Mar 6  2004 Chris Petersen <rpm@forevermore.net>
- Updated spec file to a better template
