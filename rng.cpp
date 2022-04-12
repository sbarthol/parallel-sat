#include "rng.h"

std::mt19937 RNG::m_mt = std::mt19937(0);

int RNG::uniform(int n) {
  std::uniform_int_distribution<int> u(0, n - 1);
  return u(m_mt);
}

int RNG::uniform() { return m_mt(); }
