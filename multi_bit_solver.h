#include <unordered_map>
#include <vector>

#include "solver.h"

#ifndef MULTI_BIT_SOLVER_H
#define MULTI_BIT_SOLVER_H

class MultiBitSolver : Solver {
 private:
  typedef uint16_t uintk_t;

  std::vector<uintk_t> phi_master;
  std::vector<std::vector<int>> clauses;
  std::vector<std::vector<int>> inv_clauses;
  int n, m;

  uintk_t get_random();

  uintk_t satisfies(const std::vector<uintk_t>& phi);
  void unit_propagation(std::vector<uintk_t>& phi);
  std::vector<std::pair<int, uintk_t>> get_rem_lits(
      const std::vector<int>& clause, const std::vector<uintk_t>& phi);
  uintk_t compute_duplicate_mask(const std::vector<uintk_t>& phi);

  int count_dups(const std::vector<uintk_t>& phi);

 public:
  MultiBitSolver(std::vector<std::vector<int>> clauses_, int n_);
  std::vector<bool> solve(int& periods);
  std::vector<bool> solve();
};

#endif