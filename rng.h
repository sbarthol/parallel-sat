#include <random>

#ifndef RNG_H
#define RNG_H

class RNG {
 private:
  RNG(){};

 public:
  static std::mt19937 m_mt;
  static int uniform_int(int n);
  static int uniform_int();
  static bool uniform_bool();
};

#endif