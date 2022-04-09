parallel-sat: cnf_generator.o main.o single_bit_solver.o multi_bit_solver.o rng.o
	g++ -std=c++17 -I. -o parallel-sat cnf_generator.o main.o single_bit_solver.o multi_bit_solver.o rng.o

cnf_generator.o: cnf_generator.cpp cnf_generator.h
	g++ -std=c++17 -I. -c cnf_generator.cpp

single_bit_solver.o: single_bit_solver.cpp single_bit_solver.h
	g++ -std=c++17 -I. -c single_bit_solver.cpp

multi_bit_solver.o: multi_bit_solver.cpp multi_bit_solver.h
	g++ -std=c++17 -I. -c multi_bit_solver.cpp

main.o: main.cpp
	g++ -std=c++17 -I. -c main.cpp

rng.o: rng.h
	g++ -std=c++17 -I. -c rng.cpp

clean:
	rm parallel-sat *.o