#include <algorithm>
#include <chrono>
#include <cstdio>
#include <numeric>
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

void unit_propagation(vector<vector<bool>>& clauses,
                      unordered_map<int, bool> phi_active) {}

int main() {
  int m = 3;  // clauses
  int n = 3;  // variables

  // (x or !y) and (x or z) and (z)
  vector<vector<bool>> initial_clauses(m, vector<bool>(2 * n, false));

  initial_clauses[0][0] = true;
  initial_clauses[0][3] = true;
  initial_clauses[1][0] = true;
  initial_clauses[1][4] = true;
  initial_clauses[2][4] = true;
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

  vector<vector<bool>> F = initial_clauses;

  int period;
  for (period = 0; !satisfies(F, phi_master); period++) {
    shuffle(pi.begin(), pi.end(), rng);
    unordered_map<int, bool> phi_active_map;
    for (int j = 0; j < n; j++) {
      unit_propagation(F, phi_active_map);
      if (phi_active_map.count(pi[j]) == 0) {
        bool v = phi_master[pi[j]];
        phi_active_map[pi[j]] = v;
      }
    }
    vector<bool> phi_active(n);
    transform(phi_active_map.begin(), phi_active_map.end(), phi_active.begin(),
              [](auto p) { return p.second; });

    if (phi_active == phi_master) {
      int random_idx = u(rng);
      phi_active[random_idx] = !phi_active[random_idx];

      /* Reinitialize from the beginning because there could have been a cascade
        of clauses eliminated through unit propagation caused by random_idx */
      F = initial_clauses;
    }
    phi_master = phi_active;
  }

  printf("Assignment found after %d periods: \n", period);
  for (int i = 0; i < n; i++) {
    printf("%c: %s\n", 'x' + i, phi_master[i] ? "true" : "false");
  }
}