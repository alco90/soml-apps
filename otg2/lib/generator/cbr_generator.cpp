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
#include <string.h>
#include <stdlib.h>

#include "cbr_generator.h"

using namespace std;

/** Constructor */
CBR_Generator::CBR_Generator()
{
  pktSize_ = 512;
  pktInterval_ = 1;
  pktRate_ = 8 * pktSize_; // 1 packet/sec

}

void
CBR_Generator::update()
{
  if (pktRate_ == 0) { throw "Rate cannot be set to zero!"; }
  pktInterval_ = (8.0 * pktSize_) / pktRate_;
}

void
CBR_Generator::defOpts()
{
  defOpt("size", POPT_ARG_INT, &pktSize_, "Size of packet", "Bytes");
  defOpt("interval", POPT_ARG_FLOAT, &pktInterval_, "Interval between consecutive packets", "milliseconds");
  defOpt("rate", POPT_ARG_FLOAT, &pktRate_, "Data rate of the flow", "kbps");
}

/** Initialize generator. Should be called after all initial properties are set. */
void
CBR_Generator::init()
{
  lastPktStamp_ = 0;
  this->update();
}

/** Key function of the traffic generator
 * Determine the parameters of next packet, set size and timestamp.
 * \param p packet structure pointer to carry the calclulated parameter values
 */
Packet* 
CBR_Generator::nextPacket(Packet* p)
{
  p->setPayloadSize(pktSize_);
  lastPktStamp_ += pktInterval_;
  p->setTxTime(lastPktStamp_);
  return p;
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
