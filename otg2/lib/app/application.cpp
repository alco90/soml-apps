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


#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <iostream>
#include <ocomm/o_log.h>
#include <oml2/omlc.h>

#include "otg2/application.h"

using namespace std;

#include "../version.h"


#define STDIN 0
#define MAX_INPUT_SIZE 256

static struct poptOption
phase1[] = {
  { "help", 'h', POPT_ARG_STRING | POPT_ARGFLAG_OPTIONAL, NULL, HELP_FLAG, "Show help", "[component]"},
  { "usage", '\0', POPT_ARG_NONE, NULL, USAGE_FLAG, "Display brief use message"},
  { "sender", '\0', POPT_ARG_STRING, NULL, 0},
  { "source", '\0', POPT_ARG_STRING, NULL, 0},
  { NULL, '\0', POPT_ARG_INCLUDE_TABLE, NULL, 0},
  { "debug-level", 'd', POPT_ARG_INT, NULL, 0, "Debug level - error:-2 .. debug:1-3"},
  { "logfile", 'l', POPT_ARG_STRING, NULL, 0, "File to log to"},
  { "version", 'v', 0, 0, VERSION_FLAG, "Print version information and exit" },
  { NULL, 0, 0, NULL, 0 }
};

static struct poptOption
phase2[] = {
  { "help", 'h', POPT_ARG_STRING | POPT_ARGFLAG_OPTIONAL, NULL, HELP_FLAG, "Show help", "[component]"},
  { NULL, '\0', POPT_ARG_INCLUDE_TABLE, NULL, 0},
  { NULL, '\0', POPT_ARG_INCLUDE_TABLE, NULL, 0},
  { "sender", '\0', POPT_ARG_STRING, NULL, 0},
  { "source", '\0', POPT_ARG_STRING, NULL, 0},
  { NULL, '\0', POPT_ARG_INCLUDE_TABLE, NULL, 0},
  { "debug-level", 'd', POPT_ARG_INT, NULL, 0, "Debug level - error:-2 .. debug: 1-3"},
  { "logfile", 'l', POPT_ARG_STRING, NULL, 0, "File to log to"},
  { "version", 'v', 0, 0, 'v', "Print version information and exit" },
  { "exit", '\0', POPT_ARG_NONE, NULL, 1, "Stop the generator and exit" },
  { "pause", '\0', POPT_ARG_NONE, NULL, 2, "pause the generator and exit" },
  { "resume", '\0', POPT_ARG_NONE, NULL, 3, "resume the generator" },
  { NULL, 0, 0, NULL, 0 }
};


/** General structure of an OTx application
 *
 * The major purpose of this function are:
 * - handle command-line inputs, before starting the program and at run-time.
 * - arrange wait, send, and stdin reading operations.
 *
 * It is designed as: First, parse all options in two phases.
 * After parsing the second level options, check the readiness of Source and Sender (if this is sender)
 * then initialize the port and set-up the scenario, and ready to start.

 * Here. we are going to have multiple streams in one OTG.
 * using multiple thread. the main thread is handling commands.
 * each stream is a seperate thread, by calling th stream_init functon, the stream is independently sending packets
 */
Application::Application(int argc, const char * argv[], const char* defLogFile)
{
  throw("Application::Application(argc, argv,...) is deprecated; use Application::Application(\"appname\", argc, argv,...) instead");
}

Application::Application(const char *appname, int argc, const char * argv[], const char* defLogFile, const char* applongname, const char* copyright)
{
  app_name_ = appname;
  app_long_name_ = applongname == NULL ? appname : applongname;
  copyright_ = copyright == NULL ? COPYRIGHT : copyright;

  loginfo("%s %s\n", app_long_name_, OTG2_VERSION);

  omlc_init(app_name_, &argc, argv, NULL);

  argc_ = argc;
  argv_ = argv;
  logdebug("Initialisation of the application\n");
  clockref_ = -1;

  component_name_ = NULL;

  log_level_ = O_LOG_INFO;
  logfile_name_ = defLogFile;

  stream_ = new Stream();
  logdebug("Initialisation of the application after Stream creation");

  phase1_ = phase1;
  phase1_[0].arg = &component_name_;
  phase1_[2].arg = &sender_name_;
  phase1_[3].arg = &source_name_;
  phase1_[4].arg = (void*)stream_->getOptions();
  phase1_[5].arg = &log_level_;
  phase1_[6].arg = &logfile_name_;
  logdebug("Initialisation of the application end of phase 1\n");
  phase2_ = phase2;
  phase2_[0].arg = &component_name_;
  phase2_[4].argDescrip = "FIXED";
  phase2_[5].arg = (void*)stream_->getOptions();
  phase2_[6].argDescrip = "FIXED";
  phase2_[7].argDescrip = "FIXED";
  phase2_[8].argDescrip = "FIXED";
  logdebug("Initialisation of the application end of phase 2\n");

}

Application::~Application()
{
  omlc_close();
}

/**  Parse options for phase 1: Protocol and Generator Type */
void
Application::parseOptionsPhase1()
{
  int rc;
  poptContext optCon = poptGetContext(NULL, argc_, argv_, phase1_, 0);
  while ((rc = poptGetNextOpt(optCon)) != -1) {
    switch (rc) {
    case HELP_FLAG:
      showHelp(optCon, component_name_);
      exit(0);
    case USAGE_FLAG:
      poptPrintUsage(optCon, stdout, 0);
      exit(0);
    case VERSION_FLAG:
      printVersion();
      exit(0);
    case POPT_ERROR_BADOPT:
      // ignore here
      break;
    default:
      logerror("Unknown flag operation %d\n", rc);
      exit(-1);
    }
  }
  poptFreeContext(optCon);
  o_set_log_file((char*)logfile_name_);
  o_set_log_level(log_level_);

}

void
Application::showHelp(poptContext optCon, char* component_name)
{
  if (component_name == NULL) {
    // common help
    poptPrintHelp(optCon, stdout, 0);
  } else {
    // help about a component. Let's find it.
    const struct poptOption* opts= getComponentOptions(component_name);
    if (opts == NULL) {
      logwarn("Unknown component '%s'\n", component_name);
    } else {
      poptContext ctxt =
        poptGetContext(NULL, argc_, argv_, opts, POPT_CONTEXT_NO_EXEC);
      poptPrintHelp(ctxt, stdout, 0);
    }
  }
}

/** Parse options in second Phase.
 * Check all port and generator parameters except "type" which has already been parsed.
 * in Phase I.
 */
void
Application::parseOptionsPhase2()
{
  int rc;
  poptContext optCon = poptGetContext(NULL, argc_, argv_, phase2_, 0);
  while ((rc = poptGetNextOpt(optCon)) >= 0);
  if (rc < -1) {
    logerror("%s (%s)\n", poptBadOption(optCon, POPT_BADOPTION_NOALIAS), poptStrerror(rc));
    poptPrintUsage(optCon, stderr, 0);
    exit(-1);
  }

  poptFreeContext(optCon);
  return;
}

int
Application::parseRuntimeOptions(char * msg)
{
  int argc;
  const char** argv;
  char strin[MAX_INPUT_SIZE];

  if (*msg == '\0') return -1;
  if (*msg != '-') {
    // allow for commands without leading --
    strcpy(strin + 2, msg);
    strin[0] = strin[1] = '-';
    msg = strin;
  }
  poptParseArgvString(msg, &argc, &argv);
  //  char* component_name;
  //  phase2_[0].arg = &component_name;
  poptContext optCon = poptGetContext(NULL, argc, argv, phase2_, POPT_CONTEXT_KEEP_FIRST);

  int rc;
  while ((rc = poptGetNextOpt(optCon)) > 0) {
    switch(rc) {
    case 1: // Stop
      stream_->exitStream();
      exit(0);  //exit terminate process and all its threads
    case 2:
      stream_->pauseStream();
      break;
    case 3:
      stream_->resumeStream();
      break;
    case HELP_FLAG:
      showHelp(optCon, component_name_);
      break;
    case VERSION_FLAG:
      printVersion();
      break;
    }
  }
  if (rc < -1) {
    logerror("%s (%s)\n", poptBadOption(optCon, POPT_BADOPTION_NOALIAS), poptStrerror(rc));
  }
  poptFreeContext(optCon);
  dynamic_cast<IComponent*>(sender_)->update();
  dynamic_cast<IComponent*>(source_)->update();
  dynamic_cast<IComponent*>(stream_)->update();
  return rc;
}

void
Application::run()
{

  parseOptionsPhase1();

  source_ = createSource(source_name_);

  if (source_ == NULL) {
    logerror("Unknown source '%s'\n", source_name_);
    exit(-1);
  }

  sender_ = createSender(sender_name_);

  if (sender_ == NULL) {
    logerror("Unknown sender '%s'\n", sender_name_);
    exit(-1);
  }
  logdebug("Sender/Source created\n");

  phase2_[1].arg = (void*)sender_->getConfigurable()->getOptions();
  phase2_[2].arg = (void*)source_->getConfigurable()->getOptions();
  parseOptionsPhase2();
  logdebug("Parsing phase 2 finished\n");

  source_->getConfigurable()->init();
  sender_->getConfigurable()->init();
  stream_->setSource(source_);
  stream_->setSender(sender_);

  logdebug("Stream configured\n");

  omlc_start();

  stream_->run();

  char msg[MAX_INPUT_SIZE];
  while (1) {
    cin.getline(msg, MAX_INPUT_SIZE );
    parseRuntimeOptions(msg);
  }
}

void
Application::printVersion()
{
  loginfo("%s %s\n", app_long_name_, OTG2_VERSION);
  loginfo("%s\n", copyright_);
}

void
Application::setSenderInfo(const char* longName, char shortName, const char* descrip, const char* argDescrip)
{
  struct poptOption& p1 = phase1_[2];
  struct poptOption& p2 = phase2_[3];

  p1.longName = p2.longName = longName;
  p1.shortName = p2.shortName = shortName;
  p1.descrip = p2.descrip = descrip;
  p1.argDescrip = p2.argDescrip = argDescrip;
}

void
Application::setSourceInfo(const char* longName, char shortName, const char* descrip, const char* argDescrip)
{
  struct poptOption& p1 = phase1_[3];
  struct poptOption& p2 = phase2_[4];

  p1.longName = p2.longName = longName;
  p1.shortName = p2.shortName = shortName;
  p1.descrip = p2.descrip = descrip;
  p1.argDescrip = p2.argDescrip = argDescrip;
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
