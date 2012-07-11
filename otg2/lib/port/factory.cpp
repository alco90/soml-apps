

//#include "tcpsock_port.h"
#include "udp_outport.h"
#include "udp_inport.h"
#include "null_outport.h"

Sender*
Port::createOutPort(
  const char* name
) {
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
Port::createInPort(
  const char* name
) {
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
