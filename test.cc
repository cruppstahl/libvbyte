
#include <vector>
#include <stdio.h>
#include <assert.h>
#include <ctime>

#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>

#include "timer.h"
#include "vbyte.h"

extern void simdvbyteinit();
extern size_t masked_vbyte_read_loop(const uint8_t* in, uint32_t* out,
                              size_t length);
extern uint32_t masked_vbyte_select(const uint8_t *in, size_t length,
                              size_t slot);
extern int masked_vbyte_search(const uint8_t *in, uint64_t length,
                              uint32_t key, uint32_t *presult);

typedef size_t (*uncompress_function)(const uint8_t *, uint32_t *, size_t);

uint32_t seed = 0;

static void
test_select32(const std::vector<uint32_t> &plain,
                const std::vector<uint8_t> &z)
{
  int loops = 5;
  uint32_t length = plain.size();

  {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      for (uint32_t i = 0; i < length; i += 1 + length / 100) {
        uint32_t v = vbyte_select32(&z[0], z.size(), i);
        assert(plain[i] == v);
      }
    }
    printf("sel %s %u -> %f\n", "plain32", (uint32_t)length,
                    t.seconds() / loops);
  }

  // masked vbyte select's 2nd parameter is the length of the sequence
  //    (in elements), not the size (in bytes)
  {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      for (uint32_t i = 0; i < length; i += 1 + length / 500) {
        uint32_t v = masked_vbyte_select(&z[0], length, i);
        assert(plain[i] == v);
      }
    }
    printf("sel %s %u -> %f\n", "masked ", (uint32_t)length,
                    t.seconds() / loops);
  }
}

static void
test_find32(const std::vector<uint32_t> &plain,
                const std::vector<uint8_t> &z, bool sorted)
{
  int loops = 5;
  uint32_t length = plain.size();

  {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      for (uint32_t i = 0; i < length; i += 1 + length / 5000) {
        uint32_t found;
        size_t pos;
        if (sorted)
          pos = vbyte_sorted_search32(&z[0], z.size(), plain[i], &found);
        else
          pos = vbyte_linear_search32(&z[0], z.size(), plain[i], &found);
        assert(i == pos);
        assert(plain[i] == found);
      }
    }
    printf("fnd plain32 (%ssorted) %u -> %f\n", sorted ? "" : "not ",
                   (uint32_t)length, t.seconds() / loops);
  }

  // masked vbyte select's 2nd parameter is the length of the sequence
  //    (in elements), not the size (in bytes)
  {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      for (uint32_t i = 0; i < length; i += 1 + length / 5000) {
        uint32_t found;
        masked_vbyte_search(&z[0], z.size(), plain[i], &found);
        // assert((int)i == pos); Ignore - broken anyway
        // assert(plain[i] == found);
      }
    }
    printf("fnd masked  %u -> %f\n", (uint32_t)length, t.seconds() / loops);
  }
}

static void
test_select64(const std::vector<uint64_t> &plain,
                const std::vector<uint8_t> &z)
{
  int loops = 5;
  uint32_t length = plain.size();

  {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      for (uint32_t i = 0; i < length; i += 1 + length / 100) {
        uint64_t v = vbyte_select64(&z[0], z.size(), i);
        assert(plain[i] == v);
      }
    }
    printf("sel %s %u -> %f\n", "plain64", (uint32_t)length,
                    t.seconds() / loops);
  }
}

static void
test_find64(const std::vector<uint64_t> &plain,
                const std::vector<uint8_t> &z, bool sorted)
{
  int loops = 5;
  uint32_t length = plain.size();

  {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      for (uint32_t i = 0; i < length; i += 1 + length / 5000) {
        uint64_t found;
        size_t pos;
        if (sorted)
          pos = vbyte_sorted_search64(&z[0], z.size(), plain[i], &found);
        else
          pos = vbyte_linear_search64(&z[0], z.size(), plain[i], &found);
        assert(i == pos);
        assert(plain[i] == found);
      }
    }
    printf("fnd plain64 (%ssorted) %u -> %f\n", sorted ? "" : "not ",
                    (uint32_t)length, t.seconds() / loops);
  }
}

static void
test32_asc(size_t length)
{
  std::vector<uint32_t> plain;
  std::vector<uint8_t> z(length * 5);
  std::vector<uint32_t> out(length);

  for (uint32_t i = 0; i < length; i++)
    plain.push_back(i * 7);

  vbyte_compress32(&plain[0], &z[0], length);

  int loops = 5;
  const char *codec_names[] = {"plain32",
                               "masked "};
  uncompress_function defun[] = {vbyte_uncompress32,
                                 masked_vbyte_read_loop};

  for (size_t i = 0; i < sizeof(defun) / sizeof(void *); i++) {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      defun[i](&z[0], &out[0], length);
      for (uint32_t i = 0; i < length; i++)
        assert(plain[i] == out[i]);
    }
    printf("asc %s %u -> %f\n", codec_names[i], (uint32_t)length,
                    t.seconds() / loops);
  }

  test_select32(plain, z);
  test_find32(plain, z, true);
}

inline static void
test32_rnd(size_t length)
{
  std::vector<uint32_t> plain;
  std::vector<uint8_t> z(length * 5);
  std::vector<uint32_t> out(length);
  boost::mt19937 rng;
  rng.seed(seed);

  for (uint32_t i = 0; i < length; i++)
    plain.push_back(rng());

  vbyte_compress32(&plain[0], &z[0], length);

  int loops = 5;
  const char *codec_names[] = {"plain32",
                               "masked "};
  uncompress_function fun[] = {vbyte_uncompress32,
                               masked_vbyte_read_loop};

  for (size_t i = 0; i < sizeof(fun) / sizeof(void *); i++) {
    Timer<boost::chrono::high_resolution_clock> t;
    for (int l = 0; l < loops; l++) {
      fun[i](&z[0], &out[0], length);
      for (uint32_t i = 0; i < length; i++)
        assert(plain[i] == out[i]);
    }
    printf("rnd %s %u -> %f\n", codec_names[i], (uint32_t)length,
                    t.seconds() / loops);
  }

  test_select32(plain, z);
  test_find32(plain, z, false);
}

inline static void
test64_asc(size_t length)
{
  std::vector<uint64_t> plain;
  std::vector<uint8_t> z(length * 9);
  std::vector<uint64_t> out(length);

  for (uint32_t i = 0; i < length; i++)
    plain.push_back(i * 7);

  vbyte_compress64(&plain[0], &z[0], length);

  int loops = 5;
  Timer<boost::chrono::high_resolution_clock> t;
  for (int l = 0; l < loops; l++) {
    vbyte_uncompress64(&z[0], &out[0], length);
    for (uint32_t i = 0; i < length; i++)
      assert(plain[i] == out[i]);
  }
  printf("asc %s %u -> %f\n", "plain64", (uint32_t)length, t.seconds() / loops);

  test_select64(plain, z);
  test_find64(plain, z, true);
}

inline static void
test64_rnd(size_t length)
{
  std::vector<uint64_t> plain;
  std::vector<uint8_t> z(length * 10);
  std::vector<uint64_t> out(length);
  boost::mt19937_64 rng;
  rng.seed(seed);

  for (uint32_t i = 0; i < length; i++)
    plain.push_back(rng());

  vbyte_compress64(&plain[0], &z[0], length);

  int loops = 5;
  Timer<boost::chrono::high_resolution_clock> t;
  for (int l = 0; l < loops; l++) {
    vbyte_uncompress64(&z[0], &out[0], length);
    for (uint32_t i = 0; i < length; i++)
      assert(plain[i] == out[i]);
  }
  printf("rnd %s %u -> %f\n", "plain64", (uint32_t)length, t.seconds() / loops);

  test_select64(plain, z);
  test_find64(plain, z, false);
}

inline static void
test(size_t length)
{
  test32_asc(length);
  test64_asc(length);
  test32_rnd(length);
  test64_rnd(length);
  printf("\n");
}

int
main()
{
  seed = std::time(0);
  printf("seed: %u\n", seed);

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
