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
#include <ocomm/o_log.h>

#include "otg2/otg2_app.h"

using namespace std;

int main(int argc, const char * argv[])
{
  try {
    OTG* otg = new OTG(argc, argv);
    //otg->registerOutPortType("mp_udp", createMPOutPort);
    otg->run();

  } catch (const char *reason) {
    logerror("%s\n", reason);
    return -1;
  }

  return 0;
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
