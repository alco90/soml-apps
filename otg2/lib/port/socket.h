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

#ifndef OSOCKET_H_
#define OSOCKET_H_

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>

#include "otg2/component.h"
#include "otg2/address.h"

#ifndef INADDR_NONE
#  define INADDR_NONE (-1)
#endif

class Socket: public Component 
{
public:
  Socket();

  void init();
  void close();

protected:
  void defOpts();

  struct sockaddr* setSockAddress(Address *addr, struct sockaddr_in *address);
  struct sockaddr* setSockAddress(const char* hostname, int   port, struct sockaddr_in *address);

  void decodeSockAddress(Address *addr, struct sockaddr_in *address);

  virtual void initSocket() = 0;

  int buffersize_; ///<sender or receiver buffer sockopt... IMPLEMENT ME LATER     
  int sockfd_; ///<socket file descriptor

  int nblockflag_;

  const char* localhost_;
  int   localport_;
  char* dsthost_;
  int   dstport_;
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
