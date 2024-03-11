
#include <stdint.h>

#include "xoroshiro64.h"


uint32_t Xoroshiro64::next() {
  const uint32_t s0 = state[0];
  uint32_t s1 = state[1];
  const uint32_t result_starstar = rotl(s0 * 0x9E3779BB, 5) * 5;

  s1 ^= s0;
  state[0] = rotl(s0, 26) ^ s1 ^ (s1 << 9); // a, b
  state[1] = rotl(s1, 13); // c

  return result_starstar;
}
