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
#include <x86intrin.h>

#include "vbyte.h"

namespace vbyte {

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
read_int(const uint8_t *in, uint64_t *out)
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
  *out = ((uint64_t)(in[4] & 0x7Fu) << 28) | *out;
  if (in[4] < 128)
    return 5;
  *out = ((uint64_t)(in[5] & 0x7Fu) << 35) | *out;
  if (in[5] < 128)
    return 6;
  *out = ((uint64_t)(in[6] & 0x7Fu) << 42) | *out;
  if (in[6] < 128)
    return 7;
  *out = ((uint64_t)(in[7] & 0x7Fu) << 49) | *out;
  if (in[7] < 128)
    return 8;
  *out = ((uint64_t)(in[8] & 0x7Fu) << 56) | *out;
  if (in[8] < 128)
    return 9;
  *out = ((uint64_t)(in[9] & 0x7Fu) << 63) | *out;
  return 10;
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

static inline int
write_int(uint8_t *p, uint64_t value)
{
  if (value < (1lu << 7)) {
    *p = value & 0x7Fu;
    return 1;
  }
  if (value < (1lu << 14)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 7;
    return 2;
  }
  if (value < (1lu << 21)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 14;
    return 3;
  }
  if (value < (1lu << 28)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 14) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 21;
    return 4;
  }
  if (value < (1lu << 35)) {
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
  if (value < (1lu << 42)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 14) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 21) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 28) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 35;
    return 6;
  }
  if (value < (1lu << 49)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 14) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 21) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 28) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 35) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 42;
    return 7;
  }
  if (value < (1lu << 56)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 14) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 21) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 28) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 35) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 42) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 49;
    return 8;
  }
  if (value < (1lu << 63)) {
    *p = (value & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 7) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 14) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 21) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 28) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 35) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 42) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 49) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 56;
    return 9;
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
    *p = ((value >> 28) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 35) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 42) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 49) & 0x7Fu) | (1u << 7);
    ++p;
    *p = ((value >> 56) & 0x7Fu) | (1u << 7);
    ++p;
    *p = value >> 63;
    return 10;
  }
}

template<typename T>
static inline size_t
compress(const T *in, uint8_t *out, size_t length)
{
  const uint8_t *initial_out = out;
  const T *end = in + length;

  while (in < end) {
    out += write_int(out, *in);
    ++in;
  }
  return out - initial_out;
}

template<typename T>
static inline size_t
uncompress(const uint8_t *in, T *out, size_t length)
{
  const uint8_t *initial_in = in;

  for (size_t i = 0; i < length; i++) {
    in += read_int(in, out);
    ++out;
  }
  return in - initial_in;
}

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
};

static lookup_line_t lookup_table[] = {
#  include "vbyte-gen.i"
};

template<typename T>
static inline T
select(const uint8_t *in, size_t size, size_t index)
{
  int remaining = (int)index;
  T rv = 0;

  while (remaining >= 0) {
    if (size - remaining >= 16) {
      // fetch the mask of the next 16 bytes
      uint16_t mask = extract_16_msbs(in);

      // get the info from the lookup table
      lookup_line_t *line = &lookup_table[mask];

      // skip this block if index is too high
      if (remaining > line->num_ints) {
        remaining -= line->num_ints;
        in += line->num_bytes;
        continue;
      }
    }

    // otherwise pick the requested integer
    while (remaining >= 0) {
      in += read_int(in, &rv);
      remaining--;
    }
  }

  return rv;
}

template<typename T>
static inline size_t
linear_search(const uint8_t *in, size_t length, T value, T *presult)
{
  T v;

  for (size_t i = 0; i < length; i++) {
    in += read_int(in, &v);
    if (v == value) {
      *presult = v;
      return i;
    }
  }
  return length;
}

template<typename T>
static inline size_t
sorted_search(const uint8_t *in, size_t length, T value, T *presult)
{
  int remaining = (int)length;
  T next;

  while (remaining >= 0) {
    if (length - remaining > 16) {
      // fetch the mask of the next 16 bytes
      uint16_t mask = extract_16_msbs(in);

      // get the info from the lookup table
      lookup_line_t *line = &lookup_table[mask];

      // read the value in the next block
      read_int(in + line->num_bytes, &next);

      // skip this block if the value is too high
      if (value > next) {
        remaining -= line->num_ints;
        in += line->num_bytes;
        continue;
      }
    }

    while (remaining >= 0) {
      in += read_int(in, &next);
      if (next == value) {
        *presult = next;
        return length - remaining;
      }
      remaining--;
    }
  }
  return length;
}

} // namespace vbyte


size_t
vbyte_compress32(const uint32_t *in, uint8_t *out, size_t length)
{
  return vbyte::compress(in, out, length);
}

size_t
vbyte_compress64(const uint64_t *in, uint8_t *out, size_t length)
{
  return vbyte::compress(in, out, length);
}

size_t
vbyte_uncompress32(const uint8_t *in, uint32_t *out, size_t length)
{
  return vbyte::uncompress(in, out, length);
}

size_t
vbyte_uncompress64(const uint8_t *in, uint64_t *out, size_t length)
{
  return vbyte::uncompress(in, out, length);
}

uint32_t
vbyte_select32(const uint8_t *in, size_t size, size_t index)
{
  return vbyte::select<uint32_t>(in, size, index);
}

uint64_t
vbyte_select64(const uint8_t *in, size_t size, size_t index)
{
  return vbyte::select<uint64_t>(in, size, index);
}

size_t
vbyte_linear_search32(const uint8_t *in, size_t length, uint32_t value,
                uint32_t *presult)
{
  return vbyte::linear_search(in, length, value, presult);
}

size_t
vbyte_linear_search64(const uint8_t *in, size_t length, uint64_t value,
                uint64_t *presult)
{
  return vbyte::linear_search(in, length, value, presult);
}

size_t
vbyte_sorted_search32(const uint8_t *in, size_t length, uint32_t value,
                uint32_t *presult)
{
  return vbyte::sorted_search(in, length, value, presult);
}

size_t
vbyte_sorted_search64(const uint8_t *in, size_t length, uint64_t value,
                uint64_t *presult)
{
  return vbyte::sorted_search(in, length, value, presult);
}
