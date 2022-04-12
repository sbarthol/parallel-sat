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

  fscanf(input, "%i \n", &n_variables);
  fscanf(input, "%i \n", &n_clauses);

  int k = 3;
  vector<vector<int>> clauses(n_clauses, vector<int>(k, 0));
  // return clauses;
  // clauses = &vector<vector<int>>(n_clauses, vector<int>(k, 0));

  for (int j = 0; j < n_clauses; j++) {
    fscanf(input, "%i %i %i 0", &clauses[j][0], &clauses[j][1], &clauses[j][2]);
    for (int kk = 0; kk < k; kk++) {
      clauses[j][kk] = (abs(clauses[j][kk]) - 1) << 1 | (clauses[j][kk] < 0);
    }
  }

  fclose(input);
  return clauses;
};
