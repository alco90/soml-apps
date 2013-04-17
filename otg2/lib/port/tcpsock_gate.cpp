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
#include <errno.h>
#include <ocomm/o_log.h>

#include "tcpsock_gate.h"
#include "tcpsock_gate_helper.h"
#include "stream.h"

using namespace std;

#define BACKLOG   10  ///< Maximum 10 connections for a TCP server

/** Init Funciton to initialize a TCP socket port
 *  default port number is set to 4500 for receiver, hostname is set to "localhost"
 *
 * if the myaddr_ is not set by command line ( both hostname and port has to be set simultaneously),
 * default address parameters will be given in init().
 * The port bind to its own address, generate a TCP socket.
 * This TCP socket will be used by sender to send packets, and for receiver to "listen".
 * Packet reception will use new socket file desciptor.
 *
 * Bind to local address is one important task in init()
 * Here source address of node itself (myaddr_) does not really be used by bind function of port.
 * The program use INADDR_ANY as the address filled in address parameters of bind().
 * So, we need an empty hostname with the port number.
 */
void
TCPSockGate::init()
{
  SockGate::init();
  if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    logerror("Error in socket(): %s\n", strerror(errno));
    throw "Could not create TCP socket";
  }
  Address *emptyAddr = new Address("", myaddr_.getPort());
  struct sockaddr* addr = setSockAddress(emptyAddr, &mySockAddress_);
  if (  bind(sockfd_, addr, sizeof(struct sockaddr_in)) < 0) {
    throw "TCP Socket Bind error";
  }
  loginfo("Listening for TCP connections on port %d\n", myaddr_.getPort());
  // TCP revceiver is actually a TCP server
  listen(sockfd_, BACKLOG);
}

/** Function to start an endless loop for Listing TCP connections
 *  This Marks the beginning of an independent receiving thread
 *  Each incomming connection will be assigned to a flow.
 */
void
TCPSockGate::startReceive()
{
  int i,fdmax, newfd;
  fd_set readfds, master;
  FD_ZERO(&readfds);
  FD_ZERO(&master);
  FD_SET(sockfd_,&master);
  fdmax = sockfd_;
  while (1) {
    readfds =  master;
    select(fdmax+1, &readfds, (fd_set *)0, (fd_set *)0, NULL);

    for ( i=0; i<=fdmax; i++) {
      if (FD_ISSET(i, &readfds)) {
        if (i == sockfd_) {
          newfd =  acceptNewConnection();
          FD_SET(newfd, &master);  // add to master set
          if (newfd > fdmax) {   // keep track of the maximum
            fdmax = newfd;
          }

        } else if (receivePacket(i)== false) {
          FD_CLR(i, &master);

        } else {
          inboundPacket();
        }
      }
    }
  }
}

/** This will use rlcurr_ to receive a packet. */
bool
TCPSockGate::receivePacket(int fd)
{
  rlcurr_ = searchFlowbyFd(fd);
  int len = (int)recv(rlcurr_->getID(), pkt_->getPayload(), pkt_->getBufferSize(),0);
  if (len == -1) {
    logerror("Error in recv(): %s\n", strerror(errno));
  }
  if (len <= 0  ) {
    close(rlcurr_->getID()); // Connection has been closed by the other end (==0) or error occurs
    return false;
  }
  pkt_->rxMeasure_->setReceivedLength(len);

  return true;
}

/** Function to accept new connections
 * After reaching maximum connections, the default sockfd_ should be closed to prohibit new connections
 * \return the new socket file descriptor
 */
int
TCPSockGate::acceptNewConnection()
{
  struct sockaddr_in tmpSockAddr; // connector's address information
  int sin_size =  sizeof(struct sockaddr_in);
  int newfd = accept(sockfd_, (struct sockaddr *)&tmpSockAddr, (socklen_t*)&sin_size);

  if ( newfd == -1){
    logerror("Error in accopt(): %s\n", strerror(errno));
    throw "Could not accept new connection";
  }
  loginfo("Accepting a new connection\n");
  decodeSockAddress(&itsaddr_, &tmpSockAddr);
  addFlow(newfd, &itsaddr_);

  // FIXME add some code to prohibit new connections if there are already 10 connections.

  return newfd;
}

const struct poptOption*
TCPSockGate::getOptions()
{
  // FIXME return tcp_sock_gate_get_options(this, NULL);
  return NULL;
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
