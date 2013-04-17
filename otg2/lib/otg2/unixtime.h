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

#ifndef UNIX_TIME_PORT_H
#define UNIX_TIME_PORT_H

#include <time.h>
#include <sys/time.h>
#include <unistd.h>

/** This class wraps some simple timing functons of Unix(Linux) System.
 * In general, UnixTime class provide a wall clock of the program and it can be paused and resumed.
 * The timing used in UNIX/Linux system is not accurate.
 * we wrap basic timing operations in this class, for example we are going to map
 * a timeval structure to a double absolute time value.
 */
class UnixTime{
public:
  UnixTime(int externalcaliber = -1 );
  void setOrigin();
  /** Function to get the original time indication in system clock. */
  inline double getOrigin(){return origin_;}
  double getAbsoluteTime();
  inline void setAbsoluteOrigin(int hours){if (hours>0)abs_origin_ = hours*3600.0;  };
  double getCurrentTime();
  bool  pauseClock();
  bool  resumeClock();
  /** Function to shift the orginal time. */
  inline void shiftOrigin(double shifttime){ origin_ += shifttime;}
  void waitAt(double timestamp);

private:
  /** original time. timing are in the unit of seconds of system clock */
  double  origin_;
  /** starting time offset to calculate absolute time */
  double  abs_origin_;
  /** The clock time (relative time) when the clock is paused */
  double  pauseinstant_;
  /** A flag to indicate if the clock is paused now */
  bool    paused_;
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
