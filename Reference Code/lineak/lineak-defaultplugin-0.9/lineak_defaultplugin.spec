Summary:       This is the default plugin for the lineakd daemon.
Name:          lineak_defaultplugin
Version:       0.8.4
Release:       1
License:       GPL
Vendor:        Sheldon Lee Wen <leewsb@hotmail.com>
Url:           http://lineak.sourceforge.net

Packager:      Sheldon Lee Wen <leewsb@hotmail.com>
Group:         Utilities/System
Source:        %{name}-%{version}.tar.gz
BuildRoot:     %{_tmppath}/%{name}-%{version}-root

Requires:      lineakd >= %{version}
BuildRequires: lineakd >= %{version}

%description
This plugin contains the original lineakd macros:

  EAK_MUTE
  EAK_VOLUP
  EAK_VOLDOWN
  EAK_PCM_UP
  EAK_PCM_DOWN
  EAK_PCM_MUTE
  EAK_EJECT
  EAK_OPEN_TRAY_SCSI
  EAK_OPEN_TRAY
  EAK_CLOSE_TRAY
  EAK_SENDKEYS or EAK_SYM
  EAK_SENDKEYS_ROOT
  EAK_MEDIADETECT

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
                                                                                                           
%doc AUTHORS COPYING ChangeLog README TODO media-detect.conf
%doc %{_mandir}/*/*

%changelog
* Wed Sep 15  2004 Chris Petersen <rpm@forevermore.net>
- Rework how files are picked, and add paths to allow the makeinstall macro to work
* Tue Mar 9  2004 Chris Petersen <rpm@forevermore.net>
- Updated spec file to a better template
