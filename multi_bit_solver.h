#include <thread>
#include <unordered_map>
#include <vector>

#include "concurrent_queue.h"
#include "solver.h"

#ifndef MULTI_BIT_SOLVER_H
#define MULTI_BIT_SOLVER_H

class MultiBitSolver : Solver {
 public:
  typedef uint64_t uintk_t;
  MultiBitSolver(std::vector<std::vector<int>> clauses_, int n_, bool use_openmp = true);
  std::vector<bool> solve(int& periods);
  std::vector<bool> solve();

 private:
  std::vector<uintk_t> phi_master;
  std::vector<std::vector<int>> clauses;
  std::vector<std::vector<int>> inv_clauses;
  int n, m;
  bool use_openmp = true;

  // Duplicate removal task
  std::thread dup_task;
  void remove_dups();
  ConcurrentQueue<std::vector<uintk_t>> in_q;
  ConcurrentQueue<uintk_t> out_q;

  uintk_t get_random();

  uintk_t satisfies(const std::vector<uintk_t>& phi);
  void unit_propagation(std::vector<uintk_t>& phi);
  void unit_propagation_queue(std::vector<uintk_t>& phi);
  void unit_propagation_fork(std::vector<uintk_t>& phi);
  std::vector<std::pair<int, uintk_t>> get_rem_lits(
      const std::vector<int>& clause, const std::vector<uintk_t>& phi);
  uintk_t compute_duplicate_mask(const std::vector<uintk_t>& phi);

  int count_dups(const std::vector<uintk_t>& phi);
};

#endif