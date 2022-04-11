#include "cnf_parser.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace std;

// https://www.cs.ubc.ca/~hoos/SATLIB/benchm.html

vector<vector<int>> CNFParser::parse_file(char* filename){
    FILE *input = fopen(filename, "r");
    if (!input) {
        printf("Unable to open file: %s.\n", filename);
    }

    fscanf(input, "%i \n", &n_variables);
    fscanf(input, "%i \n", &n_clauses);

    int tmp;
    int k=3;
    vector<vector<int>> clauses_(n_clauses, vector<int>(k, 0));
    // clauses = &vector<vector<int>>(n_clauses, vector<int>(k, 0));
    for(int j = 0; j < n_clauses; j ++) {
        fscanf(input, "%i %i %i %i", &clauses_[j][0], &clauses_[j][1], &clauses_[j][2], &tmp);      
    }

    fclose(input);
    // clauses = clauses_;
    return clauses_;

};
