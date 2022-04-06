#include "solver.h"

#include <numeric>
#include <queue>
#include <unordered_set>

#include "rng.h"

using namespace std;

Solver::Solver(vector<vector<bool>> clauses_, vector<bool> phi_master_)
    : clauses(clauses_), phi_master(phi_master_) {
  m = clauses_.size();
  assert(m > 0);
  assert(clauses_[0].size() % 2 == 0);
  n = clauses_[0].size() / 2;
}

Solver::Solver(vector<vector<bool>> clauses_) : clauses(clauses_) {
  m = clauses_.size();
  assert(m > 0);
  assert(clauses_[0].size() % 2 == 0);
  n = clauses_[0].size() / 2;
  phi_master = vector<bool>(n, false);
}

bool Solver::satisfies(const vector<bool>& assigment) {
  for (int i = 0; i < m; i++) {
    bool clause_value = false;
    int clause_count = 0;
    for (int j = 0; j < 2 * n && !clause_value; j++) {
      if (clauses[i][j]) {
        clause_value = assigment[j >> 1] == !(j & 1);
        clause_count++;
      }
    }
    if (!clause_value && clause_count > 0) {
      return false;
    }
  }
  return true;
}

bool Solver::is_unit_clause(const vector<bool>& clause,
                            const unordered_map<int, bool>& phi_active_map,
                            int& rem_lit) {
  rem_lit = -1;
  for (int u = 0; u < 2 * n; u++) {
    if (clause[u]) {
      if (phi_active_map.count(u) == 0) {
        if (rem_lit == -1) {
          rem_lit = u;
        } else {
          return false;
        }
      } else if (phi_active_map.at(u)) {
        return false;
      }
    }
  }
  return rem_lit != -1;
}

void Solver::unit_propagation(unordered_map<int, bool>& phi_active_map) {
  queue<int> q;
  unordered_set<int> in_queue;

  for (int i = 0; i < m; i++) {
    int u;
    if (is_unit_clause(clauses[i], phi_active_map, u)) {
      assert(u != -1);
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

    for (int i = 0; i < m; i++) {
      if (clauses[i][u ^ 1]) {
        phi_active_map[u >> 1] = !(u & 1);

        int v;
        if (is_unit_clause(clauses[i], phi_active_map, v)) {
          phi_active_map[v >> 1] = !(v & 1);
          if (!in_queue.count(v)) {
            q.push(v);
            in_queue.insert(v);
          }
        }
      }
    }
  }
}

vector<bool> Solver::solve(int& periods) {
  vector<int> pi(n);
  iota(pi.begin(), pi.end(), 0);

  int period;
  for (period = 0; !satisfies(phi_master); period++) {
    shuffle(pi.begin(), pi.end(), RNG::m_mt);
    unordered_map<int, bool> phi_active_map;
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
    vector<bool> phi_active(n);
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

vector<bool> Solver::solve() {
  int foo;
  return solve(foo);
}
