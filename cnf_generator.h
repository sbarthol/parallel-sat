#include <vector>

#ifndef CNF_GENERATOR_H
#define CNF_GENERATOR_H

class CNFGenerator {
 private:
  CNFGenerator(){};
  static int count_pairs(std::vector<std::vector<int>> clauses, int var_i, int clause_i);

 public:
  /*
 Generates a random string of bools,
 then clauses that the bools satisfy
 returns: clauses and clause sizes
 */
  static std::vector<std::vector<int>> generate(int n_variables, int n_clauses);
};

#endif