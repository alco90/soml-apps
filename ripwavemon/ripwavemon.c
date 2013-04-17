/*
 * ripwave-monitor, an OML-enabled application to collect statistics from the
 * Navini (now Cisco) Ripwave pre-WiMAX modem used by, e.g. Unwired, Exetel or
 * nTelos.
 *
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2010-2013
 *
 * Copyright 2010-2013 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <netinet/ip.h>
#include <errno.h>
#include <ocomm/o_log.h>
#include <oml2/omlc.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define PACKAGE_STRING __FILE__
#endif

#define OML_FROM_MAIN
#include "ripwavemon_oml.h"

#define NAVINI_REPORT_PORT  3859

/* This aligns the structure's fields to 1 byte so they can be mapped directly
 * on the received packets. */
#pragma pack(1)
typedef struct {            /* Offset from the start of the IP packet*/
  int8_t  pad1[0x12];     /* 0x1c, start of UDP data */
  uint32_t    modemid;    /* 0x2e */
  uint8_t     month;      /* 0x32 */
  uint8_t     day;        /* 0x33 */
  uint8_t     year;       /* 0x34 */
  uint8_t     hour;       /* 0x35 */
  uint8_t     minute;     /* 0x36 */
  uint8_t     second;     /* 0x37 */
  int8_t  pad2[0x11];     /* 0x38 */
  uint8_t     syncstrength1;  /* 0x49 */
  int8_t  pad3[0x0f];     /* 0x4a */
  uint8_t     btsid;      /* 0x59 */
  int8_t  pad4[0x04];     /* 0x5a */
  uint16_t    networkid;  /* 0x5e */
  int8_t  pad5[0x1a];     /* 0x60 */
  uint8_t     antpad:4,   /* MACState? Varies just after powerup */
              antenna:4;  /* 0x7a */
  int8_t  pad6[0x06];     /* 0x7b */
  uint8_t     syncstrength2;  /* 0x81, the doc incorrectly states 0x80 */
  int8_t  pad7[0x0c];     /* 0x82 */
  uint8_t     snr;        /* 0x8e */
  int8_t  pad8[0x1a];     /* 0x8f */
  uint8_t     temperature;    /* 0xa9 */
  int8_t  pad9[0xa7];     /* 0xaa; pad up to 336B (0x150) */
} navini_report_t;
#pragma pack()

int
prepare_socket(void)
{
  int sockfd;
  struct sockaddr_in sai_bind;

  if ((sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    logerror("Could not open socket: %s\n", strerror(errno));
    return -1;
  }

  memset(&sai_bind, 0, sizeof(struct sockaddr_in));
  sai_bind.sin_family = AF_INET;
  sai_bind.sin_addr.s_addr = INADDR_ANY;
  sai_bind.sin_port = htons(NAVINI_REPORT_PORT);

  if (bind(sockfd, (struct sockaddr*) &sai_bind, sizeof(sai_bind)) != 0) {
    logerror("Could not bind socket: %s\n", strerror(errno));
    return -1;
  }

  return sockfd;
}

int
prepare_oml(int argc, const char **argv)
{
  if (omlc_init("ripwavemon", &argc, argv, NULL) != 0) {
    logerror("Could not initialise OML\n");
    return -1;
  }
  oml_register_mps();
  if (omlc_start() != 0) {
    logerror("Could not start OML\n");
    return -1;
  }

  return 0;
}

void
receive_reports (int sockfd)
{
  navini_report_t report;
  ssize_t len;
  unsigned int *p;

  printf("Modem_ID,date,time,SyncStr,BTS_ID,Net_ID,Antenna,SyncStr,SNR,Temperature\n");

  while (1) {
    len = recv(sockfd, (void*) &report, sizeof(report), MSG_WAITALL);
    if (len != sizeof(report)) {
      logwarn("Received packet of unexpected size (%d instead of %d); first 32 bytes:",
          (int)len, (int)sizeof(report));
      for((p=(unsigned int *)&report); (p-(unsigned int *)&report)<4; p++)
        fprintf(stderr, " %08x", *p);
      fprintf(stderr, "\n");
      continue;
    }

    printf("%#x,%02u/%02u/%02u,%02u:%02u:%02u,%d,%#x,%#x,%#x,%d,%d,%d\n",
        ntohl(report.modemid),
        report.month,
        report.day,
        report.year,
        report.hour,
        report.minute,
        report.second,
        -report.syncstrength1,
        report.btsid,
        ntohs(report.networkid),
        report.antenna,
        -report.syncstrength2,
        report.snr,
        report.temperature
        );

    oml_inject_ripwave_stats(g_oml_mps->ripwave_stats,
        ntohl(report.modemid),
        report.month,
        report.day,
        report.year,
        report.hour,
        report.minute,
        report.second,
        -report.syncstrength1,
        report.btsid,
        ntohs(report.networkid),
        report.antenna,
        -report.syncstrength2,
        report.snr,
        report.temperature);
  }
}

int
main(int argc, const char **argv)
{

  loginfo("%s\n", PACKAGE_STRING);

  int sockfd;

  if ((sockfd = prepare_socket())<0)
    exit(1);

  if (prepare_oml(argc, argv) != 0)
    exit(2);

  receive_reports(sockfd);

  /* Unlikely to be reached... */
  omlc_close();

  return 0;
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
