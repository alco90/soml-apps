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

#include "otg2/unixtime.h"

using namespace std;

/** Constructor */
UnixTime::UnixTime(int externalcaliber)
{
  setOrigin();
  if (externalcaliber == -1 ) {
    time_t seconds;
    seconds = time (NULL);
    int days= seconds/(3600*24);// since Jan,1, 1970
    setAbsoluteOrigin(days*24);
  } else {
    setAbsoluteOrigin(externalcaliber);
  }

  paused_ =  false;
}

/** Set the orginal Starting time of clock.
 * this is the zero point of a clock.
 */
void
UnixTime::setOrigin()
{
  struct timeval tp;
  gettimeofday(&tp, NULL);
  origin_ = tp.tv_sec + tp.tv_usec/1e6;
}

/** Function to get Absolute time value */
double
UnixTime::getAbsoluteTime()
{
  struct timeval tp;
  gettimeofday(&tp, NULL);
  logdebug("Absolute time: %d\n", tp.tv_sec + tp.tv_usec/1e6 - abs_origin_);
  return (tp.tv_sec + tp.tv_usec/1e6 - abs_origin_);
}

/** Function to Get Current relative time refer to the origin time.
 * \return clocktime.if the clock is paused, the pausing time instatnt is returned
 */
double
UnixTime::getCurrentTime()
{
  if (paused_) { return pauseinstant_; }
  //return (getAbsoluteTime() - origin_ );
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return (tp.tv_sec + tp.tv_usec/1e6 - origin_);
}

/** Function to pause the clock.
 * First,  record the time of now.
 * Second, set flag as paused.
 * The order of above two operations cannot be reversed.
 */
bool
UnixTime::pauseClock()
{
  if (paused_ == true) { return false; }
  pauseinstant_ = getCurrentTime();
  paused_ = true;
  return true;
}

/** Function to resume the clock.
 * As clock is paused for some interval, but the real-world (system) clock never paused, we need to
 * shift the orginal time to a later time instant.
 */
bool UnixTime::resumeClock()
{
  if (!paused_) { return false; }
  paused_ = false;
  shiftOrigin( getCurrentTime() - pauseinstant_ );
  return true;
}


/** Function to wait till a specified time */
void UnixTime::waitAt(double timestamp)
{
  double x = getCurrentTime();	
  if (timestamp > x ) {
    unsigned long y = (unsigned long)(1e6*(timestamp-x));
    usleep(y);
  }
}

/*
 Local Variables:
 mode: C
 tab-width: 2
 indent-tabs-mode: nil
 End:
 vim: sw=2:sts=2:expandtab
*/
