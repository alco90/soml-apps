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

#ifndef OTG_NULL_OUT_PORT_H
#define OTG_NULL_OUT_PORT_H

#include "otg2/sender.h"
#include "otg2/packet.h"
#include "otg2/component.h"


class NullOutPort: public Sender, public Component
{
public:
  NullOutPort();

  void init();
  Packet* sendPacket(Packet *pkt);

  void closeSender() {}

  inline IComponent* getConfigurable() { return this; }

protected:
  inline const char* getNamespace() { return "null"; }

  void defOpts();

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
