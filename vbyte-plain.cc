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

#include "vbyte-internal.h"

uint32_t
vbyte_uncompress_plain(const uint8_t *in, uint32_t *out, uint32_t length)
{
  const uint8_t *initial_in = in;

  for (uint32_t i = 0; i < length; i++) {
    in += read_int(in, out);
    ++out;
  }
  return in - initial_in;
}

