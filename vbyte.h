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

/**
 * A fast implementation for the Variable Byte encoding.
 *
 * See the README.md file for more information, example code and references.
 *
 * Feel free to send comments/questions to chris@crupp.de. I am available
 * for consulting.
 */

#ifndef VBYTE_H_ee452711_c856_416d_82f4_e12eef8a49be
#define VBYTE_H_ee452711_c856_416d_82f4_e12eef8a49be

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compresses an unsorted sequence of |length| ints at |in| and stores the
 * result in |out|.
 */
extern uint32_t
vbyte_compress(const uint32_t *in, uint8_t *out, uint32_t length);

/**
 * Uncompresses a sequence of |length| ints at |in| and stores the
 * result in |out|.
 *
 * Returns the number of compressed bytes processed.
 */
extern uint32_t
vbyte_uncompress_plain(const uint8_t *in, uint32_t *out, uint32_t length);

/* slow mask, slow calculation of lookup table entries */
extern uint32_t
vbyte_uncompress_scalar1(const uint8_t *in, uint32_t *out, uint32_t length);

/* slow mask, huge lookup table */
extern uint32_t
vbyte_uncompress_scalar2(const uint8_t *in, uint32_t *out, uint32_t length);

/* fast mask, huge lookup table */
extern uint32_t
vbyte_uncompress_scalar3(const uint8_t *in, uint32_t *out, uint32_t length);

/* fast mask, huge lookup table, pshufb to rearrange the data */
/* broken, and terribly slow, therefore I won't bother to fix it */
extern uint32_t
vbyte_uncompress_scalar4(const uint8_t *in, uint32_t *out, uint32_t length);

/* fast mask, huge lookup table, fast code path if mask == 0 */
extern uint32_t
vbyte_uncompress_scalar5(const uint8_t *in, uint32_t *out, uint32_t length);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VBYTE_H_ee452711_c856_416d_82f4_e12eef8a49be */
