#include <vector>

#include "cnf_generator.h"
#include "multi_bit_solver.h"

using namespace std;

int main() {
  int n_variables = 30;
  int n_clauses = 5 * n_variables;
  // 1000;

  vector<vector<int>> clauses = CNFGenerator::generate(n_variables, n_clauses);
  printf("Finished generating clauses\n");

  // for(auto a: clauses){
  // for(auto b: a)
  // printf("value %d \n", b);
  // printf("\n\n");
  // }

  MultiBitSolver multi_bit_solver = MultiBitSolver(clauses, n_variables);
  int periods;
  vector<bool> assignment = multi_bit_solver.solve(periods);

  // make sure it works
  bool conj = true;
  for (vector<int>& clause : clauses) {
    bool disj = false;
    for (int u : clause) {
      disj = disj || ((u & 1) ? !assignment[u >> 1] : assignment[u >> 1]);
    }
    conj = conj && disj;
  }
  assert(conj);

  printf("Assignment found after %d periods: \n", periods);
  for (int i = 0; i < n_variables; i++) {
    printf("x%d: %s\n", i + 1, assignment[i] ? "true" : "false");
  }
}
