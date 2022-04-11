#include <vector>

#include "cnf_parser.h"
#include "single_bit_solver.h"

using namespace std;

int main() {

  CNFParser *parser;
  std::vector<std::vector<int>> clauses = parser->parse_file("problems.cnf");
  int n_variables = parser->n_variables; 
  int n_clauses = parser->n_clauses; 

  SingleBitSolver single_bit_solver = SingleBitSolver(clauses, n_variables);
  int periods;
  vector<bool> assignment = single_bit_solver.solve(periods);

  printf("Assignment found after %d periods: \n", periods);
  for (int i = 0; i < n_variables; i++) {
    printf("x%d: %s\n", i + 1, assignment[i] ? "true" : "false");
  }
}
