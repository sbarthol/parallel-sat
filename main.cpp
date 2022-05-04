#include <omp.h>

#include <vector>

#include "cnf_parser.h"
#include "multi_bit_solver.h"
#include "single_bit_solver.h"
#include <cassert>
#include <cstring>

using namespace std;

int main(int argc, char *argv[]) {
  const char* errstr = "Please use the program as \"./parallel-sat <--single or --multi>" 
  " <filename> <optional: --use_openmp=false>\"\n";

  if (argc != 3 && argc != 4) {
    printf(errstr);
    return -1;
  }
  CNFParser parser;

  std::vector<std::vector<int>> clauses = parser.parse_file(argv[2]);
  // printf("Completed parse of problem, %i n_variables %i n_clauses\n",
  //      parser.n_variables, parser.n_clauses);

  vector<bool> assignment;
  int periods;

  bool use_openmp = true;
  if (argc == 4 && strcmp("--use_openmp=false", argv[3])) {
    use_openmp = false;
  }


  if (!strcmp("--single", argv[1])) {
    printf("Solving using single bit solver...\n");
    double begin = omp_get_wtime();
    SingleBitSolver single_bit_solver =
        SingleBitSolver(clauses, parser.n_variables);
    assignment = single_bit_solver.solve(periods);
    double end = omp_get_wtime();
    printf("Solution found in %.4lf seconds and %d periods\n", end - begin,
           periods);
  } else if (!strcmp("--multi", argv[1])) {
    // printf("Solving using %lu-bit solver...\n",
    // 8 * sizeof(MultiBitSolver::uintk_t));
    double begin = omp_get_wtime();
    MultiBitSolver multi_bit_solver =
        MultiBitSolver(clauses, parser.n_variables, use_openmp);
    assignment = multi_bit_solver.solve(periods);
    double end = omp_get_wtime();
    printf("%.7lf\n", (end - begin) / periods);
  } else {
    printf(errstr);
    return -1;
  }

  // make sure it works
  bool conj = true;
  for (vector<int> &clause : clauses) {
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
