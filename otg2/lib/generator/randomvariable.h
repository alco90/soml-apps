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

#ifndef RAMD_H
#define RAND_H

#include <math.h>
#include <stdlib.h>

class RandomVariable
{
public:
  virtual  ~RandomVariable(){}
  virtual double getSample()=0;

protected:
  double mean_;
  double variance_;
};

/** Generate unifmor randome variable in [0,1)
 * Mean is 0.5
 */
class UniformRandomVariable : public RandomVariable
{
public:
  double getSample();
};

class ExponentialRandomVariable: public RandomVariable
{
public:
  void setMean(double mean);
  double getSample();
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
