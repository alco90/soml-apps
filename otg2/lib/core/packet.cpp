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

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

#include "otg2/compat.h"
#include "otg2/packet.h"

/** Default Packet constructor.
 * It is used only for generator to produce the very first empty packet.
 * this empty packet will be used again and agian for every packet generated.
 * A buffer with length 512 is initialzied for packet payload. If it is smaller, a new buffer will create to replace this.
 * set length to zero is dangerous, because that means a null pointer of payload_ is initialized...
 * \see setPayloadSize
 */
Packet::Packet(int buffer_length, UnixTime* clock)
{
  reset();
  length_ = buffer_length;
  payload_ = new char[buffer_length];
  clock_ = clock;
}

/** Set packet size.
 * As packet already has a defult buffer,
 * there are two ways to determine the payload
 *  - set size only, let the payload be as it is --> SetPayloadSize
 *   -# if necessary, adjust the payload buffer size.
 *  - set size and also fill the payload with speficic data (e.g.  for Audio and Video Applications...)
 * \see  fillPayload
 */
void
Packet::reset()
{
  ts_ = 0;
  tx_time_ = 0;
  size_ = 0;
  flowid_ = 0;
  seqnum_ = 0;
  offset_ = 0;
}

void
Packet::setPayloadSize(int size)
{
  size_ =  size;
  if (size > length_) {
    if (payload_ != NULL) delete [] payload_;
    length_ = (int)(1.5 * size);
    payload_ =  new char[length_];
  }
}

/** A function to fill payload.
 *  user can specify the content of payload for applications like audio/video playback...
 */
int
Packet::fillPayload(int size, char *inputstream)
{
  setPayloadSize(size);
  if (memcpy((char *)payload_, (char *)inputstream,  size) == NULL) {
    throw "Fill payload Failed";
  }
  return 0;
}

void
Packet::stampPacket(char version) {
  char* p = getBufferPtr(32, 0);
  *(p++) = SYNC_BYTE;
  *(p++) = SYNC_BYTE;
  *(p++) = version;
  offset_ = 3;
}

int
Packet::stampInt64Val(int64_t val, int offset)
{
  uint64_t nv = otg_htonll((uint64_t)val);

  if (offset < 0) {
    offset = offset_;
    offset_ += sizeof(uint64_t);
  }

  char* p = getBufferPtr(sizeof(uint64_t) + offset, 0) + offset;
  *(p++) = (char)((nv >> 56) & 0xff);
  *(p++) = (char)((nv >> 48) & 0xff);
  *(p++) = (char)((nv >> 40) & 0xff);
  *(p++) = (char)((nv >> 32) & 0xff);
  *(p++) = (char)((nv >> 24) & 0xff);
  *(p++) = (char)((nv >> 16) & 0xff);
  *(p++) = (char)((nv >> 8) & 0xff);
  *(p++) = (char)(nv & 0xff);


  return offset;
}

int
Packet::stampInt32Val(int32_t val, int offset)
{
  uint32_t nv = htonl((uint32_t)val);

  if (offset < 0) {
    offset = offset_;
    offset_ += sizeof(uint32_t);
  }

  char* p = getBufferPtr(sizeof(uint32_t) + offset, 0) + offset;
  *(p++) = (char)((nv >> 24) & 0xff);
  *(p++) = (char)((nv >> 16) & 0xff);
  *(p++) = (char)((nv >> 8) & 0xff);
  *(p++) = (char)(nv & 0xff);

  return offset;
}

int
Packet::stampInt16Val(int16_t val, int offset)
{
  uint16_t nv = htons((uint16_t)val);

  if (offset < 0) {
    offset = offset_;
    offset_ += sizeof(uint16_t);
  }

  char* p = getBufferPtr(sizeof(uint16_t) + offset_, 0) + offset_;
  *(p++) = (char)((nv >> 8) & 0xff);
  *(p++) = (char)(nv & 0xff);

  return offset;
}

/** A function to recover some packet information from the payload of received packet
 * As the sending application might stamp some information in the payload,
 * so be careful to use this function
 */
char
Packet::checkStamp()
{
  offset_ = 0;
  if (size_ < 3) { return -1; } // not the best solution

  char* p = payload_;
  if (*(p++) == (char)SYNC_BYTE && *(p++) == (char)SYNC_BYTE) {
    offset_ = 3;
    return *p;
  }
  return -1;
}

int64_t
Packet::extractInt64Val()
{
  if (size_ < offset_ + (int)sizeof(uint64_t)) { return 0; } // not the best solution

  unsigned char* p = (unsigned char*)payload_ + offset_;
  uint64_t nv = 0;
  unsigned int i;

  for (i=0;i<sizeof(uint64_t);i++) {
    nv <<= 8;
    nv += *p++;
  }
  uint64_t hv = otg_ntohll(nv);
  offset_ += sizeof(uint64_t);
  return (int64_t)(hv);
}

int32_t
Packet::extractInt32Val()
{
  if (size_ < offset_ + (int)sizeof(uint32_t)) { return 0; } // not the best solution

  char* p = payload_ + offset_;
  uint32_t nv = *p++ << 24;
  nv += *p++ << 16;
  nv += *p++ << 8;
  nv += *p++;
  uint32_t hv = ntohl(nv);
  offset_ += sizeof(uint32_t);
  return (int32_t)(hv);
}

int16_t
Packet::extractInt16Val()
{
  if (size_ < offset_ + (int)sizeof(uint16_t)) { return 0; } // not the best solution

  char* p = payload_ + offset_;
  uint16_t nv = *p++ << 8;
  nv += *p++;
  uint16_t hv = ntohs(nv);
  offset_ += sizeof(uint16_t);
  return (int16_t)(hv);
}

char*
Packet::getBufferPtr(int minLength, int maintainContent)
{
  if (length_ >= minLength) { return payload_; }

  // Need to resize
  if (maintainContent) {
    throw "Not implemented";
  }

  if (length_ > 0) { delete payload_; }
  payload_ = new char[minLength];
  length_ = minLength;

  return payload_;
}

void
Packet::setTimeStamp(double time) {
  if (time <= 0 && clock_ != NULL) {
    // set to current time
    ts_ = clock_->getAbsoluteTime() * 1e3;
  } else {
    ts_ = time;
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
