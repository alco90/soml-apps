// GPS logger for gpsd
// fetches lat, lon, ele, fix mode and time from gpsd
// and feeds them into OML

// to compile: gcc -o gpslogger gpslogger.c -loml2 -lgps 

#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <errno.h>
#include <gps.h>
#include <oml2/omlc.h>

static char *progname;
static time_t int_time, old_int_time;
static time_t timeout = 5; /* seconds */
static bool verbose = false;
OmlMP* mp;

struct fixsource_t 
{
    char *spec;
    char *server;
    char *port;
    char *device;
};

static void log_fix(struct gps_fix_t *fix, struct tm *time)
{
	char time_string [20];
	char mode [5];
	
	(void)sprintf(time_string, "%04d-%02d-%02dT%02d:%02d:%02dZ",
		time->tm_year+1900, time->tm_mon+1, time->tm_mday,
		time->tm_hour, time->tm_min, time->tm_sec);
		
	if (fix->mode==MODE_NO_FIX)
		(void)sprintf (mode, "none");
    	else
		(void)sprintf (mode, "%dd", fix->mode);
	
	if (verbose) {
		// print fix data
		(void)printf("lat=\"%f\" lon=\"%f\" ele=\"%f\" fix=\"%s\" ts=\"%s\"\n", 
			 fix->latitude, fix->longitude, fix->altitude, mode, time_string);
		(void)fflush (stdout);
	}
	
	// log fix data to OML
	OmlValueU values[5];
	omlc_set_double (values[0], fix->latitude);
	omlc_set_double (values[1], fix->longitude);
	omlc_set_double (values[2], fix->altitude);
	omlc_set_string (values[3], mode);
	omlc_set_string (values[4], time_string);
	omlc_inject (mp, values);
}

static void conditionally_log_fix(struct gps_fix_t *gpsfix)
{
    int_time = floor(gpsfix->time);
    if ((int_time != old_int_time) && gpsfix->mode >= MODE_2D) {
		struct tm time;
		old_int_time = int_time;
		gmtime_r(&(int_time), &time);
		log_fix(gpsfix, &time);
    }
}

static void quit_handler (int signum) 
{
	omlc_close();
    exit(0);
}

static struct gps_fix_t gpsfix;

struct fixsource_t source;

static void process(struct gps_data_t *gpsdata,
	     char *buf UNUSED, size_t len UNUSED)
{
    /* this is where we implement source-device filtering */
    if (gpsdata->dev.path[0] && source.device!=NULL && strcmp(source.device, gpsdata->dev.path) != 0)
	return;

    conditionally_log_fix(&gpsdata->fix);
}

static int socket_mainloop(void)
{
    fd_set fds;
    struct gps_data_t *gpsdata;

    gpsdata = gps_open(source.server, source.port);
    if (!gpsdata) {
	fprintf(stderr,
		"%s: no gpsd running or network error: %d, %s\n",
		progname, errno, gps_errstr(errno));
	exit(1);
    }

    gps_set_raw_hook(gpsdata, process);
    gps_stream(gpsdata, WATCH_ENABLE|WATCH_NEWSTYLE, NULL);

    for(;;) {
	int data;
	struct timeval tv;

	FD_ZERO(&fds);
	FD_SET(gpsdata->gps_fd, &fds);

	tv.tv_usec = 250000;
	tv.tv_sec = 0;
	data = select(gpsdata->gps_fd + 1, &fds, NULL, NULL, &tv);

	if (data == -1) {
	    (void)fprintf(stderr,"%s\n", strerror(errno));
	    break;
	}
	else if (data)
	    gps_poll(gpsdata);
    }
    return 0;
}

static void usage(void) 
{
    fprintf(stderr,
	    "Usage: %s [-h] [-v] [server[:port:[device]]]\n",
	    progname);
    fprintf(stderr,
	    "\tdefaults to '%s localhost:2947'\n",
	    progname);
	fprintf(stderr,
	    "\t-v = verbose mode: print fix data\n");
    exit(1);
}

int main (int argc, char *argv[]) 
{
	// initialize OML
	int result = omlc_init ("GPSlogger", &argc, (const char **)argv, NULL);
  	if (result == -1) {
    	fprintf (stderr, "Could not initialize OML\n");
    	exit (1);
  	}

	OmlMPDef mp_def [] =
	{
	  { "lat", OML_DOUBLE_VALUE },
	  { "lon", OML_DOUBLE_VALUE },
	  { "ele", OML_DOUBLE_VALUE },
	  { "fix", OML_STRING_VALUE },
	  { "time", OML_STRING_VALUE },
	  { NULL, (OmlValueT)0 }
	};
	
	mp = omlc_add_mp ("gps_data", mp_def);

  	if (mp == NULL) {
    	fprintf (stderr, "Error: could not register Measurement Point \"gps_data\"");
    	exit (1);
  	}

    int ch;

	progname = argv[0];
    while ((ch = getopt(argc, argv, "hv")) != -1) {
	switch (ch) {
	case 'v':
	    verbose = true;
	    break;
	default:
 	    usage();
	    /* NOTREACHED */
	}
    }

    if (optind < argc) {
	gpsd_source_spec(argv[optind], &source);
    } else
	gpsd_source_spec(NULL, &source);

    /* initializes the gpsfix data structure */
    gps_clear_fix(&gpsfix);

    /* catch all interesting signals */
    signal (SIGTERM, quit_handler);
    signal (SIGQUIT, quit_handler);
    signal (SIGINT, quit_handler);

    //openlog ("gpxlogger", LOG_PID | LOG_NDELAY , LOG_DAEMON);

	omlc_start();
    return socket_mainloop();
}
