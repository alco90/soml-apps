ACLOCAL_AMFLAGS = -I ../m4 -Wnone

bin_PROGRAMS = wlanconfig_oml2 gps_oml2

wlanconfig_oml2_SOURCES = c/wlanconfig_oml.c

wlanconfig_oml2_LDADD = -loml2 -locomm

gps_oml2_SOURCES = \
	c/Gps.cpp \
	c/Gps.hpp

gps_oml2_LDADD = -loml2 -locomm

