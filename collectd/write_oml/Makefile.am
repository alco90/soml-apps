collectd_libdir = @collectd_libdir@

sysconf_DATA = collectd_write_oml.conf
collectd_lib_LTLIBRARIES = write_oml.la

write_oml_la_SOURCES = write_oml.c
write_oml_la_CPPFLAGS = -I @collectd_incdir@
write_oml_la_LDFLAGS = -avoid-version -module -loml2
