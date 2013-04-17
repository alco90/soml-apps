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

#ifndef OTG_PACKET_H
#define OTG_PACKET_H

#include <string.h>
#include "otg2/unixtime.h"

#define MAX_HOSTNAME_LENGTH 256

#define SYNC_BYTE 0xAA

/** Packet is an entity given by generator.
 *
 * This packet class only carries payload information and a sending timestamp,
 * not any socket address information. Also, the packet would carry all
 * measuremnts related to this packet with it.
 */
class Packet
{

public:
  /**  Default payload size is 512 Bytes */
  static const int DEFAULT_PAYLOAD_SIZE = 512;

  //Packet();
  Packet(int buffer_length = 512, UnixTime* clock = NULL);

  /** Reset all infomation in this packet */
  void reset();

  int fillPayload(int size, char *inputstream);

  inline char* getPayload() { return payload_;}
  void setPayloadSize(int size);

  /** get the size of packet buffer where payload is stored */
  inline int getBufferSize(){return length_;}

  /** Returns a ptr to the packets buffer which is ensured to be at least size long.
   *
   * If maintainContent is set, copy current content in case the buffer needs
   * to be resized.
   */
  char* getBufferPtr(int size, int maintainContent = 0);

  /** get the size of the packet */
  inline int getPayloadSize(){ return size_;}

  /** Timestamp in seconds */
  void setTimeStamp(double stamp);
  inline double getTimeStamp()  {return ts_; }

  /** Time when to send the packet */
  inline void setTxTime(double time) {tx_time_ = time;}
  inline double getTxTime()  {return tx_time_;}

  inline short getFlowId()       {return flowid_;}
  inline void setFlowId(short i) {flowid_ = i;}

  inline unsigned long getSequenceNum() {return seqnum_;}
  inline void setSequenceNum(unsigned long i) {seqnum_ = i;}

  /** Mark the packet with a leading pattern followed by a version byte.
   *
   * Additional numbers can be added with ...
   */
  void stampPacket(char version);
  char checkStamp();

  /** Write a value into the payload at a certain offset.
   * If the offset is < 0, the value will be appended to
   * previously stamped values. In both cases, the offset
   * will be returned.
   */
  int stampLongVal(long val, int offset = -1);
  int stampShortVal(short val, int offset = -1);

  /** Extract values stored in the header (of payload). */
  long extractLongVal();
  short extractShortVal();


private:
  /** Expected Deliver Timestamp.
   * Expected time to send this paket.absolute time in seconds.
   * the actual sending  time canot as precise as this double-precision variable.
   * this is a relevant time, refer to the time when TG is started
   */
  double ts_;
  double tx_time_;

  /**  Packet length in Bytes */
  int size_;
  /**  Maximum allocated Size of Payload buffer. it is no less than the size_ */
  int length_;
  /**  Payload Pointer. */
  char* payload_;

  short flowid_;
  unsigned long seqnum_;

  int offset_;

  /**  Source for timestamping */
  UnixTime* clock_;


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
