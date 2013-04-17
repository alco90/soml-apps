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

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <ocomm/o_log.h>

#include "randomvariable.h"

double
UniformRandomVariable::getSample()
{
  double  x;
  // Set evil seed (initial seed)
  srand( (unsigned)time( NULL ) );
  x = (double) rand()/RAND_MAX;
  logdebug("Uniform random number: %f\n", x);
  return x;
}

void
ExponentialRandomVariable::setMean(double mean)
{
  mean_ = mean;
}

double
ExponentialRandomVariable::getSample()
{
  UniformRandomVariable x;
  //FIXME Need some code to calculate based on mean_
  return (-mean_*log(x.getSample()));
  //assume the pdf is 1/sigma^2*exp{-x/sigma^2} mean= sigma^2
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
