#include <unordered_map>
#include <vector>

#ifndef SOLVER_H
#define SOLVER_H

class Solver {
 public:
  virtual std::vector<bool> solve(int& periods) = 0;
  virtual std::vector<bool> solve() = 0;
};

#endif