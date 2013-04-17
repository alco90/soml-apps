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

#include "tcpsock_port.h"
#include "tcpsock_port_helper.h"

using namespace std;

/**  Maximum 10 connections for a TCP server */
#define BACKLOG   10

TCPSockPort::TCPSockPort()
{
}

/** Init Funciton to initialize a TCP socket port
 *
 * The port bind to its own address, generate a TCP socket.
 * This TCP socket will be used by sender to send packets, and for receiver to "listen".
 * Packet reception will use new socket file desciptor.
 *
 * Bind to local address is one important task in init()
 * Here source address of node itself (myaddr_) does not really be used by bind function of port.
 * The program use INADDR_ANY as the address filled in address parameters of bind().
 * So, we need an empty hostname with the port number.
 *
 * Another important thing init() does is: for TCP sender port, the init is going to connect to the other end.
 *
 * Usually, port state should changed from uninitialized to running
 * but in case of port being paused from beginning, we keep it paused.
 */
void
TCPSockPort::init()
{
  if (sockfd_ != 0) { return; } // already initialized

  InternetPort::init();

  struct sockaddr_in addr;
  setSockAddress(desthost_, destport_, &addr);
  if (connect(sockfd_, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
    perror("ERROR\tconnect()");
    throw "Could not connect to TCP server";
  }
  cerr << "INFO Connected to TCP server" << endl;
}

void
TCPSockPort::initSocket()
{
  if ((sockfd_ = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("ERROR\tsocket()");
    throw "Could not create TCP socket";
  }
}

/** The main send function of TCP Socket Port.
 * Be sure the sockfd_ is already connected to a remote TCP server
 * First, check if the port is still in "running" state.
 * Then, simply call send() to send packet
 */
void
TCPSockPort::sendPacket(Packet *pkt)
{
  int len = send(sockfd_, pkt->getPayload(), pkt->getPayloadSize(), 0);
  if (len == -1) { perror("send"); }
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
