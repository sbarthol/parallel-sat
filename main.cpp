#include <vector>

#include "cnf_parser.h"
#include "single_bit_solver.h"
#include "multi_bit_solver.h"

using namespace std;

int main() {
  
  CNFParser *parser;
  std::vector<std::vector<int>> clauses = parser->parse_file("problems250.cnf");
  int n_variables = parser->n_variables; 
  int n_clauses = parser->n_clauses; 
  printf("Completed parse of problem, %i n_variables %i n_clauses\n", n_variables, n_clauses);

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
