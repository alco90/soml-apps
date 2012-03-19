ACLOCAL_AMFLAGS = -I m4 -Wnone

progname = trace
appname = omf_trace

bin_PROGRAMS = oml2-trace

oml2_trace_SOURCES = omf_trace.c

oml2_trace_LDADD = -loml2 -ltrace -lpopt

BUILT_SOURCES = \
	$(appname)_popt.h \
	$(appname)_oml.h

CLEANFILES = \
	$(appname)_popt.h \
	$(appname)_oml.h

EXTRA_DIST=$(appname).app

$(appname)_popt.h:	$(appname).app Makefile
	oml2-scaffold --opts $<

$(appname)_oml.h:	$(appname).app Makefile
	oml2-scaffold --oml $<


installed_name = oml2-$(progname)$(EXEEXT)
installed_link_name = $(DESTDIR)$(bindir)/$(progname)_oml2$(EXEEXT)
install-exec-hook:
	rm -f $(installed_link_name)
	$(LN_S) $(installed_name) $(installed_link_name)
