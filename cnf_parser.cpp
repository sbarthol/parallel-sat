#include "cnf_parser.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html

vector<vector<int>> CNFParser::parse_file(char* filename) {
  FILE* input = fopen(filename, "r");
  if (!input) {
    printf("Unable to open file: %s.\n", filename);
  }
  while (!fscanf(input, "p cnf %i %i", &n_variables, &n_clauses)) {
    fscanf(input, "%*[^\n]\n");
    printf("skipped a line\n");
  }
  printf("read %d and %d\n", n_variables, n_clauses);

  vector<vector<int>> clauses(n_clauses);
  for (int i = 0; i < n_clauses; i++) {
    int x;
    while (fscanf(input, "%i", &x) && x) {
      clauses[i].push_back((abs(x) - 1) << 1 | (x < 0));
    }
  }

  fclose(input);
  return clauses;
};
