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
  clause_sizes = vector<int>(m);
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < 2 * n; j++) {
      clause_sizes[i] += clauses[i][j];
    }
  }
}

Solver::Solver(vector<vector<bool>> clauses_) : clauses(clauses_) {
  m = clauses_.size();
  assert(m > 0);
  assert(clauses_[0].size() % 2 == 0);
  n = clauses_[0].size() / 2;
  phi_master = vector<bool>(n, false);
  clause_sizes = vector<int>(m);
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < 2 * n; j++) {
      clause_sizes[i] += clauses[i][j];
    }
  }
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

void Solver::update_clauses(vector<vector<bool>>& clauses,
                            vector<int>& clause_sizes, int var, bool value) {
  for (int i = 0; i < m; i++) {
    if (clauses[i][var << 1] && value) {
      fill(clauses[i].begin(), clauses[i].end(), false);
      clause_sizes[i] = 0;
    } else if (clauses[i][var << 1]) {
      clauses[i][var << 1] = false;
      clause_sizes[i]--;
    }
    if (clauses[i][(var << 1) | 1] && !value) {
      fill(clauses[i].begin(), clauses[i].end(), false);
      clause_sizes[i] = 0;
    } else if (clauses[i][(var << 1) | 1]) {
      clauses[i][(var << 1) | 1] = false;
      clause_sizes[i]--;
    }
    assert(clause_sizes[i] >= 0);
  }
}

void Solver::unit_propagation(vector<vector<bool>>& clauses,
                              vector<int>& clause_sizes,
                              unordered_map<int, bool>& phi_active_map) {
  queue<int> q;
  unordered_set<int> in_queue;

  for (int i = 0; i < m; i++) {
    if (clause_sizes[i] == 1) {
      int u = 0;
      while (!clauses[i][u]) {
        u++;
      }
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
        update_clauses(clauses, clause_sizes, u >> 1, !(u & 1));
        if (clause_sizes[i] == 1) {
          int v = 0;
          while (!clauses[i][v]) {
            v++;
          }
          phi_active_map[v >> 1] = !(v & 1);
          update_clauses(clauses, clause_sizes, v >> 1, !(v & 1));
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
    vector<vector<bool>> F = clauses;
    vector<int> F_sizes = clause_sizes;

    bool change = true;

    for (int j = 0; j < n; j++) {
      if (change) {
        unit_propagation(F, F_sizes, phi_active_map);
        change = false;
      }
      if (phi_active_map.count(pi[j]) == 0) {
        bool v = phi_master[pi[j]];
        phi_active_map[pi[j]] = v;
        update_clauses(F, F_sizes, pi[j], v);
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
