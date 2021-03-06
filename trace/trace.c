/*
 * OML application reporting various captured packets' characteristics
 *
 * This application is using the libtrace library to capture packets matching a
 * given filter and report selected header files through OML
 *
 * Author: Guillaume Jourjon <guillaume.jourjon@nicta.com.au>, (C) 2009
 * Author: Max Ott  <max.ott@nicta.com.au>, (C) 2009
 * Author: Jolyon White  <jolyon.white@nicta.com.au>, (C) 2010
 * Author: Olivier Mehani  <olivier.mehani@nicta.com.au>, (C) 2010--2014
 * Author: François Hoguet  <françois.hoguet@nicta.com.au>, (C) 2012
 *
 * Copyright 2007-2014 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */
#include <libtrace.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#include <ocomm/o_log.h>
#include <oml2/omlc.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#else
# define PACKAGE_STRING __FILE__ " " __DATE__
# define PACKAGE_NAME __FILE__
# define PACKAGE_VERSION __DATE__
#endif

#define OML_FROM_MAIN
#include "trace_oml.h"
#define USE_OPTS
#include "trace_popt.h"

#ifndef MIN
/* MIN is defined in sys/param.h, which is include by recent libtrace.h */
# define MIN(x,y) ((x)<(y)?(x):(y))
#endif

static int stop_loop = 0;

static void
quit_handler (int signum)
{
    stop_loop = 1;
}

static void
trace_oml_inject_ip(oml_mps_t* oml_mps, libtrace_ip_t* ip, libtrace_packet_t *packet,
               double time_now, oml_guid_t pktid)
{
    //OmlValueU v[12];
    char buf_addr_src[INET_ADDRSTRLEN];
    char buf_addr_dst[INET_ADDRSTRLEN];
    char addr_src[INET_ADDRSTRLEN];
    char addr_dst[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(ip->ip_src), buf_addr_src, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip->ip_dst), buf_addr_dst, INET_ADDRSTRLEN);
    strcpy(addr_src,buf_addr_src);
    strcpy(addr_dst,buf_addr_dst);

    oml_inject_ip(oml_mps->ip,
        pktid,
        ip->ip_tos,
        ntohs(ip->ip_len),
        ntohs(ip->ip_id),
        ntohs(ip->ip_off & ~(7<<13)),
        ip->ip_ttl,
        ip->ip_p,
        ntohs(ip->ip_sum),
        addr_src,
        addr_dst,
        trace_get_capture_length(packet),
        time_now);
}

static void
trace_oml_inject_tcp(oml_mps_t* oml_mps, libtrace_tcp_t* tcp, libtrace_packet_t *packet,
                void* payload, double time_now, oml_guid_t pktid)
{
    (void) payload;
    oml_inject_tcp(oml_mps->tcp,
        pktid,
        trace_get_source_port(packet),
        trace_get_destination_port(packet),
        ntohs(tcp->seq),
        ntohs(tcp->ack_seq),
        ntohs(tcp->window),
        ntohs(tcp->check),
        ntohs(tcp->urg_ptr),
        trace_get_capture_length(packet),
        time_now);
}

static void
trace_oml_inject_udp(oml_mps_t* oml_mps, libtrace_udp_t* udp, libtrace_packet_t *packet,
                void* payload, double time_now, oml_guid_t pktid)
{
    (void) payload;
    oml_inject_udp(oml_mps->udp,
        pktid,
        trace_get_source_port(packet),
        trace_get_destination_port(packet),
        ntohs(udp->len),
        ntohs(udp->check),
        time_now);
}

static void
trace_oml_inject_ip6(oml_mps_t* oml_mps, libtrace_ip6_t* ip6,
               double time_now, oml_guid_t pktid)
{
    char buf_addr_src[INET6_ADDRSTRLEN];
    char buf_addr_dst[INET6_ADDRSTRLEN];
    char addr_src[INET6_ADDRSTRLEN];
    char addr_dst[INET6_ADDRSTRLEN];

    inet_ntop(AF_INET6, &(ip6->ip_src), buf_addr_src, INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6, &(ip6->ip_dst), buf_addr_dst, INET6_ADDRSTRLEN);
    strcpy(addr_src,buf_addr_src);
    strcpy(addr_dst,buf_addr_dst);

    oml_inject_ip6(oml_mps->ip6,
        pktid,
        addr_src,
        addr_dst,
        time_now);
}

static void
trace_oml_inject_ip6_mh(oml_mps_t* oml_mps, char* type, oml_guid_t pktid,
    uint16_t sequ_nb, struct timeval tv, char* data)
{
    oml_inject_ip6_mh(oml_mps->ip6_mh,
        pktid,
        type,
        sequ_nb,
        tv.tv_sec,
        tv.tv_usec,
        data);
}

/* Defining some header constants that libtrace doesn't yet */
#define TRACE_OML2_IPPROTO_MOBIL 0x87
#define TRACE_OML2_IPPROTO_MOBIL_BU 0x05
#define TRACE_OML2_IPPROTO_MOBIL_BACK 0x06

typedef struct trace_oml2_ip6_mh {
  /* from include/netinet/ip6mh.h in UMIP (commit 9236e61) */
  uint8_t         ip6mh_proto;    /* NO_NXTHDR by default */
  uint8_t         ip6mh_hdrlen;   /* Header Len in unit of 8 Octets
                                     excluding the first 8 Octets */
  uint8_t         ip6mh_type;     /* Type of Mobility Header */
  uint8_t         ip6mh_reserved; /* Reserved */
  uint16_t        ip6mh_cksum;    /* Mobility Header Checksum */
  /* Followed by type specific messages */
} __attribute__ ((packed)) trace_oml2_ip6_mh_t;

typedef struct trace_oml2_ip6_mh_bu {
  /* from include/netinet/ip6mh.h in UMIP (commit 9236e61) */
  trace_oml2_ip6_mh_t ip6mhbu_hdr;
  uint16_t        ip6mhbu_seqno;          /* Sequence Number */
  uint16_t        ip6mhbu_flags;
  uint16_t        ip6mhbu_lifetime;       /* Time in unit of 4 sec */
  /* Followed by optional Mobility Options */
} __attribute__ ((packed)) trace_oml2_ip6_mh_bu_t;

typedef struct trace_oml2_ip6_mh_back {
  /* from include/netinet/ip6mh.h in UMIP (commit 9236e61) */
  trace_oml2_ip6_mh_t ip6mhba_hdr;
  uint8_t         ip6mhba_status; /* Status code */
  uint8_t         ip6mhba_flags;
  uint16_t        ip6mhba_seqno;
  uint16_t        ip6mhba_lifetime;
  /* Followed by optional Mobility Options */
} __attribute__ ((packed)) trace_oml2_ip6_mh_back_t;


static void
trace_oml_parse_ip6_hdrs(oml_mps_t* oml_mps, libtrace_ip6_t *ip6, double now, struct timeval tv, oml_guid_t pktid)
{
  uint8_t               hdr_type; // determine type of mobility header
  uint8_t               hdr_len;  // length of header in decimal
  libtrace_ip6_ext_t*   hdr;      // point at the beginning of the header
  uint8_t               hdr_next; // code of next header

  trace_oml2_ip6_mh_t*  mh;     // mobility header
  trace_oml2_ip6_mh_bu_t*   bu; // binding update message in mobility header
  trace_oml2_ip6_mh_back_t* ba; // binding acknowledgement message in mobility header
  uint16_t              mh_sequ_nb; // sequence nb of mobility header
  int resume = 0;

  /* PARSING IPV6 HEADERS TO FIND SPECIFIC MOBILITY HEADERS
   *
   * possible headers:  0 = 0x00 = STEP   = TRACE_IPPROTO_IP         4 = 0x04 = IPv4     = TRACE_IPPROTO_IPIP
   *                   43 = 0x2b = ROUT   = TRACE_IPPROTO_ROUTING    6 = 0x06 = TCP      = TRACE_IPPROTO_TCP
   *                   44 = 0x2c = FRAG   = TRACE_IPPROTO_FRAGMENT  17 = 0x11 = UDP      = TRACE_IPPROTO_UDP
   *                   50 = 0x32 = PRIV   = TRACE_IPPROTO_ESP       41 = 0x29 = IPv6     = TRACE_IPPROTO_IPV6
   *                   51 = 0x33 = AUTH   = TRACE_IPPROTO_AH        58 = 0x3a = ICMP6    = TRACE_IPPROTO_ICMPV6
   *                   59 = 0x3b = NOMORE = TRACE_IPPROTO_NONE     132 = 0x84 = SCTP     = TRACE_IPPROTO_SCTP
   *                   60 = 0x3c = DSTOPT = TRACE_IPPROTO_DSTOPTS  136 = 0x88 = UDP lite
   *                  135 = 0x87 = MOBIL  = TRACE_OML2_IPPROTO_MOBIL
   *                  140 = 0x8c = SHIM6
   * mobility header 135 -> 3rd octet values:
   *                      5=0x05=BU       = TRACE_OML2_IPPROTO_MOBIL_BU
   *                      6=0x06=BAck     = TRACE_OML2_IPPROTO_MOBIL_BACK
   */
  hdr = (libtrace_ip6_ext_t*) ip6; /* XXX: not really a libtrace_ip6_ext_t,
                                      but this pacifies the compiler; it's
                                      fine as long as the loop below advances
                                      the pointer to the real option before
                                      accessing any field */
  hdr_next = ip6->nxt;
  hdr_len = sizeof(ip6);

  while(hdr_next != TRACE_IPPROTO_NONE && !resume) {

    /* Advance hdr to the beginning of header of type hdr_next,
     * and update its length */
    hdr = (libtrace_ip6_ext_t*)((uint8_t*)hdr + hdr_len);
    hdr_len = hdr->len*8 + 8; /* RFC 2460: Hdr Ext Len is in 8-octet units,
                                 not including the first 8 octets*/

    switch(hdr_next) { /* add your case to treat other headers */
    case TRACE_IPPROTO_IPV6:
      ip6 = (libtrace_ip6_t*)hdr;
      trace_oml_inject_ip6(oml_mps, ip6, now, pktid);
      trace_oml_parse_ip6_hdrs(oml_mps, ip6, now, tv, pktid);
      return; /* XXX: assume this was the payload */
      break;

    case TRACE_IPPROTO_ROUTING: /* routing header in BAck */
      /* XXX: Do something here */
      break;

    case TRACE_IPPROTO_DSTOPTS: /* destination option in BU */
      /* XXX: Do something here */
      break;

    case TRACE_OML2_IPPROTO_MOBIL: { /* mobility header */
                 char* mhtype;
                 char  addr_coa[INET6_ADDRSTRLEN];

                 mh = (trace_oml2_ip6_mh_t*)hdr;
                 hdr_type = mh->ip6mh_type;

                 switch(hdr_type) {

                 case TRACE_OML2_IPPROTO_MOBIL_BU: /* BU */
                   mhtype="BU";
                   bu = (trace_oml2_ip6_mh_bu_t*)hdr;
                   mh_sequ_nb = htons(bu->ip6mhbu_seqno);
                   inet_ntop(AF_INET6, (struct in6_addr*)(hdr + 16), addr_coa, INET6_ADDRSTRLEN);
                   break;

                 case TRACE_OML2_IPPROTO_MOBIL_BACK: /* BAck */
                   mhtype="BAck";
                   ba = (trace_oml2_ip6_mh_back_t*)hdr;
                   mh_sequ_nb = htons(ba->ip6mhba_seqno);
                   addr_coa[0] = '\0';
                   break;
                 }

                 trace_oml_inject_ip6_mh(oml_mps, mhtype, pktid, mh_sequ_nb, tv, addr_coa);
                 break;
               }

    default:
               resume = 1; /* Not a header we know, resume normal processing */
               break;
    }
    hdr_next = hdr->nxt;
  }
}

static void
trace_oml_inject_icmp(oml_mps_t* oml_mps, oml_guid_t pktid, uint8_t type,
                  uint16_t sequ_nb, struct timeval tv, int ipver)
{
  /* FIXME: this is probably a useless level of indirection */
  switch(ipver) {
  case 4:
    oml_inject_icmp(oml_mps->icmp,
        pktid,
        type,
        ntohs(sequ_nb),
        tv.tv_sec,
        tv.tv_usec);
    break;
  case 6:
    oml_inject_icmp6(oml_mps->icmp6,
        pktid,
        type,
        ntohs(sequ_nb),
        tv.tv_sec,
        tv.tv_usec);
    break;
  }
}

void
mac_to_s (uint8_t *mac, char *s, int n)
{
  if (mac == NULL) {
    strncpy (s, "address NULL", n);
  } else {
    snprintf (s, n, "%02x:%02x:%02x:%02x:%02x:%02x",
              mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  }
}

#define MAC_OCTETS 6
#define MAC_STRING_LENGTH (3*MAC_OCTETS)

static void
trace_oml_inject_ethernet(oml_mps_t* oml_mps, libtrace_ethertype_t ethertype,
                     void* linkptr, libtrace_packet_t* packet, oml_guid_t pktid)
{
  uint8_t *mac_source;
  uint8_t *mac_dst;
  char macS[MAC_STRING_LENGTH];
  char macD[MAC_STRING_LENGTH];
  mac_source = trace_get_source_mac(packet);
  mac_dst = trace_get_destination_mac(packet);
  mac_to_s (mac_source, macS, sizeof(macS) / sizeof(macS[0]));
  mac_to_s (mac_dst, macD, sizeof(macD) / sizeof(macD[0]));

  oml_inject_ethernet(oml_mps->ethernet,
      pktid,
      macS,
      macD,
      ethertype);
}
static void
trace_oml_inject_radiotap(oml_mps_t* oml_mps, libtrace_linktype_t linktype,
                     void* linkptr, libtrace_packet_t* packet, oml_guid_t pktid)
{
  uint64_t tsft;
  uint8_t rate;
  uint16_t freq;
  int8_t s_strength;
  int8_t n_strength;
  uint8_t s_db_strength;
  uint8_t n_db_strength;
  uint16_t attenuation;
  uint16_t attenuation_db;
  int8_t txpower;
  uint8_t antenna;
  uint8_t *mac_source;
  uint8_t *mac_dst;
  char macS[MAC_STRING_LENGTH];
  char macD[MAC_STRING_LENGTH];
  trace_get_wireless_tsft (linkptr,linktype, &tsft);
  trace_get_wireless_rate (linkptr, linktype, &rate);
  trace_get_wireless_freq (linkptr, linktype, &freq);
  trace_get_wireless_signal_strength_dbm (linkptr, linktype, &s_strength);
  trace_get_wireless_noise_strength_dbm (linkptr, linktype, &n_strength);
  trace_get_wireless_signal_strength_db (linkptr, linktype, &s_db_strength);
  trace_get_wireless_noise_strength_db (linkptr, linktype, &n_db_strength);
  trace_get_wireless_tx_attenuation (linkptr, linktype, &attenuation);
  trace_get_wireless_tx_attenuation_db (linkptr, linktype, &attenuation_db);
  trace_get_wireless_tx_power_dbm (linkptr, linktype, &txpower);
  trace_get_wireless_antenna (linkptr, linktype, &antenna);
  mac_source = trace_get_source_mac(packet);
  mac_dst = trace_get_destination_mac(packet);
  mac_to_s (mac_source, macS, sizeof(macS) / sizeof(macS[0]));
  mac_to_s (mac_dst, macD, sizeof(macD) / sizeof(macD[0]));

  oml_inject_radiotap(oml_mps->radiotap,
      pktid,
      tsft,
      rate,
      freq,
      s_strength,
      n_strength,
      s_db_strength,
      n_db_strength,
      attenuation,
      attenuation_db,
      txpower,
      antenna,
      macS,
      macD);
}

static void
per_packet(oml_mps_t* oml_mps, libtrace_packet_t* packet, long start_time, oml_guid_t pktid)
{
  uint32_t              remaining;
  void*                 l3;
  uint16_t              ethertype;
  void*                 transport;
  uint8_t               proto;
  void*                 payload;
  void*                 linkptr;
  libtrace_linktype_t   linktype;

  //  size_t size_of_packet = 0;
  struct timeval tv = trace_get_timeval(packet);
  double now = tv.tv_sec - start_time + 0.000001 * tv.tv_usec;
  /* Get link Packet */
  linkptr = trace_get_packet_buffer( packet, &linktype, &remaining);
  l3 = trace_get_layer3(packet, &ethertype, &remaining);

  switch(linktype) {
  case TRACE_TYPE_80211_RADIO:
    if(g_opts->radiotap){
      trace_oml_inject_radiotap(oml_mps, linktype, linkptr, packet, pktid);
    }
    /* Keep going to parse normal 802.11 headers */
  case TRACE_TYPE_80211:
    /* XXX: Do something for 802.11 packets without radiotap headers */
    break;
  case TRACE_TYPE_ETH:
      trace_oml_inject_ethernet(oml_mps, ethertype, linkptr, packet, pktid);
    break;
#if LIBTRACE_API_VERSION >= ((3<<16)|(0<<8)|(19))
  case TRACE_TYPE_UNKNOWN:
#endif
  case TRACE_TYPE_HDLC_POS:
  case TRACE_TYPE_ATM:
  case TRACE_TYPE_NONE:
  case TRACE_TYPE_LINUX_SLL:
  case TRACE_TYPE_PFLOG:
  case TRACE_TYPE_POS:
  case TRACE_TYPE_80211_PRISM:
  case TRACE_TYPE_AAL5:
  case TRACE_TYPE_DUCK:
  case TRACE_TYPE_LLCSNAP:
  case TRACE_TYPE_PPP:
  case TRACE_TYPE_METADATA:
  case TRACE_TYPE_NONDATA:
#if LIBTRACE_API_VERSION >= ((3<<16)|(0<<8)|(18))
  case TRACE_TYPE_OPENBSD_LOOP:
#endif
    break;
  }

  if (!l3) {
    /* Probable ARP or something */
    return;
  }

  /* Get the UDP/TCP/ICMP header from the IPv4/IPv6 packet */
  switch (ethertype) {
  case TRACE_ETHERTYPE_IP: {
    libtrace_ip_t* ip = (libtrace_ip_t*)l3;
    transport = trace_get_payload_from_ip(ip, &proto, &remaining);
    trace_oml_inject_ip(oml_mps, ip, packet, now, pktid);
    //size_of_packet =  trace_get_capture_length(ip);
    if (!transport) return;
    break;
  }

  case TRACE_ETHERTYPE_IPV6: {
    libtrace_ip6_t* ip6 = (libtrace_ip6_t*)l3;
    trace_oml_parse_ip6_hdrs(oml_mps, ip6, now, tv, pktid);
    transport = trace_get_payload_from_ip6(ip6, &proto, &remaining);
    trace_oml_inject_ip6(oml_mps, ip6, now, pktid);
    if (!transport) return;

    break;
  }

  default:
    return;
  }

  /* Parse the udp/tcp/icmp payload */
  switch(proto) {
  case TRACE_IPPROTO_ICMP  : {
    libtrace_icmp_t* icmp = trace_get_icmp(packet);
    if(icmp->type == 0 || icmp->type == 8) { // only report ping
      uint16_t icmp_sequ_nb = icmp->un.echo.sequence;
      trace_oml_inject_icmp(oml_mps, pktid, icmp->type, icmp_sequ_nb, tv, 4);
    }
    return;
  }
  case TRACE_IPPROTO_TCP:{
    libtrace_tcp_t* tcp = trace_get_tcp(packet);
    payload = trace_get_payload_from_tcp(tcp, &remaining);
    trace_oml_inject_tcp(oml_mps, tcp, packet, payload, now, pktid);
    if (!payload)
      return;
    break;
  }
  case TRACE_IPPROTO_UDP:{
    libtrace_udp_t* udp = trace_get_udp(packet);
    payload = trace_get_payload_from_udp(udp, &remaining);
    trace_oml_inject_udp(oml_mps, udp, packet, payload, now, pktid);
    if (!payload)
      return;
    break;
  }
  case TRACE_IPPROTO_ICMPV6: {
#if LIBTRACE_API_VERSION >= ((3<<16)|(0<<8)|(18))
    libtrace_icmp6_t* icmp6 = trace_get_icmp6(packet);
    if(icmp6->type == 128 || icmp6->type == 129) { // only report ping
      uint16_t icmp_sequ_nb = icmp6->un.echo.sequence;
      trace_oml_inject_icmp(oml_mps, pktid, icmp6->type, icmp_sequ_nb, tv, 6);
    }
#else
    logwarn ("ICMPv6 packets capture no longer supported with libtrace<3.0.18");
#endif
  }
  default:
    return;
  }
}

void iferr(libtrace_t *trace)
{
  libtrace_err_t err = trace_get_err(trace);
  if (err.err_num==0)
    return;
  fprintf(stderr,"ERROR\tlibtrace reports: %s\n", err.problem);
  exit(1);
}

static int
run(opts_t* opts, oml_mps_t* oml_mps)
{
  libtrace_t* trace;
  libtrace_packet_t* packet;
  libtrace_filter_t* filter;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long start_time = tv.tv_sec;

  trace = trace_create(opts->interface);
  if (trace_is_err(trace)) {
    trace_perror(trace, "ERROR\tCannot open trace file ");
    return 1;
  }

  if (opts->snaplen > 0) {
    if (trace_config(trace, TRACE_OPTION_SNAPLEN, &opts->snaplen)) {
      trace_perror(trace, "WARN\tCannot set snaplen");
    }
  }

  if (opts->filter) {
    filter = trace_create_filter(opts->filter);
    fprintf(stderr,"INFO\tfilter is `%s' \n", opts->filter);
    if (trace_config(trace, TRACE_OPTION_FILTER, filter)) {
      trace_perror(trace, "WARN\tCannot set filter");
    }
    iferr(trace);
  }

  if (opts->promisc) {
    if (trace_config(trace, TRACE_OPTION_PROMISC, &opts->promisc)) {
      trace_perror(trace, "WARN\tCannot set interface in PROMISC mode");
    }
  }
  if (trace_start(trace)==-1) {
    iferr(trace);
  }

  packet = trace_create_packet();
  while (trace_read_packet(trace, packet) > 0 && !stop_loop) {
    per_packet(oml_mps, packet, start_time, omlc_guid_generate());
  }

  trace_destroy_packet(packet);
  if (trace_is_err(trace)) {
      trace_perror(trace, "ERROR\tCannot capture packets");
  }
  trace_destroy(trace);

  return 0;
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
main(int argc, const char *argv[])
{
  char str[50] = "int:";
  char radiotap_dev[68] = "/proc/sys/net/";
  FILE *radio_dev_type;
  int result;

  loginfo("%s\n", PACKAGE_STRING);

  result = omlc_init("trace", &argc, argv, NULL);
  if (result == -1) {
    fprintf (stderr, "ERROR\tCould not initialise OML\n");
    exit (1);
  }

  // parsing command line arguments
  poptContext optCon = poptGetContext(NULL, argc, argv, options, 0);
  int c;
  while ((c = poptGetNextOpt(optCon)) > 0) {}

  if (g_opts->interface == NULL) {
    logerror("Missing interface (use -i)\n");
    return 1;
  }

  if(g_opts->radiotap){
    strcat(radiotap_dev, g_opts->interface);
    strcat(radiotap_dev,"/dev_type");

    radio_dev_type = fopen(radiotap_dev,"rb");
    if(radio_dev_type == NULL){
      logwarn("You need to enable radiotap by setting value `803' in `%s'.\n"
             "INFO\tRadiotap measurements are disabled for this run.\n", radiotap_dev);
      g_opts->radiotap = 0;
    }
  }

  strcat(str,g_opts->interface);
  g_opts->interface=str;
  // Initialize measurment points
  oml_register_mps();  // defined in xxx_oml.h
  omlc_start();
  oml_inject_metadata(argc, argv);

  signal (SIGTERM, quit_handler);
  signal (SIGQUIT, quit_handler);
  signal (SIGINT, quit_handler);

  run(g_opts, g_oml_mps);

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
