#include <algorithm>
#include <chrono>
#include <cstdio>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_map>
#include <vector>

using namespace std;

bool satisfies(const vector<vector<bool>>& clauses,
               const vector<bool>& assigment) {
  for (int i = 0; i < clauses.size(); i++) {
    bool clause_value = false;
    int clause_count = 0;
    for (int j = 0; j < clauses[i].size() && !clause_value; j++) {
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

void update_clauses(vector<vector<bool>>& clauses, vector<int>& clause_sizes,
                    int var) {
  for (int i = 0; i < clauses.size(); i++) {
    if (clauses[i][var << 1]) {
      clauses[i][var << 1] = false;
      clause_sizes[i]--;
    }
    if (clauses[i][(var << 1) | 1]) {
      clauses[i][(var << 1) | 1] = false;
      clause_sizes[i]--;
    }
    assert(clause_sizes[i] > 0);
  }
}

void unit_propagation(vector<vector<bool>>& clauses, vector<int>& clause_sizes,
                      unordered_map<int, bool>& phi_active_map) {
  queue<int> q;
  for (int i = 0; i < clause_sizes.size(); i++) {
    if (clause_sizes[i] == 1) {
      q.push(i);
    }
  }
  while (!q.empty()) {
    int top = q.front();
    q.pop();
    if (clause_sizes[top] == 1) {
      // Todo: use bit array instead of array of bools ?
      int j = -1, lsb = -1;
      for (int i = 0; i < clauses[top].size() && j == -1; i++) {
        if (clauses[top][i]) {
          j = i >> 1;
          lsb = i & 1;
        }
      }
      assert(j != -1 && lsb != -1);
      assert(phi_active_map.count(j) == 0);
      phi_active_map[j] = lsb ^ 1;
      update_clauses(clauses, clause_sizes, j);
      for (int i = 0; i < clause_sizes.size(); i++) {
        if (clause_sizes[i] == 1) {
          assert(i != top);
          q.push(i);
        }
      }
    }
  }
}

int main() {
  int m = 6;  // clauses
  int n = 4;  // variables

  // (x or !y) and (x or z) and (z)
  vector<vector<bool>> initial_clauses(m, vector<bool>(2 * n, false));
  vector<int> initial_clause_sizes(m);

  initial_clauses[0][0] = true;
  initial_clauses[0][2] = true;
  initial_clause_sizes[0] = 2;

  initial_clauses[1][1] = true;
  initial_clauses[1][2] = true;
  initial_clauses[1][4] = true;
  initial_clause_sizes[1] = 3;

  initial_clauses[2][3] = true;
  initial_clauses[2][5] = true;
  initial_clause_sizes[2] = 2;

  initial_clauses[3][3] = true;
  initial_clauses[3][4] = true;
  initial_clauses[3][7] = true;
  initial_clause_sizes[3] = 3;

  initial_clauses[4][3] = true;
  initial_clauses[4][4] = true;
  initial_clauses[4][6] = true;
  initial_clause_sizes[4] = 3;

  initial_clauses[5][5] = true;
  initial_clauses[5][7] = true;
  initial_clause_sizes[5] = 2;
  /*

  (x or !y) and (x or z)
  x = 0 -> x = 0 and !x = 1
  y = 1 -> y = 2 and !y = 3
  z = 2 -> z = 4 and !z = 5
  etc...



  */

  vector<bool> phi_master(n);

  vector<int> pi(n);
  iota(pi.begin(), pi.end(), 0);

  auto rng = mt19937(0);
  uniform_int_distribution<int> u(0, n - 1);

  int period;
  for (period = 0; !satisfies(initial_clauses, phi_master); period++) {
    shuffle(pi.begin(), pi.end(), rng);
    unordered_map<int, bool> phi_active_map;
    vector<vector<bool>> F = initial_clauses;
    vector<int> F_sizes = initial_clause_sizes;
    for (int j = 0; j < n; j++) {
      unit_propagation(F, F_sizes, phi_active_map);
      if (phi_active_map.count(pi[j]) == 0) {
        bool v = phi_master[pi[j]];
        phi_active_map[pi[j]] = v;
        update_clauses(F, F_sizes, pi[j]);
      }
    }
    vector<bool> phi_active(n);
    transform(phi_active_map.begin(), phi_active_map.end(), phi_active.begin(),
              [](auto p) { return p.second; });

    if (phi_active == phi_master) {
      int random_idx = u(rng);
      phi_active[random_idx] = !phi_active[random_idx];
    }
    phi_master = phi_active;
  }

  printf("Assignment found after %d periods: \n", period);
  for (int i = 0; i < n; i++) {
    printf("x%d: %s\n", i + 1, phi_master[i] ? "true" : "false");
  }
}