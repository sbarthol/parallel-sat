#include <unordered_map>
#include <vector>

#include "solver.h"

#ifndef SINGLE_BIT_SOLVER_H
#define SINGLE_BIT_SOLVER_H

class SingleBitSolver : Solver {
 private:
  std::vector<bool> phi_master;
  std::vector<std::vector<int>> clauses;
  std::vector<std::vector<int>> inv_clauses;
  int n, m;

  bool satisfies(const std::vector<bool>& assigment);
  void update_clauses(std::vector<std::vector<int>>& clauses, int var,
                      bool value);
  void unit_propagation(std::unordered_map<int, bool>& phi_active_map);
  bool is_unit_clause(const std::vector<int>& clause,
                      const std::unordered_map<int, bool>& phi_active_map,
                      int& remaining_literal);

 public:
  SingleBitSolver(std::vector<std::vector<int>> clauses_, int n_,
                  std::vector<bool> phi_master_);
  SingleBitSolver(std::vector<std::vector<int>> clauses_, int n_);
  std::vector<bool> solve(int& periods);
  std::vector<bool> solve();
};

#endif