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

#ifndef GENERATOR_H
#define GENERATOR_H

#include "otg2/source.h"

/** Generator is an interface for any componet which can generate
 * a packet. It is usually called from a stream.
 */
class Generator: public ISource
{
public:
  static Generator* create(const char* name);
  static const char* getDefGeneratorName();
  static const char* listGenerators();

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
