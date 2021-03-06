# Note that this is NOT a relocatable package
%define ver      0.1.10
%define RELEASE  1
%define rel      %{?CUSTOM_RELEASE} %{!?CUSTOM_RELEASE:%RELEASE}
%define prefix   /usr

Summary: Application access to USB devices
Name: libusb
Version: %ver
Release: %rel
Copyright: LGPL
Group: Libraries
Source: ftp://ftp.libusb.sourceforge.net/pub/libusb/libusb-%{ver}.tar.gz
BuildRoot: /var/tmp/libusb-%{PACKAGE_VERSION}-root
URL: http://libusb.sourceforge.net

%description
Provides a library for application access to USB devices. Development
libs and headers are in libusb-devel

%package devel
Summary: Application USB device access library
Group: System Environment/Libraries

%description devel
Static libraries and header files for the USB device access library


%changelog

%prep
%setup

%build
CFLAGS="$RPM_OPT_FLAGS" ./configure --prefix=%{prefix}
make

%install
rm -rf $RPM_BUILD_ROOT

make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-, root, root)

%doc AUTHORS COPYING LICENSE NEWS README
%{prefix}/lib/libusb-0.1.so.*
%{prefix}/lib/libusbpp-0.1.so.*

%files devel
%defattr(-, root, root)

%{prefix}/lib/*.so
%{prefix}/lib/*a
%{prefix}/include/*
%{prefix}/bin/*
