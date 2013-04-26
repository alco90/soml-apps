%define name		oml2-apps
%define version		2.9.0
%define redmineid	809

BuildRoot:		%{_tmppath}/%{name}-%{version}
Summary:		OML applications collection
License:		MIT
URL:			http://oml.mytestbed.net/projects/omlapp
Name:			%{name}
Version:		%{version}
Release:		1
Source:			http://oml.mytestbed.net/attachments/download/%{redmineid}/oml2-apps-%{version}.tar.gz
Packager:		Christoph Dwertmann <christoph.dwertmann@nicta.com.au>
Prefix:			/usr
Group:			Applications/Internet
Requires:		glibc
Requires:		oml2
Requires:		popt
Requires:		libpcap
Requires:		libtrace
Requires:		sigar
BuildRequires:		tar
BuildRequires:		bzip2
BuildRequires:		make
BuildRequires:		gcc
BuildRequires:		gcc-c++
BuildRequires:		glibc-devel
BuildRequires:		oml2-devel
BuildRequires:		popt-devel
BuildRequires:		libpcap-devel
BuildRequires:		libtrace-devel
BuildRequires:		libxml2-devel
BuildRequires:		sigar-devel
BuildRequires:		wget
%if 0%{?fedora}
Requires:		gpsd
BuildRequires:		gpsd-devel
%endif

%description
This package installs all the OML2 Application packages:
    * collectd-write-oml2: writer plugin for collectd;
    * gpslogger-oml2: measure GPS data from gpsd (only for Fedora);
    * iperf-oml2: iperf with OML2 measurement recording;
    * nmetrics-oml2: node metrics using libsigar;
    * otg2-oml2: the otg2/otr2 programs for generating background traffic;
    * ping-oml2: a simple OML4R wrapper around the system ping tool;
    * ripwavemon-oml2: use OML to monitor the Navini Ripwave WiMAX (kinda) modem;
    * trace-oml2: wrapper around libtrace with OML2 measurement recording;
    * wpamon-oml2: connect to wpa_supplicant's control socket and reports Wi-Fi
      events.

%prep
%setup -q

%build
test -e ../../SOURCES/collectd.tar.gz && tar xfz ../../SOURCES/collectd.tar.gz -C collectd
./configure --prefix=%{_prefix} --sbindir=%{_sbindir} --mandir=%{_mandir} --libdir=%{_libdir} --sysconfdir=%{_sysconfdir} --with-collectd-version=4.10.1
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install-strip

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/*
%{_sysconfdir}/*
%{_libdir}/*
%{_prefix}/share/*

%post
echo "Trying to install the OML4R gem..." >&2
/usr/bin/gem install oml4r || \
        echo "Could not install the oml4r gem; ping-oml2 will not be usable until you do so manually (maybe after installing the rubygems package)" >&2
