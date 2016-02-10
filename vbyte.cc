/*
 * Copyright (C) 2005-2016 Christoph Rupp (chris@crupp.de).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <assert.h>
#include <string.h>

#if defined(_MSC_VER) && _MSC_VER < 1600
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
#else
#  include <stdint.h>
#endif

#include "vbyte.h"

static inline int
read_int(const uint8_t *in, uint32_t *out)
{
  *out = in[0] & 0x7Fu;
  if (in[0] < 128)
    return 1;
  *out = ((in[1] & 0x7Fu) << 7) | *out;
  if (in[1] < 128)
    return 2;
  *out = ((in[2] & 0x7Fu) << 14) | *out;
  if (in[2] < 128)
    return 3;
  *out = ((in[3] & 0x7Fu) << 21) | *out;
  if (in[3] < 128)
    return 4;
  *out = ((in[4] & 0x7Fu) << 28) | *out;
  return 5;
}


static inline int
write_int(uint8_t *p, uint32_t value)
{
  if (value < (1u << 7)) {
    *p = value & 0x7Fu;
    return 1;
  }
  if (value < (1u << 14)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 7;
    return 2;
  }
  if (value < (1u << 21)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 14;
    return 3;
  }
  if (value < (1u << 28)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 14) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 21;
    return 4;
  }
  else {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 14) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 21) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 28;
    return 5;
  }
}

uint32_t
vbyte_compress(const uint32_t *in, uint8_t *out, uint32_t length)
{
  const uint8_t *initial_out = out;
  const uint32_t *end = in + length;

  while (in < end) {
    out += write_int(out, *in);
    ++in;
  }
  return out - initial_out;
}

uint32_t
vbyte_uncompress(const uint8_t *in, uint32_t *out, uint32_t length)
{
  const uint8_t *initial_in = in;

  for (uint32_t i = 0; i < length; i++) {
    in += read_int(in, out);
    ++out;
  }
  return in - initial_in;
}

/*
static uint16_t
extract_16_msbs(const uint8_t *in)
{
  __m128i v = _mm_lddqu_si128((const __m128i *)in);
  return _mm_movemask_epi8(v);
}

struct lookup_line_t
{
  char num_ints;
  char num_bytes;
  char ints[16];
};

static lookup_line_t lookup_table[] = {
#  include "vbyte-gen.i"
};

*/
