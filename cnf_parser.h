#include <string>
#include <vector>

#ifndef CNF_PARSER_H
#define CNF_PARSER_H

class CNFParser {
 public:
  CNFParser(){};
  std::vector<std::vector<int>> parse_file(char* filename);
  int n_variables;
  int n_clauses;
};

#endif