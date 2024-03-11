#pragma once

#include <stdint.h>


class Xoroshiro64 {
    private:
      uint32_t state[2];
      inline uint32_t rotl(const uint32_t x, int k) {
        return (x << k) | (x >> (32 - k));
      };

    public:
      uint32_t next();
};
