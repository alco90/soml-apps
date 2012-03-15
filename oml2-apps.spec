%define name            oml2-apps
%define version         2.5.0

BuildRoot:              %{_tmppath}/%{name}-%{version}-build
Summary:                OML application collection
License:                MIT
URL:                    http://oml.mytestbed.net/projects/omlapp
Name:                   %{name}
Version:                %{version}
Release:                1
Source:                 http://oml.mytestbed.net/attachments/download/645/oml2-apps-2.5.0.tar.gz
Packager:               Christoph Dwertmann <christoph.dwertmann@nicta.com.au>
Prefix:                 /usr
Group:                  Applications/Internet
BuildRequires:		autoconf make automake libtool gcc gcc-c++ oml2-devel gpsd-devel popt-devel sigar-devel libpcap-devel libtrace-devel ruby

%description
This package provides some OML2 applications:
- oml2-gpslogger -- measure GPS data from gpds;
- oml2-wlanconfig -- record wireless lan information from wlanconfig;
- oml2-trace -- wrapper around libtrace with OML2 measurement recording
- oml2-nmetrics -- node metrics using libsigar;
- oml2-otg2 -- the otg2/otr2 programs for generating background traffic.

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

