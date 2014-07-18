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

#ifndef OTG2_PORT_H
#define OTG2_PORT_H

#include "otg2/component.h"
#include "otg2/sender.h"
#include "otg2/packet.h"
#include "otg2/source.h"

#define DEFAULT_PORT 3000

/** Port is an abstract class for the interface to send and receive packets.
 * Ports which receive (and therefore 'produce') packet, implement the
 * ISource interface. Ports which send packets implement the Sender
 * interface.
 */
class Port
{
public:
  static Sender* createOutPort(const char* name);
  static const char* getDefOutPortName();
  static const char* listOutPorts();

  static ISource* createInPort(const char* name);
  static const char* getDefInPortName();
  static const char* listInPorts();
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
