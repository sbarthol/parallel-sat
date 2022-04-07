#include "cnf_generator.h"

#include "rng.h"

using namespace std;

vector<vector<int>> CNFGenerator::generate(int n_variables, int n_clauses) {
  // First generate random assignment
  vector<bool> assignment(n_variables, false);
  for (int i = 0; i < assignment.size(); i++) {
    if (RNG::uniform(2) == 0) {
      assignment[i] = true;
    }
  }

  // Then generate clauses valid according to assignment
  vector<vector<int>> clauses(n_clauses);

  int inv_freq = max(int(n_variables / 2), 2);  // can be tuned
  for (int j = 0; j < n_clauses; j++) {
    bool has_valid = false;
    for (int i = 0; i < n_variables && clauses[j].size() < 2; i++) {
      if (RNG::uniform(inv_freq) != 0) continue;
      // if we pass rand(), then include variable i in this clause

      // if our clause already has a valid term, we can be random
      if (has_valid) {
        if (RNG::uniform(inv_freq) != 0) {
          clauses[j].push_back(i << 1);
        } else {
          clauses[j].push_back((i << 1) + 1);
        }
      }  // else, we put the actual value
      else {
        if (assignment[i]) {
          clauses[j].push_back(i << 1);
        } else {
          clauses[j].push_back((i << 1) + 1);
        }
        has_valid = true;
      }
    }
  }

  return clauses;
}