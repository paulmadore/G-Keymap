%define pkg_name    lineak_xosdplugin
%define pkg_ver     0.8
%define pkg_rel     1mdk
%define pkg_copy    GPL

# Use this definition for RedHat/Fedora
#define xosd_name   xosd
# Use this definition for Mandrake
%define xosd_name   libxosd2

# This spec file was generated using Kpp
# If you find any problems with this spec file please report
Summary:   This is the XOSD Onscreen Display plugin for the lineakd daemon. 
Name:      %{pkg_name}
Version:   %{pkg_ver}
Release:   %{pkg_rel}
Copyright: %{pkg_copy}
Vendor:    Sheldon Lee Wen <leewsb@hotmail.com>
Url:       http://lineak.sourceforge.net

Packager:  Sheldon Lee Wen <leewsb@hotmail.com>
Group:     Utilities/System
Source:    %{pkg_name}-%{pkg_ver}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Requires: lineakd >= %{pkg_ver}
BuildRequires: lineakd >= %{pkg_ver}
Requires: %{xosd_name} >= 2.1
BuildRequires: %{xosd_name}-devel >= 2.1

%description
  This plugin allows display using the xosd library.
  It requires that you have the following configuration options
  in your lineakd.conf file:
  Display_plugin = xosd
  It also understands the following configuration directives.
  Display_font =
  Display_color =
  Display_pos =
  Display_align =
  Display_timeout =
  Display_hoffset =
  Display_voffset =
  Display_soffset =

%prep
%setup
export WANT_AUTOCONF_2_5=1
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                --prefix=/usr --sysconfdir=/etc  \
                $LOCALFLAGS

%build

#{configure}

%{__make}

%install
[  %{buildroot} != "/" ] && rm -rf %{buildroot}
%{makeinstall}

# make install doesn't seem to understand the install root, so install this manually
install -D xosd_plugin/.libs/xosdplugin.so %{buildroot}%{_libdir}/lineakd/plugins/xosdplugin.so

%clean
[  %{buildroot} != "/" ] && rm -rf %{buildroot}

%files
%{_libdir}/lineakd/plugins/xosdplugin.so
%{_mandir}/man1/lineak_xosdplugin.1.bz2

%doc AUTHORS COPYING ChangeLog README TODO
%doc %{_mandir}

%changelog
* Tue Feb 10  2004 Chris Petersen <rpm@forevermore.net>
- Updated spec format and made it more compatible with rpmmacros and RedHat/Fedora
