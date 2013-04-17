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

#ifndef OTG_TCP_SOCK_PORT_H
#define OTG_TCP_SOCK_PORT_H

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <netinet/in.h>

#include "otg2/port.h"
#include "sockport.h"

class TCPSockPort: public InternetPort
{
public:
  TCPSockPort();

  void init();
  Packet* sendPacket(Packet* pkt);

protected:
  inline const char* getNamespace() { return "tcp"; }

  void initSocket();
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
