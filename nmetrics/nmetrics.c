/*
 * OML application reporting various node metrics, such as cpu, memory, ...
 *
 * This application is using the libsigar library to report regularily through
 * the OML framework on various node metrics, such as cpu load, memory and
 * network usage.
 *
 * Author: Guillaume Jourjon <guillaume.jourjon@nicta.com.au>, (C) 2008
 * Author: Max Ott  <max.ott@nicta.com.au>, (C) 2009
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2012
 *
 * Copyright 2007-2013 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define PACKAGE_STRING __FILE__
#endif

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <sigar.h>
#include <oml2/omlc.h>

#define OML_FROM_MAIN
#include "nmetrics_oml.h"
#define USE_OPTS
#include "nmetrics_popt.h"

#define MIN(x,y) ((x)<(y)?(x):(y))

typedef struct _if_monitor_t {
  char           if_name[64];

  int            not_first;
  sigar_uint64_t start_rx_packets;
  sigar_uint64_t start_rx_bytes;
  sigar_uint64_t start_rx_errors;
  sigar_uint64_t start_rx_dropped;
  sigar_uint64_t start_rx_overruns;
  sigar_uint64_t start_rx_frame;
  sigar_uint64_t start_tx_packets;
  sigar_uint64_t start_tx_bytes;
  sigar_uint64_t start_tx_errors;
  sigar_uint64_t start_tx_dropped;
  sigar_uint64_t start_tx_overruns;
  sigar_uint64_t start_tx_collisions;
  sigar_uint64_t start_tx_carrier;

  struct _if_monitor_t* next;
} if_monitor_t;

static int stop_loop = 0;

static void
quit_handler (int signum)
{
  stop_loop = 1;
}

void
run( opts_t* opts, oml_mps_t* oml_mps, if_monitor_t* first_if)
{
  sigar_t* sigar_p;
  if_monitor_t* if_p;
  sigar_cpu_t c;
  sigar_mem_t m;
  sigar_net_interface_stat_t is;
  while(!stop_loop) {
    sigar_open(&sigar_p);

    if (opts->report_cpu) {

      sigar_cpu_get(sigar_p, &c);
      oml_inject_cpu(oml_mps->cpu,
          c.user,
          (uint64_t) c.sys,
          (uint64_t)c.nice,
          (uint64_t)c.idle,
          (uint64_t)c.wait,
          (uint64_t)c.irq,
          (uint64_t)c.soft_irq,
          (uint64_t)c.stolen,
          (uint64_t)c.total);
    }
    if (opts->report_memory) {

      sigar_mem_get(sigar_p, &m);

      oml_inject_memory(oml_mps->memory,
          (uint64_t)m.ram,
          (uint64_t)m.total,
          (uint64_t)m.used,
          (uint64_t)m.free,
          (uint64_t)m.actual_used,
          (uint64_t)m.actual_free);
    }

    if_p = first_if;
    while (if_p) {

      sigar_net_interface_stat_get(sigar_p, if_p->if_name, &is);
      if (! if_p->not_first) {
        if_p->start_rx_packets = is.rx_packets;
        if_p->start_rx_bytes = is.rx_bytes;
        if_p->start_rx_errors = is.rx_errors;
        if_p->start_rx_dropped = is.rx_dropped;
        if_p->start_rx_overruns = is.rx_overruns;
        if_p->start_rx_frame = is.rx_frame;
        if_p->start_tx_packets = is.tx_packets;
        if_p->start_tx_bytes = is.tx_bytes;
        if_p->start_tx_errors = is.tx_errors;
        if_p->start_tx_dropped = is.tx_dropped;
        if_p->start_tx_overruns = is.tx_overruns;
        if_p->start_tx_collisions = is.tx_collisions;
        if_p->start_tx_carrier = is.tx_carrier;
        if_p->not_first = 1;
      }
      oml_inject_network(oml_mps->network,
          if_p->if_name,
          (uint64_t)(is.rx_packets - if_p->start_rx_packets),
          (uint64_t)(is.rx_bytes - if_p->start_rx_bytes),
          (uint64_t)(is.rx_errors - if_p->start_rx_errors),
          (uint64_t)(is.rx_dropped - if_p->start_rx_dropped),
          (uint64_t)(is.rx_overruns - if_p->start_rx_overruns),
          (uint64_t)(is.rx_frame - if_p->start_rx_frame),
          (uint64_t)(is.tx_packets - if_p->start_tx_packets),
          (uint64_t)(is.tx_bytes - if_p->start_tx_bytes),
          (uint64_t)(is.tx_errors - if_p->start_tx_errors),
          (uint64_t)(is.tx_dropped - if_p->start_tx_dropped),
          (uint64_t)(is.tx_overruns - if_p->start_tx_overruns),
          (uint64_t)(is.tx_collisions - if_p->start_tx_collisions),
          (uint64_t)(is.tx_carrier),
          (uint64_t)(is.speed / 1000000));



      if_p = if_p->next;
    }

    sigar_close(sigar_p);
    sleep(opts->sample_interval);
  }

}

int
main(int argc, const char *argv[])
{
  char c;
  if_monitor_t* first = NULL;
  char *progname = strdup(argv[0]), *p=progname, *p2;
  int result, l;

  loginfo("%s\n", PACKAGE_STRING);

  /* Get basename */
  p2 = strtok(p, "/");
  while(p2) {
    p = p2;
    p2 = strtok(NULL, "/");
  }
  p2 = p;
  /* The canonical name is `nmetrics-oml2', so it clearly does not start with `om' */
  l = strlen(p);
  if (!strncmp(p, "om", MIN(l,2)) || !strncmp(p, "nmetrics_oml2", MIN(l,13))) {
    logwarn("Binary name `%s' is deprecated and will disappear soon, please use `nmetrics-oml2' instead\n", p);
  }
  free(progname);

  result = omlc_init("nmetrics", &argc, argv, NULL);
  if (result == -1) {
    logerror("Could not initialise OML\n");
    exit (1);
  }

  // parsing command line arguments
  poptContext optCon = poptGetContext(NULL, argc, argv, options, 0);
  while ((c = poptGetNextOpt(optCon)) >= 0) {
    switch (c) {
    case 'i': {
                if_monitor_t* im = (if_monitor_t *)malloc(sizeof(if_monitor_t));
                memset(im, 0, sizeof(if_monitor_t));
                strncpy(im->if_name, g_opts->if_name, 64);
                im->next = first;
                first = im;

                loginfo("Monitoring interface %s\n", g_opts->if_name);
                break;
              }
    }
  }

  oml_register_mps();  // defined in xxx_oml.h
  omlc_start();

  signal (SIGTERM, quit_handler);
  signal (SIGQUIT, quit_handler);
  signal (SIGINT, quit_handler);

  // Do some work
  run(g_opts, g_oml_mps, first);

  omlc_close();

  return(0);
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
