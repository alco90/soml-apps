#include <fstream>
#include <iostream>
using namespace std;

#include <sys/time.h>
#include <iostream>
using namespace std;

#include <otg2/port.h>
#include <otg2/otg2_app.h>

#define STDIN 0
#define MAX_INPUT_SIZE 256

OTG::OTG(
  int argc,
  const char* argv[],
  const char* senderName,
  const char* sourceName,
  const char* appName,
  const char* copyright
): Application("otg2", argc, argv, "-", appName, copyright)

{
  sender_name_ = senderName == NULL ? Port::getDefOutPortName() : senderName;
  source_name_ = sourceName == NULL ? Generator::getDefGeneratorName() : sourceName;
  setSenderInfo("protocol", 'p', "Protocol to use to send packet", Port::listOutPorts());
  setSourceInfo("generator", 'g', "Generator producing packets", Generator::listGenerators());

}


const struct poptOption*
OTG::getComponentOptions(
  char*       component_name
) {
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
OTG::createSender(
  const char* name
) {
  return Port::createOutPort(name);
}

ISource*
OTG::createSource(
  const char* name
) {
  return Generator::create(name);
}


