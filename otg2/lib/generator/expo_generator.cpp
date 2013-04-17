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
#include <ocomm/o_log.h>

#include "expo_generator.h"

using namespace std;

/** Constructor
 * This behaves as a Poisson generator if the rate is set to a
 * high value and ontime_ is set to zero
 */
Expo_Generator::Expo_Generator(int size, double rate, double ontime, double offtime)
{
  //two paramters to define a Expo_Generator
  pktSize_ = size;
  rate_ = rate;
  ontime_ = ontime;
  offtime_ = offtime;
  rem_ = 0;
}

void
Expo_Generator::defOpts()
{
  defOpt("size", POPT_ARG_INT, &pktSize_, "Size of packet", "bytes");
  defOpt("ontime", POPT_ARG_FLOAT, &ontime_, "Average length of burst", "msec");
  defOpt("offtime", POPT_ARG_FLOAT, &offtime_, "Average length of idle time", "msec");
  defOpt("rate", POPT_ARG_FLOAT, &rate_, "Data rate of the flow", "kbps");
}

/** Initialize generator. Should be called after all initial properties are set.
 * Derived parameters such as burstLength_ are computed here
 */
void
Expo_Generator::init()
{
  //pktInterval_ = 8.0* pktSize_ /rate_;
  //burstLength_.setMean(ontime_/pktInterval_);
  //burstLength_.setMean(ontime_*rate_)
  lastPktStamp_ = 0 ;
  update();
}

void
Expo_Generator::update()
{
  offtimeVar_.setMean(offtime_);
}

/** Determine the parameters of next packet, set size and timestamp.
 * \param p packet structure pointer to carry the calclulated parameter values
 */
Packet*
Expo_Generator::nextPacket(Packet* p)
{
  double t = pktInterval_;
  p->setPayloadSize(pktSize_); //set size first

  pktInterval_ = 8.0 * pktSize_ /rate_;
  burstLength_.setMean(ontime_/pktInterval_);
  if (rem_ == 0) {
    /* compute number of packets in next burst */
    rem_ = int(burstLength_.getSample() + .5);
    if (rem_ == 0) rem_ = 1;
    /* start of an idle period, compute idle time */
    lastPktStamp_ += offtimeVar_.getSample();
  }
  rem_--;
  lastPktStamp_ += t;
  p->setTxTime(lastPktStamp_);
  logdebug("Last packet sent at %d\n", lastPktStamp_);
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
