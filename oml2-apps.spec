%define name		oml2-apps
%define version		2.8.0
%define redmineid	724

BuildRoot:		%{_tmppath}/%{name}-%{version}-build
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
BuildRequires:		autocon make automake libtool gcc gcc-c++ gnulib oml2-devel gpsd-devel popt-devel sigar-devel libpcap-devel libtrace-devel ruby

%description
This package provides some OML2 applications:
* gpslogger-oml2 -- measure GPS data from gpds;
* iperf-oml2 -- traffic generator and path capacity testing, with OML2 reporting;
* nmetrics-oml2 -- node metrics using libsigar;
* otg2-oml2 -- OTG2/OTR2 programs for generating and capturing background traffic;
* ripwavemon-oml2 -- parse and report status message from Navini RipWave modems;
* trace-oml2 -- wrapper around libtrace with OML2 measurement recording;
* wlanconfig-oml2 -- record wireless lan information from wlanconfig.

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
