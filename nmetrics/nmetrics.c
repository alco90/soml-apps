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
 * Copyright (c) 2007-2012 National ICT Australia (NICTA)
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


#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define USE_OPTS
#include "nmetrics_popt.h"

#define OML_FROM_MAIN
#include "nmetrics_oml.h"

#include <sigar.h>

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



void
run(
  opts_t* opts,
  oml_mps_t* oml_mps,
  if_monitor_t* first_if
) {
  long val = 1;
  sigar_t* sigar_p;
  if_monitor_t* if_p;
  sigar_cpu_t c;
  sigar_mem_t m;
  sigar_net_interface_stat_t is;
  while(1) {
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
main(
  int argc,
  const char *argv[]
) {
  char c;
  if_monitor_t* first = NULL;
  if_monitor_t* if_p;
  char *progname = strdup(argv[0]), *p=progname, *p2;
  int result, l;

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
	  fprintf(stderr,
              "warning: binary name `%s' is deprecated and will disappear with OML 2.9.0, please use `nmetrics-oml2' instead\n", p);
  }
  free(progname);

  omlc_init("nmetrics", &argc, argv, NULL);
  if (result == -1) {
    fprintf (stderr, "error: could not initialise OML\n");
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

      printf("IF: %s\n", g_opts->if_name);
      break;
    }
    }
  }

  oml_register_mps();  // defined in xxx_oml.h
  omlc_start();

// Do some work
  run(g_opts, g_oml_mps, first);

  return(0);
}