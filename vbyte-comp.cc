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
#include "vbyte.h"

#include <assert.h>
#include <string.h>

#if defined(_MSC_VER) && _MSC_VER < 1600
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
#else
#  include <stdint.h>
#endif

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

