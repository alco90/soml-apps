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

#ifndef APPLICATION_H_
#define APPLICATION_H_

#include <popt.h>

#include "otg2/stream.h"
#include "otg2/generator.h"
#include "otg2/port.h"

#define HELP_FLAG 99
#define USAGE_FLAG 98
#define VERSION_FLAG 97

class Application
{
public:

  Application(int argc, const char * argv[], const char* defLogFile = "-"); /* Deprecated, throws an exception */
  Application(const char *appname, int argc, const char * argv[], const char* defLogFile, const char* applongname = NULL, const char* copyright = NULL);
  ~Application();

  virtual void
    run();

protected:

  virtual Sender* createSender(const char* senderName) = 0;

  virtual ISource* createSource(const char* sourceName) = 0;

  void showHelp(poptContext optCon, char* component_name);

  virtual void printVersion();

  virtual const struct poptOption* getComponentOptions(char* component_name) = 0;

  void parseOptionsPhase1();
  void parseOptionsPhase2();

  int parseRuntimeOptions(char*   msg);

  void setSenderInfo(
      const char* longName,
      char        shortName,        /* may be ’\0’ */
      const char*       descrip,        /* description for autohelp -- may be NULL */
      const char*       argDescrip
      );

  void setSourceInfo(
        const char* longName,
        char        shortName,        /* may be ’\0’ */
        const char* descrip,        /* description for autohelp -- may be NULL */
        const char* argDescrip
        );

  struct poptOption* phase1_;
  struct poptOption* phase2_;

  int argc_;
  const char** argv_;

  const char* sender_name_;
  const char* source_name_;
  int   clockref_;
  char* component_name_;

  ISource*   source_;
  Sender*    sender_;
  Stream*    stream_;

  int   log_level_;
  const char* logfile_name_;

  const char* app_name_;
  const char* app_long_name_;
  const char* copyright_;

};

#endif /*APPLICATION_H_*/

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
