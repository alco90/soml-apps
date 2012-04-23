/*
 * GPS logger for gpsd
 *
 * Fetches latitude, longitude, elevation, fix mode and time from gpsd
 * and feeds them into OML.
 *
 * To compile: gcc -o gpslogger gpslogger.c -loml2 -lgps 
 *
 * Author: Christoph Dwertmann <christoph.dwertmann@nicta.com.au>, (C) 2010
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2012
 *
 * Copyright (c) 2010-2012 National ICT Australia (NICTA)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

static time_t int_time, old_int_time;
static time_t timeout = 5; /* seconds */
static bool verbose = false;
OmlMP* mp;

#define MIN(x,y) ((x)<(y)?(x):(y))

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
    (void)printf("lat=\"%f\" lon=\"%f\" ele=\"%f\" track=\"%f\" speed=\"%f\" climb=\"%f\" fix=\"%s\" ts=\"%s\"\n", 
        fix->latitude, fix->longitude, fix->altitude, fix->track, fix->speed, fix->climb, mode, time_string);
    (void)fflush (stdout);
  }

  // log fix data to OML
  OmlValueU values[8];
  omlc_set_double (values[0], fix->latitude);
  omlc_set_double (values[1], fix->longitude);
  omlc_set_double (values[2], fix->altitude);
  omlc_set_double (values[3], fix->track);
  omlc_set_double (values[4], fix->speed);
  omlc_set_double (values[5], fix->climb);
  omlc_set_string (values[6], mode);
  omlc_set_string (values[7], time_string);
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

#if GPSD_API_MAJOR_VERSION >= 5
  gpsdata = malloc(sizeof(struct gps_data_t));
  if (!gpsdata) {
    fprintf(stderr, "error: cannot allocate memory for gpsdata structure\n");
    exit(1);
  }
  gps_open(source.server, source.port, gpsdata);
#else
  gpsdata = gps_open(source.server, source.port);
#endif
  if (!gpsdata) {
    fprintf(stderr,
        "error: no gpsd running or network error: %d, %s\n",
        errno, gps_errstr(errno));
    exit(1);
  }

#if GPSD_API_MAJOR_VERSION < 5
  gps_set_raw_hook(gpsdata, process);
#endif
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
      (void)fprintf(stderr,"error: %s\n", strerror(errno));
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
  return 0;
}

static void usage(const char* progname) 
{
  fprintf(stderr,
      "usage: %s [-h] [-v] [server[:port:[device]]]\n"
      "\tdefaults to '%s localhost:2947'\n"
      "\t-v = verbose mode: print fix data\n",
      progname,
      progname);
}

int main (int argc, const char **argv) 
{
  char *progname=strdup(argv[0]), *p=progname, *p2;
  int result, l;

  /* Get basename */
  p2 = strtok(p, "/");
  while(p2) {
    p = p2;
    p2 = strtok(NULL, "/");
  }
  p2 = p;
  /* The canonical name is `gpslogger-oml2', so it clearly does not start with `om' */
  l = strlen(p);
  if (!strncmp(p, "om", MIN(l,2)) || !strncmp(p, "gpslogger_oml2", MIN(l,13))) {
    fprintf(stderr,
        "warning: binary name `%s' is deprecated and will disappear with OML 2.9.0, please use `gpslogger-oml2' instead\n", p);
  }
  free(progname);

  result = omlc_init ("gpslogger", &argc, argv, NULL);
  if (result == -1) {
    fprintf (stderr, "error: could not initialise OML\n");
    exit (1);
  }

  OmlMPDef mp_def [] =
  {
    { "lat", OML_DOUBLE_VALUE },
    { "lon", OML_DOUBLE_VALUE },
    { "ele", OML_DOUBLE_VALUE },
    { "track", OML_DOUBLE_VALUE },
    { "speed", OML_DOUBLE_VALUE },
    { "climb", OML_DOUBLE_VALUE },
    { "fix", OML_STRING_VALUE },
    { "time", OML_STRING_VALUE },
    { NULL, (OmlValueT)0 }
  };

  mp = omlc_add_mp ("gps_data", mp_def);

  if (mp == NULL) {
    fprintf (stderr, "error: could not register Measurement Point \"gps_data\"");
    exit (1);
  }

  int ch;

  while ((ch = getopt(argc, argv, "hv")) != -1) {
    switch (ch) {
    case 'v':
      verbose = true;
      break;
    default:
      usage(argv[0]);
      exit(1);
    }
  }

  if (optind < argc) {
    gpsd_source_spec(argv[optind], &source);
  } else
    gpsd_source_spec(NULL, &source);

  /* initialises the gpsfix data structure */
  gps_clear_fix(&gpsfix);

  /* catch all interesting signals */
  signal (SIGTERM, quit_handler);
  signal (SIGQUIT, quit_handler);
  signal (SIGINT, quit_handler);

  //openlog ("gpxlogger", LOG_PID | LOG_NDELAY , LOG_DAEMON);

  omlc_start();
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
