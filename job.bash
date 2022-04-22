#!/bin/bash

for filename in test/uf250-1065/*.cnf; do
    #echo "Running "$filename" ..."
    ./parallel-sat --multi "$filename"
    #echo "---------------------------------------"
done

