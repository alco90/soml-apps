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

#ifndef OTG_UDP_INPORT_H
#define OTG_UDP_INPORT_H

#include "otg2/source.h"
#include "otg2/packet.h"
#include "socket.h"

class UDPInPort: public ISource, public Socket
{
public:
  UDPInPort();

  void init();

  /** Returns the next packet from the source.
   * The (optionally) packet 'p' can be used to
   * minimize packet creation. If not used, it
   * should be freed by this generator.
   */
  Packet* nextPacket(Packet* p);

  void closeSource();

  inline IComponent* getConfigurable() { return this; }

  time_t timestamp;

protected:
  inline const char* getNamespace() { return "udp"; }

  void defOpts();

  void initSocket();

  int maxPktLength_;
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
