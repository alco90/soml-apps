/*
 * Copyright 2014 National ICT Australia (NICTA), Olivier Mehani
 *
 * This software may be used and distributed solely under the terms of
 * the MIT license (License).  You should find a copy of the License in
 * COPYING or at http://opensource.org/licenses/MIT. By downloading or
 * using this software you accept the terms and the liability disclaimer
 * in the License.
 */
#include <inttypes.h>
#include <arpa/inet.h>

/** 64-bit byteorder(3) functions
 * FIXME: Detect if available systemwide or enable, at configure time
 * */
inline uint64_t otg_ntohll(uint64_t val)
{
#ifdef WORDS_BIGENDIAN
    return val;
#else
    return (((uint64_t)ntohl(val)) << 32) | ntohl(val >> 32);
#endif
}

inline uint64_t otg_htonll(uint64_t val)
{
#ifdef WORDS_BIGENDIAN
    return val;
#else
    return (((uint64_t)htonl(val)) << 32) | htonl(val >> 32);
#endif
}
