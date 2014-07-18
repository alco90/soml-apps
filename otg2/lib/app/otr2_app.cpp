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

#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <iostream>

#include "otg2/port.h"
#include "otg2/otr2_app.h"

using namespace std;

#define STDIN 0
#define MAX_INPUT_SIZE 256

OTR::OTR(int argc, const char* argv[], const char* senderName, const char* sourceName, const char* appName, const char* copyright):
  Application("otr2", argc, argv, "-", appName, copyright)
{
  sender_name_ = senderName == NULL ? (char*)"null" : senderName;
  source_name_ = sourceName == NULL ? Port::getDefInPortName() : sourceName;

  setSenderInfo("protocol", 'p', "Protocol to use to send packet", Port::listInPorts());
  setSourceInfo("sink", 'g', "What to do with received packets", Port::listOutPorts());
}


const struct poptOption*
OTR::getComponentOptions(char*       component_name)
{
  const struct poptOption* opts= NULL;
  ISource* gen;
  Sender*      port;

  if ((gen = createSource(component_name)) != NULL) {
    opts = gen->getConfigurable()->getOptions();
    cerr << "Options for generator '" << component_name << "'." << endl
      <<endl;
  } else if ((port = createSender(component_name)) != NULL) {
    opts = port->getConfigurable()->getOptions();
    cerr << "Options for port '" << component_name << "'." << endl <<endl;
  }
  return opts;
}

Sender*
OTR::createSender(const char* name)
{
  return Port::createOutPort(name);
}

ISource*
OTR::createSource(const char* name)
{
  return Port::createInPort(name);
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
