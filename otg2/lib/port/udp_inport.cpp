/*
 * Copyright 2004-2010 WINLAB, Rutgers University, USA
 * Copyright 2007-2013 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */

#include <netdb.h>
#include <arpa/inet.h>
#include <otg2/port.h>   // defines defaults
#include <ocomm/o_log.h>

#define OML_FROM_MAIN
#include "otr2_oml.h"
#include "udp_inport.h"

#define DEF_PKT_LENGTH 1024

UDPInPort::UDPInPort()
{
  struct timeval time;
  timestamp = time.tv_sec;

  localhost_ = "localhost";
  localport_ = DEFAULT_RECV_PORT;
  maxPktLength_ = DEF_PKT_LENGTH;

  oml_register_mps();
}

void
UDPInPort::defOpts()
{
  Socket::defOpts();
}

void
UDPInPort::init()
{
  if (sockfd_ != 0) { return; } // already initialized

  Socket::init();
}

void
UDPInPort::initSocket()
{
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    throw "Error while opening UDP socket";
  }
}

/** Returns the next packet from the source.
 * The (optionally) packet 'p' can be used to
 * minimize packet creation. If not used, it
 * should be freed by this generator.
 */
Packet*
UDPInPort::nextPacket(Packet* pkt)
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double now = tv.tv_sec - timestamp + 0.000001 * tv.tv_usec;

  if (pkt == NULL) { pkt = new Packet(); }
  struct sockaddr_in tmpSockAddr;
  int length = sizeof(struct sockaddr);
  char* buf =  pkt->getBufferPtr(maxPktLength_);
  int len = (int)recvfrom(sockfd_, buf, pkt->getBufferSize(), 0, (struct sockaddr*)&tmpSockAddr, (socklen_t *)&length);
  if (len == -1) {
    logerror("Error in recvfrom(): %s\n", strerror(errno));
    delete pkt;
    return NULL;
  }

  pkt->setPayloadSize(len);
  char* senderHost = inet_ntoa(tmpSockAddr.sin_addr);
  int senderPort = ntohs(tmpSockAddr.sin_port);
  logdebug("Receiving UDP packet of size '%d' from '%s:%d'\n",
      len, senderHost, senderPort);

  // UDP will stamp packet id, other protocols will not use this feature
  if (pkt->checkStamp() == 1) {
    pkt->setFlowId(pkt->extractShortVal());
    pkt->setSequenceNum(pkt->extractLongVal());
  }
  pkt->setTimeStamp(-1); // mark with current time

  oml_inject_udp_in(g_oml_mps->udp_in,
      now,
      pkt->getFlowId(),
      pkt->getSequenceNum(),
      len,
      senderHost,
      senderPort);

  return pkt;
}

void
UDPInPort::closeSource()
{
  Socket::close();
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
