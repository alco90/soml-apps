/*
 * Copyright 2004-2010 WINLAB, Rutgers University, USA
 * Copyright 2007-2014 National ICT Australia (NICTA)
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */

#ifndef OTG_UDP_SOCK_PORT_H
#define OTG_UDP_SOCK_PORT_H

#include "otg2/port.h"
#include "otg2/packet.h"
#include "socket.h"

class UDPOutPort: public Sender, public Socket
{
public:
  UDPOutPort();

  void init();
  Packet* sendPacket(Packet *pkt);

  void closeSender();

  inline IComponent *getConfigurable() { return this; }

  time_t timestamp;

protected:
  inline const char *getNamespace() { return "udp"; }

  void defOpts();

  void initSocket();

  int bcastflag_;
  /**  for every outgoing packet of a connection, the dstaddress. */
  struct sockaddr_in dstSockAddress_; ;
};

#endif

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
