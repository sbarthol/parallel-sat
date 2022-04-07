#include <vector>

#include "cnf_generator.h"
#include "solver.h"

using namespace std;

int main() {
  int n_variables = 40;
  int n_clauses = 10;

  vector<vector<int>> clauses = CNFGenerator::generate(n_variables, n_clauses);

  Solver single_bit_solver = Solver(clauses, n_variables);
  int periods;
  vector<bool> assignment = single_bit_solver.solve(periods);

  printf("Assignment found after %d periods: \n", periods);
  for (int i = 0; i < n_variables; i++) {
    printf("x%d: %s\n", i + 1, assignment[i] ? "true" : "false");
  }
}
