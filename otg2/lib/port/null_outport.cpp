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

#include "null_outport.h"
#include <ocomm/o_log.h>

NullOutPort::NullOutPort()
{
  // nothing to do
}

void
NullOutPort::defOpts()
{
  // nothing to do
}


void
NullOutPort::init()
{
  // nothing to do
}

Packet*
NullOutPort::sendPacket(Packet* pkt)
{
  int pktLength = pkt->getPayloadSize();
  logdebug("Consuming  packet of size '%d'\n", pktLength);
  return pkt;
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
