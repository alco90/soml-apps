
#include <libtrace.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define USE_OPTS
#include "omf_trace_popt.h"
#include <netinet/in.h>
#define OML_FROM_MAIN
#include "omf_trace_oml.h"

static void
omlc_inject_ip(oml_mps_t* oml_mps, libtrace_ip_t* ip, libtrace_packet_t *packet,
               double time_now, uint64_t pktid)
{
    OmlValueU v[12];
    char buf_addr_src[INET_ADDRSTRLEN];
    char buf_addr_dst[INET_ADDRSTRLEN];
    char addr_src[INET_ADDRSTRLEN];
    char addr_dst[INET_ADDRSTRLEN];

    inet_ntop(AF_INET, &(ip->ip_src), buf_addr_src, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &(ip->ip_dst), buf_addr_dst, INET_ADDRSTRLEN);
    strcpy(addr_src,buf_addr_src);
    strcpy(addr_dst,buf_addr_dst);

    omlc_set_uint64(v[0], pktid);
    omlc_set_uint32(v[1], ip->ip_tos);
    omlc_set_uint32(v[2], ntohs(ip->ip_len));
    omlc_set_int32(v[3], ntohs(ip->ip_id));
    /* XXX: fragmentation flags are trimmed */
    omlc_set_uint32(v[4], ntohs(ip->ip_off & ~(7<<13)));
    omlc_set_uint32(v[5], ip->ip_ttl);
    omlc_set_uint32(v[6], ip->ip_p);
    omlc_set_uint32(v[7], ntohs(ip->ip_sum));
    omlc_set_const_string(v[8], addr_src );
    omlc_set_const_string(v[9], addr_dst);
    omlc_set_uint32(v[10], trace_get_capture_length(packet));
    omlc_set_double(v[11], time_now);
    omlc_inject(oml_mps->ip, v);
}

static void
omlc_inject_tcp(oml_mps_t* oml_mps, libtrace_tcp_t* tcp, libtrace_packet_t *packet,
                void* payload, double time_now, uint64_t pktid)
{
    (void) payload;
    OmlValueU v[10];

    omlc_set_uint64(v[0], pktid);
    omlc_set_uint32(v[1], trace_get_source_port(packet));
    omlc_set_uint32(v[2], trace_get_destination_port(packet));
    omlc_set_uint32(v[3], ntohs(tcp->seq));
    omlc_set_uint32(v[4], ntohs(tcp->ack_seq));
    omlc_set_uint32(v[5], ntohs(tcp->window));
    omlc_set_uint32(v[6], ntohs(tcp->check));
    omlc_set_uint32(v[7], ntohs(tcp->urg_ptr));
    omlc_set_uint32(v[8], trace_get_capture_length(packet));
    omlc_set_double(v[9], time_now);
    omlc_inject(oml_mps->tcp, v);

}

static void
omlc_inject_udp(oml_mps_t* oml_mps, libtrace_udp_t* udp, libtrace_packet_t *packet,
                void* payload, double time_now, uint64_t pktid)
{
    (void) payload;
    OmlValueU v[6];

    omlc_set_uint64(v[0], pktid);
    omlc_set_uint32(v[1], trace_get_source_port(packet));
    omlc_set_uint32(v[2], trace_get_destination_port(packet));
    omlc_set_uint32(v[3], ntohs(udp->len));
    omlc_set_uint32(v[4], ntohs(udp->check));
    omlc_set_double(v[5], time_now);
    omlc_inject(oml_mps->udp, v);
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
omlc_inject_radiotap(oml_mps_t* oml_mps, libtrace_linktype_t linktype,
                     void* linkptr, libtrace_packet_t* packet, uint64_t pktid)
{
  OmlValueU v[14];
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

  omlc_set_uint64(v[0], pktid);
  omlc_set_uint64(v[1], tsft);
  omlc_set_uint32(v[2], rate);
  omlc_set_uint32(v[3], freq);
  omlc_set_int32(v[4], s_strength);
  omlc_set_int32(v[5], n_strength);
  omlc_set_uint32(v[6], s_db_strength);
  omlc_set_uint32(v[7], n_db_strength);
  omlc_set_uint32(v[8], attenuation);
  omlc_set_uint32(v[9], attenuation_db);
  omlc_set_int32(v[10], txpower);
  omlc_set_uint32(v[11], antenna);
  omlc_set_const_string(v[12], macS);
  omlc_set_const_string(v[13], macD);
  omlc_inject(oml_mps->radiotap, v);
}

static void
per_packet(oml_mps_t* oml_mps, libtrace_packet_t* packet, long start_time, uint64_t pktid)
{
  double                last_ts;
  uint32_t              remaining;
  void*                 l3;
  uint16_t              ethertype;
  void*                 transport;
  uint8_t               proto;
  void*                 payload;
  void*                 linkptr;
  libtrace_linktype_t   linktype;
  last_ts = trace_get_seconds(packet);
  //  size_t size_of_packet = 0;
  struct timeval tv = trace_get_timeval(packet);
  double now = tv.tv_sec - start_time + 0.000001 * tv.tv_usec;
  /* Get link Packet */
  linkptr = trace_get_packet_buffer( packet, &linktype, &remaining);

  if(g_opts->radiotap){
    if(linktype == 15){
      omlc_inject_radiotap(oml_mps, linktype, linkptr, packet, pktid);
    }
  }

  l3 = trace_get_layer3(packet, &ethertype, &remaining);
  if (!l3) {
    /* Probable ARP or something */
    return;
  }

  /* Get the UDP/TCP/ICMP header from the IPv4/IPv6 packet */
  switch (ethertype) {
  case 0x0800: {
    libtrace_ip_t* ip = (libtrace_ip_t*)l3;
    transport = trace_get_payload_from_ip(ip, &proto, &remaining);
    omlc_inject_ip(oml_mps, ip, packet, now, pktid);
    //size_of_packet =  trace_get_capture_length(ip);
    if (!transport) return;
    break;
  }
  case 0x86DD:
    transport = trace_get_payload_from_ip6((libtrace_ip6_t*)l3,
                                           &proto,
                                           &remaining);
    if (!transport)
      return;

    break;
  default:
    return;
  }
  /* Parse the udp/tcp/icmp payload */
  switch(proto) {
  case 1:
    // icmp;
    return;
  case 6:{
    libtrace_tcp_t* tcp = trace_get_tcp(packet);
    payload = trace_get_payload_from_tcp(tcp, &remaining);
    omlc_inject_tcp(oml_mps, tcp, packet, payload, now, pktid);
    if (!payload)
      return;
    break;
  }
  case 17:{
    libtrace_udp_t* udp = trace_get_udp(packet);
    payload = trace_get_payload_from_udp(udp, &remaining);
    omlc_inject_udp(oml_mps, udp, packet, payload, now, pktid);
    if (!payload)
      return;
    break;
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
  printf("Error: %s\n",err.problem);
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
  uint64_t pktid = 0;

  trace = trace_create(opts->interface);
  if (trace_is_err(trace)) {
    trace_perror(trace,"Opening trace file");
    return 1;
  }

  if (opts->snaplen > 0) {
    if (trace_config(trace, TRACE_OPTION_SNAPLEN, &opts->snaplen)) {
      trace_perror(trace, "ignoring: ");
    }
  }

  if (opts->filter) {
    filter = trace_create_filter(opts->filter);
    printf("FILTER %s \n", opts->filter);
    if (trace_config(trace, TRACE_OPTION_FILTER, filter)) {
      trace_perror(trace, "ignoring: ");
    }
    iferr(trace);
  }

  if (opts->promisc) {
    if (trace_config(trace, TRACE_OPTION_PROMISC, &opts->promisc)) {
      trace_perror(trace, "ignoring: ");
    }
  }
  if (trace_start(trace)==-1) {
    iferr(trace);
  }

  packet = trace_create_packet();
  while (trace_read_packet(trace, packet) > 0) {
    per_packet(oml_mps, packet, start_time, pktid++);
  }

  trace_destroy_packet(packet);
  if (trace_is_err(trace)) {
    trace_perror(trace, "Reading packets");
  }
  trace_destroy(trace);

  return 0;
}

int
main(int argc, const char *argv[])
{
  char str[50] = "int:";
  char radiotap_dev[68] = "/proc/sys/net/";
  FILE *radio_dev_type;
  char *progname = strdup(argv[0]), *p = progname;
  do *p = (*p == '-') ? '_' : *p; while (*p++);

  omlc_init(progname, &argc, argv, NULL);

  // parsing command line arguments
  poptContext optCon = poptGetContext(NULL, argc, argv, options, 0);
  int c;
  while ((c = poptGetNextOpt(optCon)) > 0) {}

  if (g_opts->interface == NULL) {
    fprintf(stderr, "Missing interface\n");
    return 1;
  }

  if(g_opts->radiotap){
    strcat(radiotap_dev, g_opts->interface);
    strcat(radiotap_dev,"/dev_type");

    radio_dev_type = fopen(radiotap_dev,"rb");
    if(radio_dev_type == NULL){
      printf("WARNING:  You need to enable radiotap in %s, by putting the value 803.\n"
             "          Radiotap measurements are disabled for this run.\n", radiotap_dev);
      g_opts->radiotap = 0;
    }
  }
  strcat(str,g_opts->interface);
  strcpy(g_opts->interface, str);
  // Initialize measurment points
  oml_register_mps();  // defined in xxx_oml.h
  omlc_start();

  // Do some work
  run(g_opts, g_oml_mps);

  return(0);
}
