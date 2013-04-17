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

#include <iostream>
#include <netdb.h>
#include <arpa/inet.h>

#include "sockgate.h"
#include "stream.h"

using namespace std;

SockGate::SockGate(): Gate(), sockfd_(0)
{
  setHostname("localhost");
  setPort(DEFAULT_RECV_PORT);
}

/** Init Function to initialize a socket port
 *  default port number is set to 4000, hostname is set to "localhost"
 *
 * if the myaddr_ is not set by command line ( both hostname and port has to be set simultaneously),
 * default address parameters will be given in Init().
 */
void SockGate::init()
{
  if (sockfd_ != 0) { return; }
  if ( myaddr_.isSet() == false) {
    setHostname("localhost");
    setPort(DEFAULT_RECV_PORT);
  }
}

/** Function to check if the IP address of receiving interface is set in myaddr_.
 * Giving an IP address is the strongest argument to specify a receiving interface,
 * instead of using hostname
 * or "localhost". This is necessary when LIBMAC functions are desired.
 */
bool
SockGate::isIPAddrSet()
{
  if (inet_addr(myaddr_.getHostname()) == INADDR_NONE ) { return false; }
  return true;
}

/** Fill sockaddr_in 'address' structure with information taken from 'addr' and return it cast to a 'struct sockaddr'.
 * It handles following situations:
 * - if hostname is given as empty "", then INADDR_ANY is used in return
 * - if an IP address is given, then address could be set directly
 * - if a hostname is given, call gethostbyname() to find the ip address of the hostname from DNS
 */
struct sockaddr*
SockGate::setSockAddress(Address *addr, struct sockaddr_in *address)
{
  char *hostname;
  int port;
  unsigned int tmp;
  struct hostent *hp;

  hostname = addr->getHostname();
  port = addr->getPort();

  address->sin_family = AF_INET;
  address->sin_port   = htons((short)port);

  if (strcmp(hostname, "") == 0) {
    address->sin_addr.s_addr = htonl(INADDR_ANY);
  }
  else {
    tmp = inet_addr(hostname);  // If an IP address is given
    if(tmp != (unsigned long) INADDR_NONE){

      address->sin_addr.s_addr = tmp;
    } else{  // if a hostname is passed, call DNS
      if ((hp = gethostbyname(hostname)) == NULL) {
        throw "Error in Resolving hostname!" ;
      }
      memcpy((char *)&address->sin_addr, (char *)hp->h_addr, hp->h_length);
    }
  }
  return (sockaddr*)address;
}

/** Function to interpreate hostname and port from the SocketAddress */
void
SockGate::decodeSockAddress(Address *addr, struct sockaddr_in *address)
{
  addr->setHostname(inet_ntoa(address->sin_addr));
  addr->setPort(ntohs(address->sin_port));
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
