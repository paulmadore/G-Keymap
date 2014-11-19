# This spec file was generated using Kpp
# If you find any problems with this spec file please report
# the error to ian geiser <geiseri@msoe.edu>
Summary:   Program to map and configure your Easy Access Keys
Name:      klineakconfig
Version:   0.8-beta1
Release:   1
Copyright: GPL
Vendor:    Sheldon Lee Wen <leewsb@hotmail.com>
Url:       http://lycadican.sourceforge.net/Lycadican/klineakconfig-0.5pre3.tar.gz
Icon:      klineakconfig.xpm
Packager:  Sheldon Lee Wen <leewsb@hotmail.com>
Group:     X11/KDE/Utilities
Source:    klineakconfig-0.8-beta1.tar.gz
BuildRoot: /usr/src/RPM/BUILD/%{name}-buildroot
Prefix:    /usr
Requires:  lineakd >= 0.8

%description
KlineakConfig is a configuration and control program for the lineakd daemon that provides access to your keyboards easy access keys.

%prep
%setup
CFLAGS="$RPM_OPT_FLAGS" CXXFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix}\
                 \
                $LOCALFLAGS
%build
# Setup for parallel builds
numprocs=`egrep -c ^cpu[0-9]+ /proc/stat || :`
if [ "$numprocs" = "0" ]; then
  numprocs=1
fi

make -j$numprocs

%install
make install-strip DESTDIR=$RPM_BUILD_ROOT

cd $RPM_BUILD_ROOT
find . -type d | sed '1,2d;s,^\.,\%attr(-\,root\,root) \%dir ,' > $RPM_BUILD_DIR/file.list.klineakconfig
find . -type f | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.klineakconfig
find . -type l | sed 's,^\.,\%attr(-\,root\,root) ,' >> $RPM_BUILD_DIR/file.list.klineakconfig

%clean
rm -rf $RPM_BUILD_ROOT/*
rm -rf $RPM_BUILD_DIR/klineakconfig
rm -rf ../file.list.klineakconfig


%files -f ../file.list.klineakconfig
