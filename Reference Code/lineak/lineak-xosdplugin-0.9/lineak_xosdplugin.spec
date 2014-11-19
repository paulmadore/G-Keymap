# If this is fedora or redhat, xosd is called "xosd" - otherwise call it "libxosd2"
%define xosd_name %((test -e /etc/fedora-release && rpm -q fedora-release --qf "xosd") || (test -e /etc/redhat-release && rpm -q redhat-release --qf "xosd") || echo "libxosd2")

Summary:       This is the XOSD Onscreen Display plugin for the lineakd daemon.
Name:          lineak_xosdplugin
Version:       0.8.3
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
Requires:      %{xosd_name}       >= 2.1
BuildRequires: %{xosd_name}-devel >= 2.1

%description
This plugin allows display using the xosd library.
It requires that you have the following configuration options
in your lineakd.conf file:

  Display_plugin = xosd

It also understands the following configuration directives.

  Display_font    =
  Display_color   =
  Display_pos     =
  Display_align   =
  Display_timeout =
  Display_hoffset =
  Display_voffset =
  Display_soffset =

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
* Tue Mar 9  2004 Chris Petersen <rpm@forevermore.net>
- Updated spec file to a better template that should work with redhat and mandrake
* Tue Feb 10  2004 Chris Petersen <rpm@forevermore.net>
- Updated spec format and made it more compatible with rpmmacros and RedHat/Fedora
