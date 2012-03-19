bin_PROGRAMS = ripwave-monitor
ripwave_monitor_SOURCES = ripwave-monitor.c
ripwave_monitor_LDADD = -loml2

BUILT_SOURCES = ripwave_monitor_oml.h
EXTRA_DIST = navini-protocol.txt ripwave_monitor.app

%_oml.h: %.app
	oml2-scaffold --oml $^

clean-local:
	rm -f ripwave_monitor_oml.h
