#include "cnf_generator.h"

#include "rng.h"

using namespace std;

vector<vector<bool>> CNFGenerator::generate(int n_variables, int n_clauses) {
  // First generate random assignment
  vector<bool> assignment(n_variables, false);
  for (int i = 0; i < assignment.size(); i++) {
    if (RNG::uniform(2) == 0) {
      assignment[i] = true;
    }
  }

  // Then generate clauses valid according to assignment
  vector<vector<bool>> clauses(n_clauses, vector<bool>(2 * n_variables, false));
  vector<int> clause_sizes(n_clauses);

  int inv_freq = max(int(n_variables / 2), 2);  // can be tuned
  for (int j = 0; j < n_clauses; j++) {
    bool has_valid = false;
    for (int i = 0; i < n_variables && clause_sizes[j] < 2; i++) {
      if (RNG::uniform(inv_freq) != 0) continue;
      // if we pass rand(), then include variable i in this clause

      // if our clause already has a valid term, we can be random
      if (has_valid) {
        if (RNG::uniform(inv_freq) != 0) {
          clauses[j][(i << 1)] = true;
          clause_sizes[j]++;
        } else {
          clauses[j][(i << 1) + 1] = true;
          clause_sizes[j]++;
        }
      }  // else, we put the actual value
      else {
        if (assignment[i]) {
          clauses[j][(i << 1)] = true;
          clause_sizes[j]++;
        } else {
          clauses[j][(i << 1) + 1] = true;
          clause_sizes[j]++;
        }
        has_valid = true;
      }
    }
  }

  return clauses;
}