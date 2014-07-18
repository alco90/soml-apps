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

#include "udp_outport.h"
#include "udp_inport.h"
#include "null_outport.h"

Sender*
Port::createOutPort(const char* name)
{
  if (strcmp(name, "udp") == 0) {
    return new UDPOutPort();
  } else if (strcmp(name, "null") == 0) {
    return new NullOutPort();
  }
  return NULL;
}

const char*
Port::listOutPorts()
{
  return "udp|null";
}

const char*
Port::getDefOutPortName()
{
  return "udp";
}

ISource*
Port::createInPort(const char* name)
{
  if (strcmp(name, "udp") == 0) {
    return new UDPInPort();
  }
  return NULL;
}

const char*
Port::listInPorts()
{
  return "udp";
}

const char*
Port::getDefInPortName()
{
  return "udp";
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
