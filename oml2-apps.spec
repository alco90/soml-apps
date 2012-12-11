%define name		oml2-apps
%define version		2.9.0
%define redmineid	724

BuildRoot:		%{_tmppath}/%{name}-%{version}
Summary:		OML application collection
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
Requires:		gpsd
Requires:		oml2
Requires:		popt
Requires:		libpcap
Requires:		libtrace
Requires:		sigar
BuildRequires:		make
BuildRequires:		gcc
BuildRequires:		gcc-c++
BuildRequires:		glibc-devel
BuildRequires:		gpsd-devel
BuildRequires:		oml2-devel
BuildRequires:		popt-devel
BuildRequires:		libpcap-devel
BuildRequires:		libtrace-devel
BuildRequires:		sigar-devel

%description
This package installs all the OML2 Application packages:
    * gpslogger-oml2: measure GPS data from gpds;
    * iperf-oml2: iperf with OML2 measurement recording;
    * nmetrics-oml2: node metrics using libsigar;
    * otg2-oml2: the otg2/otr2 programs for generating background traffic;
    * ripwavemon-oml2: use OML to monitor the Navini Ripwave WiMAX (kinda) modem;
    * trace-oml2: wrapper around libtrace with OML2 measurement recording;
    * wlanconfig-oml2: record wireless lan information from wlanconfig.

%prep
%setup -q

%build
./configure --prefix /usr
make %{?_smp_mflags}

%install
make DESTDIR=%{buildroot} install-strip

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root,-)
%{_bindir}/*
/usr/lib/*
/usr/share/*