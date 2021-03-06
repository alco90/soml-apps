/*
 * GPS logger for gpsd
 *
 * Fetches latitude, longitude, elevation, fix mode and time from gpsd
 * and feeds them into OML.
 *
 * To compile: gcc -o gpslogger gpslogger.c -locomm -loml2 -lgps
 *
 * Author: Christoph Dwertmann <christoph.dwertmann@nicta.com.au>, (C) 2010
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2012-2013
 *
 * Copyright 2010-2014 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */

#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <errno.h>
#include <gps.h>
#include <oml2/omlc.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define PACKAGE_STRING __FILE__ " " __DATE__
# define PACKAGE_NAME __FILE__
# define PACKAGE_VERSION __DATE__
#endif

#define OML_FROM_MAIN
#include "gpslogger_oml.h"
static time_t int_time, old_int_time;
static bool verbose = false;
static int stop_loop = 0;

#define MIN(x,y) ((x)<(y)?(x):(y))

struct fixsource_t
{
  char *spec;
  char *server;
  char *port;
  char *device;
};

static struct gps_fix_t gpsfix;
struct fixsource_t source;

static void
log_fix(struct gps_fix_t *fix, struct tm *time)
{
  static int header_printed = 0;
  char time_string [100];
  char mode [10];

  (void)snprintf(time_string, sizeof(time_string), "%04d-%02d-%02dT%02d:%02d:%02dZ",
      time->tm_year+1900, time->tm_mon+1, time->tm_mday,
      time->tm_hour, time->tm_min, time->tm_sec);

  if (fix->mode==MODE_NO_FIX) {
    (void)snprintf (mode, sizeof(mode), "none");
  } else {
    (void)snprintf (mode, sizeof(mode), "%dd", fix->mode);
  }

  if (verbose) {
    if (!header_printed) {
      (void)printf("timestamp,\tlatitude,\tlongitude,\televation,\ttrack,\tspeed,\tclimb,\tfix\n");
      header_printed = 1;
    }
    // print fix data
    (void)printf("%s,\t%f,\t%f,\t%f,\t%f,\t%f,\t%f,\t%s\n",
        time_string, fix->latitude, fix->longitude, fix->altitude, fix->track, fix->speed, fix->climb, mode);
  }

  // log fix data to OML
  oml_inject_gps_data(g_oml_mps_gpslogger->gps_data,
      fix->latitude,
      fix->longitude,
      fix->altitude,
      fix->track,
      fix->speed,
      fix->climb,
      mode,
      time_string);
}

static void
conditionally_log_fix(struct gps_fix_t *gpsfix)
{
  int_time = floor(gpsfix->time);
  if ((int_time != old_int_time) && gpsfix->mode >= MODE_2D) {
    struct tm time;
    old_int_time = int_time;
    gmtime_r(&(int_time), &time);
    log_fix(gpsfix, &time);
  }
}

static void
quit_handler (int signum)
{
  stop_loop = 1;
}

static void
process(struct gps_data_t *gpsdata, char *buf UNUSED, size_t len UNUSED)
{
  /* this is where we implement source-device filtering */
  if (gpsdata->dev.path[0] && source.device!=NULL && strcmp(source.device, gpsdata->dev.path) != 0) {
    return;
  }

  conditionally_log_fix(&gpsdata->fix);
}

static int
socket_mainloop(void)
{
  fd_set fds;
  struct gps_data_t *gpsdata;

#if GPSD_API_MAJOR_VERSION >= 5
  gpsdata = malloc(sizeof(struct gps_data_t));
  if (!gpsdata) {
    logerror("Cannot allocate memory for gpsdata structure: %s\n", strerror(errno));
    exit(1);
  }
  if(gps_open(source.server, source.port, gpsdata) < 0) {
#else
  gpsdata = gps_open(source.server, source.port);
  if (!gpsdata) {
#endif
    logerror("No gpsd running or network error: %s (%d)\n",
        gps_errstr(errno), errno);
    exit(1);
  }

#if GPSD_API_MAJOR_VERSION < 5
  gps_set_raw_hook(gpsdata, process);
#endif
  gps_stream(gpsdata, WATCH_ENABLE|WATCH_NEWSTYLE, NULL);

  while(!stop_loop) {
    int data;
    struct timeval tv;

    FD_ZERO(&fds);
    FD_SET(gpsdata->gps_fd, &fds);

    tv.tv_usec = 250000;
    tv.tv_sec = 0;
    data = select(gpsdata->gps_fd + 1, &fds, NULL, NULL, &tv);

    if (data == -1) {
      logerror("Did not receive data from gpsd: %s\n", strerror(errno));
      break;
    }
    else if (data) {
#if GPSD_API_MAJOR_VERSION >= 5
      gps_read(gpsdata);
      process(gpsdata, NULL, -1);
#else
      gps_poll(gpsdata);
#endif
    }
  }

  omlc_close();
  exit(0);
}

static void
usage(const char* progname)
{
  fprintf(stderr,
      "usage: %s [-h] [-v] [server[:port:[device]]]\n"
      "\tdefaults to '%s localhost:2947'\n"
      "\t-v = verbose mode: print fix data\n",
      progname,
      progname);
}

/* XXX: To be provided by oml2-scaffold templates */
static inline void
oml_inject_metadata(int argc, const char **argv)
{
  int i;
  OmlValueU v;
  omlc_zero(v);

  /* Reconstruct command line */
  size_t cmdline_len = 0;
  for(i = 0; i < argc; i++) {
    cmdline_len += strlen(argv[i]) + 1;
  }
  char cmdline[cmdline_len + 1];
  cmdline[0] = '\0';
  for(i = 0; i < argc; i++) {
    strncat(cmdline, argv[i], cmdline_len);
    cmdline_len -= strlen(argv[i]);
    strncat(cmdline, " ", cmdline_len);
    cmdline_len--;
  }

  omlc_set_string(v, PACKAGE_NAME);
  omlc_inject_metadata(NULL, "appname", &v, OML_STRING_VALUE, NULL);

  omlc_set_string(v, PACKAGE_VERSION);
  omlc_inject_metadata(NULL, "version", &v, OML_STRING_VALUE, NULL);

  omlc_set_string(v, cmdline);
  omlc_inject_metadata(NULL, "cmdline", &v, OML_STRING_VALUE, NULL);
  omlc_reset_string(v);
}

int
main (int argc, const char **argv)
{
  int result;

  loginfo("%s\n", PACKAGE_STRING);

  result = omlc_init ("gpslogger", &argc, argv, NULL);
  if (result == -1) {
    logerror("Could not initialise OML\n");
    exit (1);
  }

  oml_register_mps();

  int ch;

  /* getopt(3) expects arg 2 to be char * const (i.e., pointer to an array of
   * immutable strings), however, omlc_init(3) needs argv to be mutable, so our
   * main() cannot declare it as such we cast it here to make the C compiler
   * happy by letting it know wo know what we are doing
   */
  while ((ch = getopt(argc, (char* const*)argv, "hv")) != -1) {
    switch (ch) {
    case 'v':
      verbose = true;
      break;
    default:
      usage(argv[0]);
      exit(1);
    }
  }

  /* initialises the gpsfix data structure */
  gps_clear_fix(&gpsfix);

  /* catch all interesting signals */
  signal (SIGTERM, quit_handler);
  signal (SIGQUIT, quit_handler);
  signal (SIGINT, quit_handler);

  //openlog ("gpxlogger", LOG_PID | LOG_NDELAY , LOG_DAEMON);

  omlc_start();
  oml_inject_metadata(argc, argv);

  return socket_mainloop();
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
