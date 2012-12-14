/*
 * OML application reporting event from wpa_supplicant
 *
 * This application connect to wpa_supplicant's control socket and reports
 * Wi-Fi events.
 *
 * Author: François Hoguet  <françois.hoguet@nicta.com.au>, (C) 2012
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2012
 *
 * Copyright (c) 2012 National ICT Australia (NICTA)
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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/time.h>
#include <oml2/omlc.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define PACKAGE_STRING __FILE__
#endif

#define OML_FROM_MAIN
#include "wpamon_oml.h"
#define USE_OPTS
#include "wpamon_popt.h"

#define SOCK_PATH "/var/run/wpa_supplicant/%s"
#define LOCAL_PATH "/tmp/wpamon_%s.sock"
#define DEFAULT_IF "wlan0"

#ifndef UNIX_PATH_MAX /* Could have been defined from <linux/un.h> */
/* 108 under Linux, but only 104 for OpenBSD */
# define UNIX_PATH_MAX 104
#endif

int exit_loop = 0;

void sigfun(int sig) {
  fprintf(stderr, "INFO\tCaught signal %d, exiting...\n", sig);
  exit_loop = 1;
}

int main(int argc, const char *argv[]) {
  int s;
  int t, len;
  struct sockaddr_un local, remote;
  char local_path[UNIX_PATH_MAX];
  char sock_path[UNIX_PATH_MAX];
  char str[100], temp[100] = "\0";
  char *saveptr, *event, *bssid;
  struct sigaction new_action, old_action;
  struct timeval tv;

  fprintf(stderr, "INFO\t" PACKAGE_STRING "\n");

  /* FIXME: Initialisation should be done by scaffold */
  g_opts_storage.interface = DEFAULT_IF;
  g_opts_storage.ctrl_interface = NULL;
  g_opts_storage.local_path = NULL;

  poptContext optCon = poptGetContext(NULL, argc, argv, options, 0);
  while (poptGetNextOpt(optCon) > 0);

  if (g_opts_storage.local_path) {
    if (g_opts_storage.interface)
      fprintf(stderr, "WARN\t both --interface (%s) and --socket (%s) were specified; using the latter\n",
          g_opts_storage.interface, g_opts_storage.local_path);
    strncpy(local_path, g_opts_storage.local_path, UNIX_PATH_MAX);
  } else {
    snprintf(local_path, sizeof(local_path), LOCAL_PATH, g_opts_storage.interface);
  }
  if (g_opts_storage.ctrl_interface) {
    if (g_opts_storage.interface)
      fprintf(stderr, "WARN\t both --interface (%s) and --ctrl_interface (%s) were specified; using the latter\n",
          g_opts_storage.interface, g_opts_storage.ctrl_interface);
    strncpy(sock_path, g_opts_storage.ctrl_interface, UNIX_PATH_MAX);
  } else {
    snprintf(sock_path, sizeof(local_path), SOCK_PATH, g_opts_storage.interface);
  }

  omlc_init("wpamon", &argc, argv, NULL);
  oml_register_mps();

  if ((s = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1) {
    fprintf(stderr, "ERROR\tCould not create socket: %s\n", strerror(errno));
    exit_loop = 2;
    goto cleanup_socket;
  }

  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, local_path);
  if(bind(s, (struct sockaddr *) &local, sizeof(local)) < 0) {
    fprintf(stderr, "ERROR\tCould not bind socket to '%s': %s\n", local_path, strerror(errno));
    exit_loop = 2;
    goto cleanup_socket;
  }

  remote.sun_family = AF_UNIX;
  strcpy(remote.sun_path, sock_path);
  len = strlen(remote.sun_path) + sizeof(remote.sun_family);
  if (connect(s, (struct sockaddr *)&remote, len) == -1) {
    fprintf(stderr, "ERROR\tCould not connect to wpa_supplicant socket '%s': %s\n", sock_path, strerror(errno));
    exit_loop = 2;
    goto cleanup_socket;
  }

  if(send(s, "ATTACH", 6, 0) < 0) {
    fprintf(stderr, "ERROR\tCould not send 'ATTACH' command to wpa_supplicant: %s\n", strerror(errno));
    exit_loop = 3;
    goto cleanup_socket;
  }
  fprintf(stderr, "INFO\tAttached to wpa_supplicant socket '%s'\n", sock_path);

  new_action.sa_handler = sigfun;
  sigemptyset (&new_action.sa_mask);
  new_action.sa_flags = 0;

  omlc_start();

  /* Replace OML's handling of SIGINT.
   * It's ok as we call omlc_close() ourselves */
  sigaction (SIGINT, NULL, &old_action);
  if (old_action.sa_handler != SIG_IGN)
    sigaction (SIGINT, &new_action, NULL);

  fprintf(stdout, "event,bssid,full_string\n");
  while(!exit_loop) {
    if ( (t=recv(s, str, sizeof(str), 0)) > 0) {
      gettimeofday(&tv, NULL);
      str[t] = '\0';
      if(strcmp(temp, str)) { /* Avoid duplicate messages */
        strcpy(temp, str); /* Prepare to fiddle */
        /* XXX: This is not very robust parsing... */
        strtok_r(temp, "> ", &saveptr);
        event = strtok_r(NULL, " ", &saveptr);
        if(!event) continue;

        if(!strncmp(event, "CTRL-EVENT-", 11)) {
          bssid = NULL;
          if(!strncmp(event+11, "CONNECTED", 9)) {
            /* CTRL-EVENT-CONNECTED - Connection to 00:14:bf:0c:e7:a4 completed (reauth) [id=9 id_str=dd-wrt] */
            bssid = strtok_r(NULL, " ", &saveptr); /* - */
            bssid = strtok_r(NULL, " ", &saveptr); /* Connection */
            bssid = strtok_r(NULL, " ", &saveptr); /* to */
            bssid = strtok_r(NULL, " ", &saveptr);

          } else if(!strncmp(event+11, "DISCONNECTED", 12)) {
            /* CTRL-EVENT-DISCONNECTED bssid=00:14:bf:0c:e7:a4 reason=3 */
            bssid = strtok_r(NULL, "= ", &saveptr); /* bssid */
            bssid = strtok_r(NULL, "= ", &saveptr);

          } else if(!strncmp(event+11, "BSS-ADDED", 9) || !strncmp(event+11, "BSS-REMOVED", 11) ) {
            /* CTRL-EVENT-BSS-ADDED 219 00:22:55:ee:b2:91
             * CTRL-EVENT-BSS-REMOVED 212 00:60:64:6c:82:f6 */
            bssid = strtok_r(NULL, " ", &saveptr); /* entry id */
            bssid = strtok_r(NULL, " ", &saveptr);
          } else if(!strncmp(event+11, "SCAN-RESULTS", 12)) {
            /* Skip this one */
            event = NULL;
          }

          if(event) {
            if(!bssid) bssid = "";
            fprintf(stdout, "%s,%s,%s\n", event, bssid, str+3);
            oml_inject_network_event(g_oml_mps_wpamon->network_event, event, bssid, str, tv.tv_sec, tv.tv_usec);
          }
        }
        strcpy(temp, str); /* Prepare to avoid duplicate messages */
      }
    } else if (t < 0) {
      fprintf(stderr, "ERROR\tCould not receive anything from wpa_supplicant: %s\n", strerror(errno));
      exit_loop = 4;
    } else {
      fprintf(stderr, "INFO\tServer closed connection\n");
      exit_loop = 1;
    }
  }

  omlc_close();

cleanup_socket:
  close(s);
  if(remove(local_path) == -1) {
    fprintf(stderr, "ERROR\tCould not connect remove socket '%s': %s\n", local_path, strerror(errno));
  }
  exit(exit_loop - 1);
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
