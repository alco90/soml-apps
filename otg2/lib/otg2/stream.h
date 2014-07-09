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

#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>
#include <oml2/omlc.h>
#include "otg2/unixtime.h"
#include "otg2/source.h"
#include "otg2/packet.h"
#include "otg2/sender.h"
#include "otg2/component.h"

class Stream : public Component
{
public:
  Stream(uint64_t id = omlc_guid_generate());

  void setSource(ISource* source) { source_ = source; }
  void setSender(Sender* sender) { sender_ = sender; }
  void run();

  void startStream();
  void pauseStream();
  void resumeStream();
  void exitStream();

  void init() {}

  // should only be used internally
  void _run();

protected:
  inline const char* getNamespace() { return "flow"; }

  void defOpts();

  /**  number of packets sent or received  by this stream */
  unsigned long seqno_;
  /**  the generator to feed packets to this stream */
  ISource*      source_;
  Sender*       sender_; /// the port for sending packets out.
  uint64_t      streamid_; /// stream identifier.

  /** Contorl the timing of stream */
  UnixTime streamclock_;

  /**  indicate whether stream is paused */
  bool paused_;

  /**  set to true while stream is active */
  int active_;
  pthread_t thread_;
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
