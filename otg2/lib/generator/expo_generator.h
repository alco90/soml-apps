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
/** \file expo_generator.h
 * \brief Exponential GENERATOR Module
 *
 * Exponential: Exponential BIT RATE Traffic
 * Specify mean burst time
 * Specify mean idle time
 * Rate
 * Fixed packet size
 */

#ifndef EXPO_GENERATOR_H
#define EXPO_GENERATOR_H

#include <popt.h>

#include "otg2/packet.h"
#include "otg2/generator.h"
#include "randomvariable.h"

class Expo_Generator: public Generator, public Component
{
public:
  Expo_Generator(int size=512, double rate=4096.0, double ontime=1.0, double offtime=1.0);

  void init();
  void update();

  Packet* nextPacket(Packet* p);

  inline void
    closeSource() {}

  inline IComponent*
    getConfigurable() {return this;}

protected:
  inline const char*
    getNamespace() { return "exp"; }

  void
    defOpts();

private:
  /** packet size variable, constant. */
  int pktSize_;
  /** send rate during on time (bps) bits per second */
  double rate_;
  /** average length of the burst (sec), burst time */
  double ontime_ ;
  /** Exp random var: average length of idle time (sec) */
  double offtime_ ;
  /** Exp random var: average length of idle time (sec) */
  ExponentialRandomVariable offtimeVar_ ;
  /** interarrival time defined as the offset between two beginnings of consecutive packets during burst */
  double pktInterval_;
  /** used to record last packet generation time. */
  double lastPktStamp_;
  /** Exp Random variable: Average number of packets during burst period */
  ExponentialRandomVariable burstLength_;
  /** number of remaining packets in a burst, tracking the deduction of burstLength */
  unsigned int rem_;
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
