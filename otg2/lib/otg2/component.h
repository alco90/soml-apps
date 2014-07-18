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
/** \file component.h
 * \brief This defines the parent class for all configurable objects in OTG.
 *
 * It primarily provides support
 * for POPT to configure an object from command
 * line options.
 */

#ifndef OTG2_COMPONENT_H_
#define OTG2_COMPONENT_H_

#include <popt.h>
#include <string>

class IComponent
{
public:
  virtual ~IComponent() {};

  /** Function to initialize the component.
   * Can only be called once and should be
   * called after options have been set.
   */
  virtual void init() = 0;

  /** Function to inform component that some of its options have been changed. */
  virtual void
    update() = 0;

  /** Function to get Command line Options */
  virtual const struct poptOption* getOptions() = 0;
};

class Component : public IComponent
{
public:
  Component();

  /** Function to get Command line Options */
  const struct poptOption* getOptions();

  void update();

protected:
  virtual const char* getNamespace();

  inline virtual void defOpts() {}

  void defOpt(const char * longName = NULL, int argInfo = 0, void* arg = 0, const char* descrip = NULL, const char * argDescrip = NULL);

  char* namespace_;

private:
  struct poptOption* opts_;
  int opts_length_;
  int opts_count_;

};

#endif /* OTG2_COMPONENT_H_*/

/*
   Local Variables:
mode: C
tab-width: 2
indent-tabs-mode: nil
End:
vim: sw=2:sts=2:expandtab
*/
