#include "multi_bit_solver.h"

#include <numeric>
#include <queue>
#include <unordered_set>

#include "rng.h"

using namespace std;

MultiBitSolver::MultiBitSolver(vector<vector<int>> clauses_, int n_,
                               vector<int> phi_master_)
    : clauses(clauses_), n(n_), phi_master(phi_master_) {
  m = clauses_.size();
  assert(m > 0);
  inv_clauses = vector<vector<int>>(2 * n);
  for (int i = 0; i < m; i++) {
    for (int j : clauses[i]) {
      inv_clauses[j].push_back(i);
    }
  }
}

MultiBitSolver::MultiBitSolver(vector<vector<int>> clauses_, int n_)
    : clauses(clauses_), n(n_) {
  m = clauses_.size();
  assert(m > 0);
  phi_master = vector<int>(n, 0);
  inv_clauses = vector<vector<int>>(2 * n);
  for (int i = 0; i < m; i++) {
    for (int j : clauses[i]) {
      inv_clauses[j].push_back(i);
    }
  }
}

bool MultiBitSolver::satisfies(const vector<int>& assigment) {
  for (vector<int>& clause : clauses) {
    int clause_value = 0;
    for (int j = 0; j < clause.size() && !clause_value; j++) {
      if (clause[j] & 1) {
        clause_value = !assigment[clause[j] >> 1];
      } else {
        clause_value = assigment[clause[j] >> 1];
      }
    }
    if (!clause_value && !clause.empty()) {
      return false;
    }
  }
  return true;
}

vector<int> MultiBitSolver::get_rem_lits(
    const vector<int>& clause, const unordered_map<int, int>& phi_active_map) {
  assert(false);
}

void MultiBitSolver::unit_propagation(unordered_map<int, int>& phi_active_map) {
  queue<int> q;
  unordered_set<int> in_queue;

  for (int i = 0; i < m; i++) {
    vector<int> rem_lits = get_rem_lits(clauses[i], phi_active_map);
    for (int u : rem_lits) {
      if (!in_queue.count(u) && !in_queue.count(u ^ 1)) {
        q.push(u);
        in_queue.insert(u);
      }
    }
  }

  while (!q.empty()) {
    int u = q.front();
    q.pop();
    in_queue.erase(u);

    for (int i : inv_clauses[u ^ 1]) {
      phi_active_map[u >> 1] = !(u & 1);
      vector<int> rem_lits = get_rem_lits(clauses[i], phi_active_map);
      for (int v : rem_lits) {
        assert(v != u);
        phi_active_map[v >> 1] = !(v & 1);
        if (!in_queue.count(v)) {
          q.push(v);
          in_queue.insert(v);
        }
      }
    }
  }
}

vector<int> MultiBitSolver::solve(int& periods) {
  vector<int> pi(n);
  iota(pi.begin(), pi.end(), 0);

  int period;
  for (period = 0; !satisfies(phi_master); period++) {
    shuffle(pi.begin(), pi.end(), RNG::m_mt);
    unordered_map<int, int> phi_active_map;
    bool change = true;

    for (int j = 0; j < n; j++) {
      if (change) {
        unit_propagation(phi_active_map);
        change = false;
      }
      if (phi_active_map.count(pi[j]) == 0) {
        bool v = phi_master[pi[j]];
        phi_active_map[pi[j]] = v;
        change = true;
      }
    }

    vector<int> phi_active(n);
    for (auto p : phi_active_map) {
      phi_active[p.first] = p.second;
    }

    if (phi_active == phi_master) {
      int random_idx = RNG::uniform(n);
      phi_active[random_idx] = !phi_active[random_idx];
    }
    phi_master = phi_active;
  }

  periods = period;
  return phi_master;
}

vector<int> MultiBitSolver::solve() {
  int foo;
  return solve(foo);
}
