#include <ctime>
#include <vector>

#include "cnf_parser.h"
#include "multi_bit_solver.h"
#include "single_bit_solver.h"

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 3) {
    printf(
        "Please use the program as \"./parallel-sat <filename> <--single or "
        "--multi>\"\n");
    return -1;
  }
  CNFParser parser;

  std::vector<std::vector<int>> clauses = parser.parse_file(argv[1]);
  printf("Completed parse of problem, %i n_variables %i n_clauses\n",
         parser.n_variables, parser.n_clauses);

  vector<bool> assignment;
  int periods;

  if (!strcmp("--single", argv[2])) {
    SingleBitSolver single_bit_solver =
        SingleBitSolver(clauses, parser.n_variables);
    printf("Solving using single bit solver...\n");
    clock_t begin = clock();
    assignment = single_bit_solver.solve(periods);
    clock_t end = clock();
    printf("Solution found in %.4lf seconds\n",
           double(end - begin) / CLOCKS_PER_SEC);
  } else if (!strcmp("--multi", argv[2])) {
    MultiBitSolver multi_bit_solver =
        MultiBitSolver(clauses, parser.n_variables);
    printf("Solving using %lu-bit solver...\n",
           8 * sizeof(MultiBitSolver::uintk_t));
    clock_t begin = clock();
    assignment = multi_bit_solver.solve(periods);
    clock_t end = clock();
    printf("Solution found in %.4lf seconds\n",
           double(end - begin) / CLOCKS_PER_SEC);
  } else {
    printf(
        "Please use the program as \"./parallel-sat <filename> <--single or "
        "--multi>\"\n");
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

  /*printf("Assignment found after %d periods: \n", periods);
  for (int i = 0; i < parser.n_variables; i++) {
    printf("x%d: %s\n", i + 1, assignment[i] ? "true" : "false");
  }*/
}
