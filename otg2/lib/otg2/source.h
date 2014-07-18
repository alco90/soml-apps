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

#ifndef SOURCE_H_
#define SOURCE_H_

#include "otg2/packet.h"
#include "otg2/component.h"

class ISource
{
public:
  virtual ~ISource() {}

  /** Returns the next packet from the source.
   * The (optionally) packet 'p' can be used to
   * minimize packet creation. If not used, it
   * should be freed by this generator.
   */
  virtual Packet* nextPacket(Packet* p) = 0;

  /** Function to close whatever is behind this source. Any further
   * calls to +nextPacket+ is illegal.
   */
  virtual void closeSource() = 0;

  virtual IComponent* getConfigurable() = 0;

};
#endif /*SOURCE_H_*/

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
