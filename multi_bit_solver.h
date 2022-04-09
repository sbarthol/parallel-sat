#include <unordered_map>
#include <vector>

#ifndef SOLVER_H
#define SOLVER_H

class MultiBitSolver {
 private:
  std::vector<int> phi_master;
  std::vector<std::vector<int>> clauses;
  std::vector<std::vector<int>> inv_clauses;
  int n, m;

  bool satisfies(const std::vector<int>& assigment);
  void update_clauses(std::vector<std::vector<int>>& clauses, int var,
                      int value);
  void unit_propagation(std::vector<int>& phi_active_map);
  std::vector<int> get_rem_lits(const std::vector<int>& clause,
                                const std::vector<int>& phi_active_map);

 public:
  MultiBitSolver(std::vector<std::vector<int>> clauses_, int n_,
                 std::vector<int> phi_master_);
  MultiBitSolver(std::vector<std::vector<int>> clauses_, int n_);
  std::vector<int> solve(int& periods);
  std::vector<int> solve();
};

#endif