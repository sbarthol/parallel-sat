#include <unordered_map>
#include <vector>

#ifndef SOLVER_H
#define SOLVER_H

class Solver {
 private:
  std::vector<bool> phi_master;
  std::vector<std::vector<bool>> clauses;
  std::vector<int> clause_sizes;
  int n, m;

  bool satisfies(const std::vector<bool>& assigment);
  void update_clauses(std::vector<std::vector<bool>>& clauses,
                      std::vector<int>& clause_sizes, int var, bool value);
  void unit_propagation(std::vector<std::vector<bool>>& clauses,
                        std::vector<int>& clause_sizes,
                        std::unordered_map<int, bool>& phi_active_map);

 public:
  Solver(std::vector<std::vector<bool>> clauses_,
         std::vector<bool> phi_master_);
  Solver(std::vector<std::vector<bool>> clauses_);
  std::vector<bool> solve(int& periods);
  std::vector<bool> solve();
};

#endif