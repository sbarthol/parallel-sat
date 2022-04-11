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
    vector<vector<int>> clauses(n_clauses, vector<int>(k, 0));
    // clauses = &vector<vector<int>>(n_clauses, vector<int>(k, 0));
    for(int j = 0; j < n_clauses; j ++) {
        fscanf(input, "%i %i %i %i", &clauses[j][0], &clauses[j][1], &clauses[j][2], &tmp);     
        for(int kk = 0; kk < k; k++) {
            int signedval = clauses[j][kk];
            clauses[j][kk] = (signedval << 1) + int(signedval < 0);
        } 
    }

    fclose(input);
    // clauses = clauses_;
    return clauses;

};
