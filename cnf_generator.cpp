#include "cnf_generator.h"

#include "rng.h"
#include <queue>
#include <iostream>

using namespace std;

// Implement Balanced SAT https://arxiv.org/pdf/1903.03592.pdf

// https://www.geeksforgeeks.org/priority-queue-of-pairs-in-c-ordered-by-first/
typedef pair<int, int> pairint;

// Count the number of "repeated pairs" for from adding var_i to clause_i
int CNFGenerator::count_pairs(vector<vector<int>> clauses, int var_i, int clause_i) {

  // For each variable already in this clause
  vector<int> possible_repetition = clauses[clause_i];
  for(int y = 0; y < clauses[clause_i].size(); y++)
    if (clauses[clause_i][y] == var_i) return INT_MAX;
    
  int result = 0;
  for(auto pr : possible_repetition) {
    int count = 0;

    for(int z = 0; z < clauses.size(); z++){
      if(z == clause_i) continue;

      for(int y = 0; y < clauses[z].size(); y++) {
        if (clauses[z][y] == pr) count ++;
      }
    } 

    // if count >= 2, not a *new* repeated pair
    if(count < 2) {
      result += count;
    }
  }
  return result;
}

vector<vector<int>> CNFGenerator::generate(int n_variables, int n_clauses) {

  // Pair is: (count of # occurances, variable index)
  priority_queue<pairint, vector<pairint>, greater<pairint> > pq;
  for (int i = 0; i < n_variables; i++) {
    pq.push(make_pair(0, i));
  }

  // size of clause
  int k = 2;
  vector<vector<int>> clauses(n_clauses, vector<int>(k, 0));

  for (int i = 0; i < n_clauses; i++) {
    for (int j = 0; j < k; j++) {

      // setup tiebreaking
      // https://stackoverflow.com/questions/16111337/declaring-a-priority-queue-in-c-with-a-custom-comparator
      auto comparator = [clauses, i](pairint a, pairint b) {
        int a_pairs = CNFGenerator::count_pairs(clauses, a.second, i);
        int b_pairs = CNFGenerator::count_pairs(clauses, b.second, i);
        return a_pairs < b_pairs;
      };
      priority_queue<pairint, vector<pairint>, decltype(comparator)> ties(comparator);

      // execute tiebreaking
      int num_occ = pq.top().first;
      while(pq.top().first == num_occ && pq.size() > 0) {
        ties.push(pq.top());
        pq.pop();
      }

      // found bestpair
      pairint best_pair = ties.top();
      ties.pop();

      // add back the rest of the ties
      while(ties.size()) {
        pq.push(ties.top());
        ties.pop();
      }

      // now assign clause
      clauses[i][j] = (best_pair.second << 1);
      best_pair.first ++ ;
      pq.push(best_pair);
    }
  }

  // now do random 50/50 pos/neg
  vector<bool> assignment(n_variables, false);
  for (int i = 0; i < assignment.size(); i++) {
    if (RNG::uniform(2) == 0) {
      assignment[i] = true;
    }
  }

  // for(bool b : assignment)
  // cout << b; 
  // printf("\n\n\n");

  for (int i = 0; i < n_clauses; i++) {
    for (int j = 0; j < k; j++) {
      // if we assigned this variable to be false, then enforce
      if( !assignment[clauses[i][j] << 1 ] ) {
        clauses[i][j] ++ ;
      }
    }
  }

  return clauses;
}