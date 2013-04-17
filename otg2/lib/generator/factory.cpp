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
/** \file factory.cpp
 * \brief Contains various factory methods for objects which may be dynamically loaded.
 */

#include "cbr_generator.h"
#include "expo_generator.h"

Generator*
Generator::create(const char* name) {
  if (strcmp(name, "cbr") == 0) {
    return new CBR_Generator();
  } else if (strcmp(name, "expo") == 0) {
    return new Expo_Generator();
  }
  return NULL;
}

const char*
Generator::listGenerators()
{
  return "cbr|expo";
}

const char*
Generator::getDefGeneratorName()
{
  return "cbr";
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
