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

#ifndef ADDRESS_H
#define ADDRESS_H

#include <string.h>

#define MAX_HOSTNAME_LENGTH 256
#define MAC_ADDR_LENGTH 6


/**  Address Class is to handle addresses.
 * For normal socket, the address used will be a combination of IP address and port.
 * In Socket Programming, IP address it self is not enough to distinguish an connection (stream, flow...), port # is also needed.
 * For PF_PACKET sockets, the address used is MAC address (HW address). So, we also put macaddr_ as a member variable.
 */
class Address
{

public:

  Address();
  Address(char* hostname, short port);

  /** Check if an address has already been set or remain uninitialized */
  inline bool isSet() { return (hostname_[0] != '\0' && port_ >= 0); }

  inline void setPort(const short port){ port_ = port; }
  inline short getPort(){ return port_; }

  inline void setHostname(const char* hostname)
  {
    if (hostname == NULL) {
      hostname_[0] = '\0';
    } else {
      strncpy(hostname_, hostname, MAX_HOSTNAME_LENGTH);
    }
  }

  inline char* getHostname() {return hostname_;}

  inline unsigned char* getHWAddr() { return macaddr_;}

  void setHWAddr(unsigned char* hwaddr);

  void setHWAddrFromColonFormat(const char* colon_seperated_macaddr);

  char* convertHWAddrToColonFormat();

  Address* clone();

  /** Compare the two normal socket address is same or not */
  inline bool isSame(Address* addr)
  {
    if (port_ ==  addr->getPort()
        && strcmp(hostname_, addr->getHostname()) == 0) {
      return true;
    }
    return false;
  }

  bool isSameMACAddr(Address* addr);

  //const struct poptOption* getOptions();

private:
  /**  both hostname and ipaddress format (10.0.0.1) could be given as a string */
  char   hostname_[MAX_HOSTNAME_LENGTH];
  /**  port number for UDP or TCP (Transport layer) */
  short port_;

  /** optional use */
  char * ipaddr_;
  /**  optional use for Ethernet Socket */
  unsigned char   macaddr_[MAC_ADDR_LENGTH];
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
