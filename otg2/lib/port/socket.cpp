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

#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <ocomm/o_log.h>

#include "otg2/port.h"  // defines defaults
#include "socket.h"

Socket::Socket(): sockfd_(0)
{
  localport_ = 0;
  dstport_ = DEFAULT_PORT;
}

void
Socket::defOpts()
{
  defOpt("local_host", POPT_ARG_STRING, (void*)&localhost_,
      "Name of local host", "[name]");
  defOpt("local_port", POPT_ARG_INT, (void*)&localport_,
      "Local port to bind to");
  defOpt("dst_host", POPT_ARG_STRING, (void*)&dsthost_,
      "Name of destination host", "[name]");
  defOpt("dst_port", POPT_ARG_INT, (void*)&dstport_,
      "Destination port to send to");
}

/** Init Function to initialize a Internet socket port.
 *  Init will do
 *  - set the port's own  address
 *  - set the port's destination address
 *  - check if the stream is load
 *
 *  Notes:
 *  default port number is set to 3000, hostname is set to "localhost"
 * if the myaddr_ is not set by command line ( both hostname and port has to be set simultaneously),
 * default address parameters will be given in Init().
 *
 */
void
Socket::init()
{
  struct sockaddr_in addr;

  if (sockfd_ != 0) { return; } // already initialized

  initSocket();
  if (nblockflag_  == 1) {
    if (fcntl(sockfd_, F_SETFL, O_NONBLOCK)  == -1) {
      logerror("Error in fcntl(): %s\n", strerror(errno));
      throw "Failed to set non-blocking option for a socket...";
    }
  }

  //Address *emptyAddr = new Address("", myaddr_.getPort());
  setSockAddress(localhost_, localport_, &addr);
  logdebug("Binding port to '%s:%d'\n", localhost_, localport_);
  if (bind(sockfd_, (struct sockaddr*)&addr, sizeof(struct sockaddr_in))  < 0 ) {
    throw "Socket Bind Error";
  }
}

struct sockaddr *
Socket::setSockAddress(Address *addr, struct sockaddr_in *address)
{
  return (struct sockaddr*)address;
}

/** Fill sockaddr_in 'address' structure with information taken from 'addr' and return it cast to a 'struct sockaddr'.
 * It handles following situations:
 * - if hostname is given as empty "", then INADDR_ANY is used in return
 * - if an IP address is given, then address could be set directly
 * - if a hostname is given, call gethostbyname() to find the ip address of the hostname from DNS
 */
struct sockaddr*
Socket::setSockAddress(const char* hostname, int port, struct sockaddr_in *address)
{
  unsigned int tmp;
  struct hostent *hp;

  address->sin_family = AF_INET;
  address->sin_port   = htons((short)port);

  if (hostname == NULL || strcmp(hostname, "") == 0) {
    address->sin_addr.s_addr = htonl(INADDR_ANY);
  } else {
    tmp = inet_aton(hostname, &(address->sin_addr));
    if (tmp == 0) {  // if a hostname is passed, call DNS
      if ((hp = gethostbyname(hostname)) == NULL) {
        logerror("Error in gethostbyname(): %s\n", hstrerror(errno));
        throw "Error resolving hostname!" ;
      }
      memcpy((char *)&address->sin_addr, (char *)hp->h_addr, hp->h_length);
    }
  }
  return (sockaddr*)address;
}

/** Function to interpreate hostname and port from the SocketAddress */
void
Socket::decodeSockAddress(Address *addr, struct sockaddr_in *address)
{
  addr->setHostname(inet_ntoa(address->sin_addr));
  addr->setPort(ntohs(address->sin_port));
}

void
Socket::close()
{
  ::close(sockfd_);
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/

