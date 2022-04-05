#include <random>

#ifndef RNG_H
#define RNG_H

class RNG {
 private:
  RNG(){};

 public:
  static std::mt19937 m_mt;
  static int uniform(int n);
};

#endif