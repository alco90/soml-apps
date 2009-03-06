ACLOCAL_AMFLAGS = -I ../m4 -Wnone

bin_PROGRAMS = gps_oml2 app_pcap_oml2 app_sigar_oml2

gps_oml2_SOURCES = \
	c/Gps.cpp \
	c/Gps.hpp

gps_oml2_LDADD = -loml2 -locomm

app_pcap_oml2_SOURCES = \
    c/app_pcap.c \
    c/app_pcap.h

app_pcap_oml2_LDADD = -loml2 -locomm -lpcap

app_sigar_oml2_SOURCES = \
    c/app_sigar.c \
    c/app_sigar.h

app_sigar_oml2_LDADD = -loml2 -locomm -lsigar

