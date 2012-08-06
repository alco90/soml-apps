collectd_libdir = @collectd_libdir@

collectd_lib_LTLIBRARIES = write_oml.la

write_oml_la_SOURCES = write_oml.c
# The following avoids the `error: 'write_oml.la' is not a standard libtool library name' message
write_oml_la_LDFLAGS = -module
