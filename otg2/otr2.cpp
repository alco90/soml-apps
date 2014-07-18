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

#include <iostream>
#include <signal.h>
#include <stdlib.h>
#include <ocomm/o_log.h>

#include "otg2/otr2_app.h"

using namespace std;

OTR* otr;

static void
quit_handler (int signum)
{
  exit(0);
}

int main(int argc, const char * argv[])
{
  try {
    otr = new OTR(argc, argv);

    signal (SIGTERM, quit_handler);
    signal (SIGQUIT, quit_handler);
    signal (SIGINT, quit_handler);

    otr->run();

  } catch (const char *reason ) {
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
