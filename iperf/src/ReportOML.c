/*
 * Iperf reporter using the OML library.
 * Copyright (c) 2010, Nicta, Olivier Mehani <olivier.mehani@nicta.com.au>
 * All rights reserved.
 *
 * $Id: bsdnotice.ab.c 371 2008-05-30 03:45:32Z shtrom $
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
#include <oml2/omlc.h>
#include "headers.h"
#include "Settings.hpp"
#include "SocketAddr.h"
#include "Reporter.h"
#include "report_default.h"
#include "report_OML.h"

OmlMP* tcp_measure;
OmlMP* udp_measure;
OmlMP* peer_information;

static OmlMPDef peer_info[] = {
	{"ID", OML_INT32_VALUE},
	{"local_address", OML_STRING_VALUE},
	{"local_port", OML_INT32_VALUE},
	{"foreign_address", OML_STRING_VALUE},
	{"foreign_port", OML_INT32_VALUE},
	{NULL, (OmlValueT)0}
};

static OmlMPDef tcp_receiverport[] = {
	{"ID", OML_INT32_VALUE},
	{"Begin_interval", OML_DOUBLE_VALUE},
	{"End_interval", OML_DOUBLE_VALUE},
	{"Transfer", OML_DOUBLE_VALUE},
	{"Bandwidth", OML_DOUBLE_VALUE},
	{NULL, (OmlValueT)0}

};

static OmlMPDef udp_receiverport[] = {
	{"ID", OML_INT32_VALUE},
	{"Begin_interval", OML_DOUBLE_VALUE},
	{"Transfer", OML_DOUBLE_VALUE},
	{"Bandwidth", OML_DOUBLE_VALUE},
	{"Jitter", OML_DOUBLE_VALUE},
	{"Packet_Lost", OML_INT32_VALUE},
	{"Total_Packet", OML_INT32_VALUE},
	{"PLR", OML_DOUBLE_VALUE},
	{NULL, (OmlValueT)0}
};

int OML_init(int *argc, const char **argv) {
	return omlc_init("iperf", argc,  argv, NULL);
}

int OML_set_measurepoints(thread_Settings *mSettings) {
	peer_information = omlc_add_mp("Peer_Info", peer_info);
	if (isUDP(mSettings)) {
		if(mSettings->mThreadMode == kMode_Client)
			tcp_measure = omlc_add_mp("UDP_Periodic_Info", tcp_receiverport);
		udp_measure = omlc_add_mp("UDP_Rich_Info", udp_receiverport);
	} else { /* TCP */
		tcp_measure = omlc_add_mp("TCP_Info", tcp_receiverport);
	}

	return omlc_start();
}

void *OML_peer(Connection_Info *stats, int ID) {
	OmlValueU v[5];
	/*
	 * It is somewhat better to store address strings statically in this
	 * function rather than somewhere unknown in the libc. This will have
	 * to do until omlc_set_const_string() makes copies or the entire
	 * matter is handled differently.
	 */
	static char local_addr[REPORT_ADDRLEN], peer_addr[REPORT_ADDRLEN];

	omlc_set_int32(v[0], ID);

	/* YUCK! (cf. include/headers.h) */
	inet_ntop(
#ifdef HAVE_IPV6
			((struct sockaddr_storage) stats->local).ss_family,
			SockAddr_isIPv6(&stats->local)?
				(void *)SockAddr_get_in6_addr(&stats->local):
#else
			((struct sockaddr_in) stats->local).sin_family,
#endif
			(void *)SockAddr_get_in_addr(&stats->local),
			local_addr, REPORT_ADDRLEN);
	omlc_set_const_string(v[1], local_addr);
	omlc_set_int32(v[4], SockAddr_getPort(&stats->local));

	inet_ntop(
#ifdef HAVE_IPV6
			((struct sockaddr_storage) stats->peer).ss_family,
			SockAddr_isIPv6(&stats->peer)?
				(void *)SockAddr_get_in6_addr(&stats->peer):
#else
			((struct sockaddr_in) stats->peer).sin_family,
#endif
				(void *)SockAddr_get_in_addr(&stats->peer),
			peer_addr, REPORT_ADDRLEN);
	omlc_set_const_string(v[3], peer_addr);
	omlc_set_int32(v[4], SockAddr_getPort(&stats->peer));

	omlc_inject(peer_information, v);

	/* Give the OML user some feedback*/
	return reporter_reportpeer(stats, ID);}

static inline void _fill_common_values(Transfer_Info *stats, OmlValueU v[]) {
	double speed;

	omlc_set_int32(v[0], stats->transferID);
	omlc_set_double(v[1], (double) stats->startTime);
	omlc_set_double(v[2], (double) stats->endTime);
	omlc_set_double(v[3], (double) stats->TotalLen * 8.0);

	speed = (((double)stats->TotalLen * 8.0) / (stats->endTime - stats->startTime));
	omlc_set_double(v[4], speed);
}

void OML_stats(Transfer_Info *stats) {
	OmlValueU v[5];

	_fill_common_values(stats, v);

	omlc_inject(tcp_measure, v);
}

void OML_serverstats(Connection_Info *conn, Transfer_Info *stats) {
	OmlValueU v[9];

	_fill_common_values(stats, v);

	omlc_set_double(v[5], stats->jitter * 1000.0);
	omlc_set_double(v[6], stats->cntError);
	omlc_set_double(v[7], stats->cntDatagrams);
	omlc_set_double(v[8], (100.0 * stats->cntError) / stats->cntDatagrams);

	omlc_inject(udp_measure, v);
}
