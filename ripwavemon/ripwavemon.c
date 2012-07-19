/*
 * ripwave-monitor, an OML-enabled application to collect statistics from the
 * Navini (now Cisco) Ripwave pre-WiMAX modem used by, e.g. Unwired, Exetel or
 * nTelos.
 *
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2010--2012
 *
 * Copyright (c) 2010-2012 National ICT Australia (NICTA)
 *
 * This program is based on documentation available at [0], which is dumped
 * in file navini-protocol.txt for convenience.
 * [0] http://www.hamilton.ie/gavinmc/ripwave/navini_diag.html
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of Nicta nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdint.h>
#include <netinet/ip.h>
#define OML_FROM_MAIN
#include "ripwavemon_oml.h"
//#include "EPEventDef.h"

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

int prepare_socket(void)
{
    int sockfd;
    struct sockaddr_in sai_bind;

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("error: socket()");
        return -1;
    }

    memset(&sai_bind, 0, sizeof(struct sockaddr_in));
    sai_bind.sin_family = AF_INET;
    sai_bind.sin_addr.s_addr = INADDR_ANY;
    sai_bind.sin_port = htons(NAVINI_REPORT_PORT);

    if (bind(sockfd, (struct sockaddr*) &sai_bind, sizeof(sai_bind)) != 0) {
        perror("error: bind()");
        return -1;
    }

    return sockfd;
}

prepare_oml(int argc, const char **argv)
{
    if (omlc_init("ripwavemon", &argc, argv, NULL) != 0) {
        fprintf(stderr, "error: could not initialise OML\n");
        return -1;
    }
    oml_register_mps();
    if (omlc_start() != 0) {
        fprintf(stderr, "error: could not start OML\n");
        return -1;
    }

    return 0;
}

void receive_reports (int sockfd)
{
    navini_report_t report;
    ssize_t len;
    unsigned int *p;
    OmlValueU values[14];

    printf("# Modem ID\tMM/DD/YY HH:MM:SS\tSyncStr\tBTS ID\tNet ID\tAntenna\tSyncStr\tSNR\tTemperature\n");

    while (1) {
        len = recv(sockfd, (void*) &report, sizeof(report), MSG_WAITALL);
        if (len != sizeof(report)) {
            fprintf(stderr, "warning: received packet of unexpected size (%d instead of %d)\n",
                    (int)len, (int)sizeof(report));
            fprintf(stderr, "info: first 32 bytes are");
            for(p=(unsigned int *)&report; p++;
                (p-(unsigned int *)&report)<4)
              fprintf(stderr, " %08x", *p);
            fprintf(stderr, "\n");
            continue;
        }

        printf("%#x\t%02u/%02u/%02u %02u:%02u:%02u\t%d\t%#x\t%#x\t%#x %#x\t%d\t%d\t%d\n",
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
                report.antpad,
                -report.syncstrength2,
                report.snr,
                report.temperature
        );

        omlc_set_uint32(values[0], ntohl(report.modemid));
        omlc_set_uint32(values[1], report.month);
        omlc_set_uint32(values[2], report.day);
        omlc_set_uint32(values[3], report.year);
        omlc_set_uint32(values[4], report.hour);
        omlc_set_uint32(values[5], report.minute);
        omlc_set_uint32(values[6], report.second);
        omlc_set_int32(values[7], -report.syncstrength1);
        omlc_set_uint32(values[8], report.btsid);
        omlc_set_uint32(values[9], ntohs(report.networkid));
        omlc_set_uint32(values[10], report.antenna);
        omlc_set_int32(values[11], -report.syncstrength2);
        omlc_set_uint32(values[12], report.snr);
        omlc_set_uint32(values[13], report.temperature);
        omlc_inject(g_oml_mps->ripwave_stats, values);

    }
}

int main(int argc, const char **argv)
{
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
