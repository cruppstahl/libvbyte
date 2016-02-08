
#include <vector>
#include <stdio.h>
#include <assert.h>

#include "timer.h"
#include "vbyte.h"

extern void simdvbyteinit();
extern uint32_t masked_vbyte_read_loop(const uint8_t* in, uint32_t* out,
                              uint32_t length);

typedef uint32_t (*uncompress_function)(const uint8_t *, uint32_t *, uint32_t);

inline static void
test_decompress(uncompress_function foo, const std::vector<uint32_t> &plain,
                const std::vector<uint8_t> &z, std::vector<uint32_t> &out,
                uint32_t length)
{
  foo(&z[0], &out[0], length);
  for (uint32_t i = 0; i < length; i++)
    assert(plain[i] == out[i]);
}

inline static void
test(uint32_t length)
{
  std::vector<uint32_t> plain;
  std::vector<uint8_t> z(length * 5);
  std::vector<uint32_t> out(length);

  for (uint32_t i = 0; i < length; i++)
    plain.push_back(i * 7);

  vbyte_compress(&plain[0], &z[0], length);

  int loops = 5;
  const char *codec_names[] = {"plain  ",
                               "scalar1",
                               "scalar2",
                               "scalar3",
                               "masked "};
  uncompress_function fun[] = {vbyte_uncompress_plain,
                               vbyte_uncompress_scalar1,
                               vbyte_uncompress_scalar2,
                               vbyte_uncompress_scalar3,
                               masked_vbyte_read_loop};

  for (int i = 0; i < 5; i++) {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      test_decompress(fun[i], plain, z, out, length);
    }
    printf("%s %u -> %f\n", codec_names[i], length, t.seconds() / loops);
  }

  printf("\n");
}

int
main()
{
  simdvbyteinit();
  test(1);
  test(2);
  test(10);
  test(16);
  test(33);
  test(42);
  test(100);
  test(128);
  test(256);
  test(333);
  test(1000);
  test(10000);
  test(20000);
  test(100000);
  test(1000000);
  test(10000000);
  return 0;
}
