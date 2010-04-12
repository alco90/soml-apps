/*
 *  C Implementation: wlanconfig_oml2
 *
 * Description:  An application to take measurements from wlanconfig using
 *               OML.
 *
 * Author: Guillaume Jourjon <guillaume.jourjon@nicta.com.au>, (C) 2008
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <pcap.h>
#include <errno.h>
#include <unistd.h>
#include <ocomm/o_log.h>
#include <oml2/omlc.h>

static OmlMPDef oml_def[] = {
  {"macAddress", OML_STRING_VALUE},
  {"RSSI", OML_LONG_VALUE},
  {"DBM", OML_LONG_VALUE},
  {"myMacAddress", OML_STRING_VALUE},
  {NULL, (OmlValueT)0},
};
static OmlMP* oml_mp = NULL;

int
main(int argc, const char *argv[])
{
  int i = 0;
  int j = 0;
  int stop = 1;
  OmlValueU v[4];
  pid_t pid;
  int mypipe[2];
  char command[256];
  char command2[256] = "256";
  char macAddress[256] = "e";
  int* argc_;
  const char** argv_;
  char* ifwifi;
  omlc_init(argv[ 0 ], &argc, argv, o_log);
  argc_ = &argc;
  argv_ = argv;

  ifwifi = "ath0";
  if (pipe (mypipe)) {
    fprintf (stderr, "Pipe failed.\n");
    return EXIT_FAILURE;
  }
  pid = fork ();
  if (pid == (pid_t) 0) {
    close (mypipe[0]);
    close(1);
    dup(mypipe[1]);
    if (execl("/sbin/ifconfig", "ifconfig", "ath0", NULL) < 0) {
      printf("execl failed\n");
      exit(1);
    }
  } else if (pid < (pid_t) 0) {
    fprintf (stderr, "Fork failed.\n");
    return EXIT_FAILURE;
  } else {
    close(mypipe[1]);
    close(0);
    dup(mypipe[0]);
    scanf ("%s",command);
    scanf ("%s",command);
    scanf ("%s",command);
    scanf ("%s",command);
    scanf ("%s",command);
    strcpy(macAddress, command);
    close(mypipe[0]);
    while(strcmp(command,command2) != 0) {
      strcpy(command2, command);
      scanf ("%s",command);
    }
  }

  oml_mp = omlc_add_mp("wifi_info", oml_def);

  omlc_start();
  printf("mac address %s \n",macAddress);

  while(1) {
    i = 0;
    stop = 1;
    if (pipe (mypipe)) {
      fprintf (stderr, "Pipe failed.\n");
      return EXIT_FAILURE;
    }
    pid = fork ();
    if (pid == (pid_t) 0) {
      close (mypipe[0]);
      close(1);
      dup(mypipe[1]);
      if (execl("/sbin/wlanconfig", "wlanconfig", "ath0", "list", NULL) < 0) {
        printf("execl failed\n");
        exit(1);
      }
    } else if (pid < (pid_t) 0) {
      fprintf (stderr, "Fork failed.\n");
      return EXIT_FAILURE;
    } else {
      close(mypipe[1]);
      close(0);
      dup(mypipe[0]);
      scanf ("%s",command);

      while(stop) {
        strcpy(command2, command);

        if(strcmp(command,"ADDR") != 0) {
          omlc_set_const_string(v[0], command2);
          scanf ("%s",command);
          scanf ("%s",command);
          scanf ("%s",command);
          scanf ("%s",command);

          v[1].longValue = atol(command);
          scanf ("%s",command);

          v[2].longValue = atol(command);
          omlc_set_const_string(v[3], macAddress);

          omlc_inject(oml_mp, v);
          //stop=0;
          for(j = 0; j < 9; j++) {
            scanf ("%s",command);
            strcpy(command2, command);
          }
          while(command[0] != '0' && command[2] != ':') {
            printf("result wlanconfig 11: %s \n ", command);
            scanf ("%s",command);
            if(strcmp(command,command2) == 0) {
              stop = 0;
              break;
            }
            strcpy(command2, command);
          }
        } else {
          for(i = 0; i < 16; i++) {
            scanf ("%s",command);
          }
        }
      }
    }
    sleep(1);
  }

  return 0;
}
