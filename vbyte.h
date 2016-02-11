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
extern size_t
vbyte_compress32(const uint32_t *in, uint8_t *out, size_t length);
extern size_t
vbyte_compress64(const uint64_t *in, uint8_t *out, size_t length);

/**
 * Uncompresses a sequence of |length| ints at |in| and stores the
 * result in |out|.
 *
 * Returns the number of compressed bytes processed.
 */
extern size_t
vbyte_uncompress32(const uint8_t *in, uint32_t *out, size_t length);
extern size_t
vbyte_uncompress64(const uint8_t *in, uint64_t *out, size_t length);


/**
 * Returns the value at the given |index| from a compressed sequence.
 *
 * |size| is the length of the byte array pointed to by |in|.
 *
 * Make sure that |index| does not exceed the length of the sequence.
 */
extern uint32_t
vbyte_select32(const uint8_t *in, size_t size, size_t index);
extern uint64_t
vbyte_select64(const uint8_t *in, size_t size, size_t index);

/**
 * Performs a linear search for |value|.
 *
 * Returns the index of the found element, or |length| if the key was not
 * found.
 */
extern size_t
vbyte_linear_search32(const uint8_t *in, size_t length, uint32_t value,
                uint32_t *presult);
extern size_t
vbyte_linear_search64(const uint8_t *in, size_t length, uint64_t value,
                uint64_t *presult);

extern size_t
vbyte_sorted_search32(const uint8_t *in, size_t length, uint32_t value,
                uint32_t *presult);
extern size_t
vbyte_sorted_search64(const uint8_t *in, size_t length, uint64_t value,
                uint64_t *presult);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* VBYTE_H_ee452711_c856_416d_82f4_e12eef8a49be */
