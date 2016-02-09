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
#include <x86intrin.h>
#include <tmmintrin.h>

#if defined(_MSC_VER) && _MSC_VER < 1600
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef signed char int8_t;
#else
#  include <stdint.h>
#endif

#include "vbyte-internal.h"

struct lookup_line_t
{
  char num_ints;
  char num_bytes;
  char ints[16];
};

static lookup_line_t lookup_table[] = {
#  include "vbyte-gen.i"
};

static inline void
create_pshufb_masks(lookup_line_t *line, __m128i *masks)
{
  uint8_t ch[16];
  int b = 0;

  for (int i = 0; i < line->num_ints; i++) {
    switch (line->ints[i]) {
      case 1:
        ch[b++] =    i;
        ch[b++] = 0x80;
        ch[b++] = 0x80;
        ch[b++] = 0x80;
        break;
      case 2:
        ch[b++] = i + 1;
        ch[b++] =    i;
        ch[b++] = 0x80;
        ch[b++] = 0x80;
        break;
      case 3:
        ch[b++] = 0x80;
        ch[b++] =    2;
        ch[b++] =    1;
        ch[b++] =    0;
        break;
      case 4:
        ch[b++] =    3;
        ch[b++] =    2;
        ch[b++] =    1;
        ch[b++] =    0;
        break;
      case 5:
        assert(!"???");
        break;
    }
    if (b == 16) {
      b = 0;
      *masks = _mm_set_epi8(ch[15], ch[14], ch[13], ch[12],
                            ch[11], ch[10],  ch[9],  ch[8],
                             ch[7],  ch[6],  ch[5],  ch[4],
                             ch[3],  ch[2],  ch[1],  ch[0]);
      masks++;
    }
  }
  (void)line;
  (void)masks;
}

uint32_t
vbyte_uncompress_scalar4(const uint8_t *in, uint32_t *out, uint32_t length)
{
  const uint8_t *initial_in = in;
  uint32_t remaining = length;

  while (remaining >= 16) {
    // extract the MSBs of the next 16 bytes
    __m128i in128 = _mm_lddqu_si128((const __m128i *)in);
    uint16_t mask = (uint16_t)_mm_movemask_epi8(in128);

    // use a lookup table to get information about the compressed integers
    lookup_line_t *line = &lookup_table[mask];

    // create masks for pshufb
    __m128i masks[4];
    create_pshufb_masks(line, &masks[0]);

    // shuffle the integers around, then shift the bytes to remove
    // the MSBs
    // TODO what happens if an integer is encoded in 5 bytes?
    __m128i *out128 = (__m128i *)out;
    if (remaining >= 4) {
      __m128i v = _mm_shuffle_epi8(in128, masks[0]);
      _mm_storeu_si128(&out128[0], v);
    }
    if (remaining >= 8) {
      __m128i v = _mm_shuffle_epi8(in128, masks[1]);
      _mm_storeu_si128(&out128[1], v);
    }
    if (remaining >= 12) {
      __m128i v = _mm_shuffle_epi8(in128, masks[2]);
      _mm_storeu_si128(&out128[2], v);
    }
    if (remaining >= 16) {
      __m128i v = _mm_shuffle_epi8(in128, masks[3]);
      _mm_storeu_si128(&out128[3], v);
    }

    // now shift the bits to remove the MSBs
    uint32_t v;
    for (int j = 0; j < line->num_ints; ++j, ++out) {
      switch (line->ints[j]) {
        case 1:
          in += 1;
          break;
        case 2:
          v = *out;
          *out =   ((v & 0x7F00u) >> 1)
                 |  (v & 0x7Fu);
          in += 2;
          break;
        case 3:
          v = *out;
          *out =   ((v & 0x7F0000u) >> 2)
                 | ((v & 0x7F00u) >> 1)
                 |  (v & 0x7Fu);
          in += 3;
          break;
        case 4:
          v = *out;
          *out =   ((v & 0x7F000000u) >> 3)
                 | ((v & 0x7F0000u) >> 2)
                 | ((v & 0x7F00u) >> 1)
                 |  (v & 0x7Fu);
          in += 4;
          break;
        default:
          v = *out;
          assert(line->ints[j] == 5);
          *out =   ((v & 0x7F00000000u) >> 4)
                 | ((v & 0x7F000000u) >> 3)
                 | ((v & 0x7F0000u) >> 2)
                 | ((v & 0x7F00u) >> 1)
                 |  (v & 0x7Fu);
          in += 5;
          break;
      }
    }

    remaining -= line->num_ints;
  }

  // pick up the remaining values
  while (remaining > 0) {
    in += read_int(in, out);
    out++;
    remaining--;
  }
  return in - initial_in;
}


