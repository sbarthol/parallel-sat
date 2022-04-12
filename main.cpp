#include <vector>

#include "cnf_parser.h"
#include "multi_bit_solver.h"
#include "single_bit_solver.h"

using namespace std;

#define SINGLE_BIT 0
#define MULTI_BIT 1

#define CURRENT 0

int main(int argc, char* argv[]) {
  if (argc != 2) {
    printf("Please provide filename as input\n");
    return -1;
  }
  CNFParser parser;

  std::vector<std::vector<int>> clauses = parser.parse_file(argv[1]);
  printf("Completed parse of problem, %i n_variables %i n_clauses\n",
         parser.n_variables, parser.n_clauses);

  vector<bool> assignment;
  int periods;

  if (CURRENT == SINGLE_BIT) {
    SingleBitSolver single_bit_solver =
        SingleBitSolver(clauses, parser.n_variables);

    printf("Solving using single bit solver...\n");
    assignment = single_bit_solver.solve(periods);
  } else if (CURRENT == MULTI_BIT) {
    MultiBitSolver multi_bit_solver =
        MultiBitSolver(clauses, parser.n_variables);
    printf("Solving using multi bit solver...\n");
    assignment = multi_bit_solver.solve(periods);
  } else {
    printf("error\n");
    return -1;
  }

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
  for (int i = 0; i < parser.n_variables; i++) {
    printf("x%d: %s\n", i + 1, assignment[i] ? "true" : "false");
  }
}
