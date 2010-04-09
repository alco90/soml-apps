/*
 *  OML application reporting various node metrics, such as cpu, memory, ...
 *
 * Description:  This application is using the libsigar library to report regularily
 *    through the OML framework on various node metrics, such as cpu load, memory
 *    and network usage.
 *
 * Author: Guillaume Jourjon <guillaume.jourjon@nicta.com.au>, (C) 2008
 * Author: Max Ott  <max.ott@nicta.com.au>, (C) 2009
 *
 * Copyright (c) 2007-2009 National ICT Australia (NICTA)
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
#include "omf_nmetrics_popt.h"

#define OML_FROM_MAIN
#include "omf_nmetrics_oml.h"

#include <ocomm/o_log.h>

OmlMP*   cpu_mp;
OmlMP*   memory_mp;
OmlMP*   net_mp;

#include <sigar.h>

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

static void  run(if_monitor_t* first_if);

int
main(
  int argc,
  const char *argv[]
) {
  char c;
  if_monitor_t* first = NULL;
  if_monitor_t* if_p;

  omlc_init(argv[0], &argc, argv, NULL);

  // parsing command line arguments
  poptContext optCon = poptGetContext(NULL, argc, argv, options, 0);
  while ((c = poptGetNextOpt(optCon)) >= 0) {
    switch (c) {
    case 'i': {
      if_monitor_t* im = (if_monitor_t *)malloc(sizeof(if_monitor_t));
      memset(im, 0, sizeof(if_monitor_t));
      sprintf(im->if_name, "%s", g_opts->if_name);
      im->next = first;
      first = im;

      printf("IF: %s\n", g_opts->if_name);
      break;
    }
    }
  }

  // Initialize measurment points
  if (g_opts->report_cpu) {
    cpu_mp = omlc_add_mp("cpu", oml_cpu_def);
  }
  if (g_opts->report_memory) {
    memory_mp = omlc_add_mp("memory", oml_memory_def);
  }
  if (first) {
    net_mp = omlc_add_mp("net_if", oml_network_def);
  }
  omlc_start();

  run(first);
  return(0);
}

static void
cpu(
  sigar_t* sigar_p,
  OmlMP*   mp_p
) {
  OmlValueU v[9];
  sigar_cpu_t c;

  sigar_cpu_get(sigar_p, &c);

  // NOTE: We cast from a u_64 to a long
  omlc_set_uint64(v[0], c.user);
  omlc_set_uint64(v[1], c.sys);
  omlc_set_uint64(v[2], c.nice);
  omlc_set_uint64(v[3], c.idle);
  omlc_set_uint64(v[4], c.wait);
  omlc_set_uint64(v[5], c.irq);
  omlc_set_uint64(v[6], c.soft_irq);
  omlc_set_uint64(v[7], c.stolen);
  omlc_set_uint64(v[8], c.total);
  omlc_inject(mp_p, v);
}

static void
memory(
  sigar_t* sigar_p,
  OmlMP*   mp_p
) {
  OmlValueU v[6];
  sigar_mem_t m;

  sigar_mem_get(sigar_p, &m);

  // NOTE: We cast from a u_64 to a long
  omlc_set_uint64(v[0], (m.ram ));
  omlc_set_uint64(v[1], (m.total ));
  omlc_set_uint64(v[2],(m.used ));
  omlc_set_uint64(v[3], (m.free ));
  omlc_set_uint64(v[4], (m.actual_used ));
  omlc_set_uint64(v[5], (m.actual_free ));
  omlc_inject(mp_p, v);
}

static void
network_if(
  sigar_t*      sigar_p,
  if_monitor_t* net_if
) {
  OmlValueU v[15];
  sigar_net_interface_stat_t is;

  sigar_net_interface_stat_get(sigar_p, net_if->if_name, &is);
  if (! net_if->not_first) {
    net_if->start_rx_packets = is.rx_packets;
    net_if->start_rx_bytes = is.rx_bytes;
    net_if->start_rx_errors = is.rx_errors;
    net_if->start_rx_dropped = is.rx_dropped;
    net_if->start_rx_overruns = is.rx_overruns;
    net_if->start_rx_frame = is.rx_frame;
    net_if->start_tx_packets = is.tx_packets;
    net_if->start_tx_bytes = is.tx_bytes;
    net_if->start_tx_errors = is.tx_errors;
    net_if->start_tx_dropped = is.tx_dropped;
    net_if->start_tx_overruns = is.tx_overruns;
    net_if->start_tx_collisions = is.tx_collisions;
    net_if->start_tx_carrier = is.tx_carrier;
    net_if->not_first = 1;
  }

  static char name[34] = "foo";

  omlc_set_const_string(v[0], net_if->if_name);
  omlc_set_uint64(v[1], (is.rx_packets - net_if->start_rx_packets));
  omlc_set_uint64(v[2], (is.rx_bytes - net_if->start_rx_bytes));
  omlc_set_uint64(v[3], (is.rx_errors - net_if->start_rx_errors));
  omlc_set_uint64(v[4], (is.rx_dropped - net_if->start_rx_dropped));
  omlc_set_uint64(v[5], (is.rx_overruns - net_if->start_rx_overruns));
  omlc_set_uint64(v[6], (is.rx_frame - net_if->start_rx_frame));
  omlc_set_uint64(v[7], (is.tx_packets - net_if->start_tx_packets));
  omlc_set_uint64(v[8], (is.tx_bytes - net_if->start_tx_bytes));
  omlc_set_uint64(v[9], (is.tx_errors - net_if->start_tx_errors));
  omlc_set_uint64(v[10], (is.tx_dropped - net_if->start_tx_dropped));
  omlc_set_uint64(v[11], (is.tx_overruns - net_if->start_tx_overruns));
  omlc_set_uint64(v[12], (is.tx_collisions - net_if->start_tx_collisions));
  omlc_set_uint64(v[13], (is.tx_carrier)); // - net_if->start_tx_carrier);
  omlc_set_uint64(v[14], (is.speed / 1000000));

  omlc_inject(net_mp, v);
}


static void
run(
  if_monitor_t* first_if
) {
  sigar_t* sigar_p;
  if_monitor_t* if_p;

  while(1) {
    sigar_open(&sigar_p);

    if (g_opts->report_cpu) {
      cpu(sigar_p, cpu_mp);
    }
    if (g_opts->report_memory) {
      memory(sigar_p, memory_mp);
    }

    if_p = first_if;
    while (if_p) {
      network_if(sigar_p, if_p);
      if_p = if_p->next;
    }

    sigar_close(sigar_p);
    sleep(g_opts->sample_interval);
  }
}
