#include "udp_inport.h"
//#include "stream.h"
//#include <iostream>
//using namespace std;
#include <netdb.h>
#include <arpa/inet.h>
#include <otg2/port.h>   // defines defaults
#include <ocomm/o_log.h>
#ifdef WITH_OML
#define OML_FROM_MAIN
#include "otr2_oml.h"
#endif


//#include "otg2/dummysink.h"
#define DEF_PKT_LENGTH 1024

UDPInPort::UDPInPort()

{
  struct timeval time;
  int i = gettimeofday(&time, NULL);
  timestamp = time.tv_sec;

  localhost_ = "localhost";
  localport_ = DEFAULT_RECV_PORT;
  maxPktLength_ = DEF_PKT_LENGTH;


#ifdef WITH_OML
  oml_register_mps();
#endif

}

void
UDPInPort::defOpts()

{
  Socket::defOpts();

  // Nothing
}

void
UDPInPort::init()

{
  if (sockfd_ != 0) return; // already initialized

  Socket::init();
}

void
UDPInPort::initSocket()

{
  if ((sockfd_ = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    throw "Error while opening UDP socket";
  }
}

/**
* Returns the next packet from the source.
* The (optionally) packet 'p' can be used to
* minimize packet creation. If not used, it
* should be freed by this generator.
*/
Packet*
UDPInPort::nextPacket(
    Packet* pkt
) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  double now = tv.tv_sec - timestamp + 0.000001 * tv.tv_usec;

  if (pkt == NULL) pkt = new Packet();
  struct sockaddr_in tmpSockAddr;
  int length = sizeof(struct sockaddr);
  char* buf =  pkt->getBufferPtr(maxPktLength_);
  int len = (int)recvfrom(sockfd_, buf, pkt->getBufferSize(), 0,
                  (struct sockaddr*)&tmpSockAddr, (socklen_t *)&length);
  if (len == -1) {
    perror("recvfrom");
    delete pkt;
    return NULL;
  }


  pkt->setPayloadSize(len);
  char* senderHost = inet_ntoa(tmpSockAddr.sin_addr);
  int senderPort = ntohs(tmpSockAddr.sin_port);
  o_log(O_LOG_DEBUG2, "Receiving UDP packet of size '%d' from '%s:%d'\n",
        len, senderHost, senderPort);

  // UDP will stamp packet id, other protocols will not use this feature
  if (pkt->checkStamp() == 1) {
    pkt->setFlowId(pkt->extractShortVal());
    pkt->setSequenceNum(pkt->extractLongVal());
  }
  pkt->setTimeStamp(-1); // mark with current time

#ifdef WITH_OML
  oml_inject_udp_in(g_oml_mps->udp_in,
		  now,
		  pkt->getFlowId(),
		  pkt->getSequenceNum(),
		  len,
		  senderHost,
		  senderPort);
#endif

  return pkt;
}

void
UDPInPort::closeSource()

{
  Socket::close();
}
