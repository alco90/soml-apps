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
/** \file cbr_generator.h
 * \brief CBR GENERATOR Module
 *
 * CBR: CONSTANT BIT RATE Traffic
 * Fix packet interval
 * Fix packet size
 */

#ifndef CBR_GENERATOR_H
#define CBR_GENERATOR_H

#include <popt.h>
#include "otg2/packet.h"
#include "otg2/generator.h"
#include "otg2/component.h"

class CBR_Generator: public Generator, public Component
{
public:
  CBR_Generator();

  void
    init();

  Packet*
    nextPacket(Packet* p);

  inline void
    closeSource() {}

  inline IComponent*
    getConfigurable() { return this; }

private:
  inline const char*
    getNamespace() { return "cbr"; }

  void
    defOpts();

  void
    update();

  /** packet size variable, constant. */
  int pktSize_;
  /** interarrival time defined as the offset between two beginnings of consecutive packets */
  float pktInterval_;
  float pktRate_;

  /** used to record last packet generation time. */
  double lastPktStamp_;
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
