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

#ifndef OTR2_APP_H_
#define OTR2_APP_H_

#include "otg2/application.h"

#define APP_NAME "OTG2 Traffic Sink"

/** Main function of the OTR
 *
 * The major purpose of this function are:
 * - handle command-line inputs, before starting the program and at run-time.
 * - arrange wait, send, and stdin reading operations.
 *
 * It is designed as: First, parse all options in two phases.
 * After parsing the second level options, check the readiness of Port and Generator (if this is sender)
 * then initialize the port and set-up the scenario, and ready to start.

 * Here. we are going to have multiple streams in one OTR.
 * using multiple thread. the main thread is handling commands.
 * each stream is a seperate thread, by calling th stream_init functon, the stream is independently sending packets
 *
 */
class OTR: public Application
{
public:
  OTR(int argc, const char* argv[], const char* senderName = NULL, const char* sourceName = NULL, const char* appName = APP_NAME, const char* copyright = NULL);

protected:
  const struct poptOption* getComponentOptions(char* component_name);

  Sender* createSender(const char* senderName);

  ISource*createSource(const char* sourceName);
};

#endif /*OTR2_APP_H_*/

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
