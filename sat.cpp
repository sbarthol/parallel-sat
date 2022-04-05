#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <queue>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

/*
Generates a random string of bools,
then clauses that the bools satisfy
arguments: m is # clauses, n is # variables
returns: clauses and clause sizes
*/
std::pair<vector<vector<bool>>, vector<int>> generate_problem(int m, int n) {
  // First generate random assignment
  std::srand(23);
  vector<bool> assignment(n, false);
  for (int i = 0; i < assignment.size(); i++) {
    if (std::rand() % 2 == 0) {
      assignment[i] = true;
    }
  }

  // Then generate clauses valid according to assignment
  vector<vector<bool>> clauses(m, vector<bool>(2 * n, false));
  vector<int> clause_sizes(m);

  int inv_freq = std::max(int(n / 2), 2);  // can be tuned
  for (int j = 0; j < m; j++) {
    bool has_valid = false;
    for (int i = 0; i < n; i++) {
      if (std::rand() % inv_freq != 0) continue;
      // if we pass rand(), then include variable i in this clause

      // if our clause already has a valid term, we can be random
      if (has_valid) {
        if (std::rand() % inv_freq != 0) {
          clauses[j][(i << 1)] = true;
        } else {
          clauses[j][(i << 1) + 1] = true;
        }
      }  // else, we put the actual value
      else {
        if (assignment[i]) {
          clauses[j][(i << 1)] = true;
        } else {
          clauses[j][(i << 1) + 1] = true;
        }
        has_valid = true;
      }
      clause_sizes[j]++;
    }
  }

  return std::pair(clauses, clause_sizes);
}
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
                    int var, bool value) {
  for (int i = 0; i < clauses.size(); i++) {
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

void unit_propagation(vector<vector<bool>>& clauses, vector<int>& clause_sizes,
                      unordered_map<int, bool>& phi_active_map) {
  queue<int> q;
  unordered_set<int> in_queue;

  for (int i = 0; i < clause_sizes.size(); i++) {
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

    for (int i = 0; i < clause_sizes.size(); i++) {
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

void print(vector<vector<bool>> clauses) {
  printf("---------------\n");

  for (int i = 0; i < clauses.size(); i++) {
    printf("clause %d:", i);
    for (int j = 0; j < clauses[i].size(); j++) {
      if (clauses[i][j]) {
        if (j & 1) {
          printf(" !x%d", j >> 1);
        } else {
          printf(" x%d", j >> 1);
        }
      }
    }
    printf("\n");
  }
}

int main() {
  int m = 6;  // clauses
  int n = 4;  // variables

  // (x or !y) and (x or z) and (z)

  vector<vector<bool>> initial_clauses(m, vector<bool>(2 * n, false));
  vector<int> initial_clause_sizes(m);

  /*
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
  */

  auto clauses_pair = generate_problem(m, n);
  initial_clauses = clauses_pair.first;
  initial_clause_sizes = clauses_pair.second;

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
