%define pkg_name    lineak_defaultplugin
%define pkg_ver     0.8
%define pkg_rel     1mdk
%define pkg_copy    GPL

# This spec file was generated using Kpp
# If you find any problems with this spec file please report
Summary:   This is the default plugin for the lineakd daemon. 
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

%description
  This plugin contains the original lineakd macros.
  EAK_MUTE
  EAK_VOLUP
  EAK_VOLDOWN
  EAK_EJECT

%prep
%setup
export WANT_AUTOCONF_2_5=1
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure \
                --prefix=/usr --sysconfdir=/etc  \
                $LOCALFLAGS

%build
%{__make}

%install
[  %{buildroot} != "/" ] && rm -rf %{buildroot}
%{makeinstall}

# make install doesn't seem to understand the install root, so install this manually
install -D default_plugin/.libs/defaultplugin.so %{buildroot}%{_libdir}/lineakd/plugins/defaultplugin.so

%clean
[  %{buildroot} != "/" ] && rm -rf %{buildroot}

%files
%{_libdir}/lineakd/plugins/defaultplugin.so
%{_mandir}/man1/lineak_defaultplugin.1.bz2

%doc AUTHORS COPYING ChangeLog README TODO
%doc %{_mandir}

%changelog
* Fri Apr 23	2004 Sheldon Lee-Wen <leewsb@hotmail.com>
- Updated the spec file and made it more compatible with rpmmacros and RedHat/Fedora. I blatantly took this stuff from the spec file fix that Chris Petersen did for the xosd plugin, since I don't really know how to make rpms, and honestly couldn't be bothered.

