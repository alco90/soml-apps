ACLOCAL_AMFLAGS = -I ../../../../m4 -Wnone

AM_CPPFLAGS = \
	-I $(srcdir)/.. \
	-D WITH_OML

noinst_LTLIBRARIES = libotg2generator.la

libotg2generator_la_SOURCES = \
	factory.cpp \
	cbr_generator.cpp \
	expo_generator.cpp \
	randomvariable.cpp \
	cbr_generator.h \
	expo_generator.h \
	randomvariable.h

libotg2generator_la_LIBADD = -locomm

