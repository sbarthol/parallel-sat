
parallel-sat: OFLAGS=-O3 
parallel-sat:  binary_name=parallel-sat

parallel-sat-o0: OFLAGS=-O0 
parallel-sat-o0:  binary_name=parallel-sat-o0

parallel-sat: cnf_parser.o main.o single_bit_solver.o multi_bit_solver.o rng.o concurrent_queue.o
	g++ -std=c++11 $(OFLAGS) -I. -fopenmp -o $(binary_name) cnf_parser.o main.o concurrent_queue.o single_bit_solver.o multi_bit_solver.o rng.o -pthread

parallel-sat-o0: cnf_parser.o main.o single_bit_solver.o multi_bit_solver.o rng.o concurrent_queue.o
	g++ -std=c++11 $(OFLAGS) -I. -fopenmp -o $(binary_name) cnf_parser.o main.o concurrent_queue.o single_bit_solver.o multi_bit_solver.o rng.o -pthread

cnf_parser.o: cnf_parser.cpp cnf_parser.h
	g++ -std=c++11 $(OFLAGS) -I. -c cnf_parser.cpp

cnf_generator.o: cnf_generator.cpp cnf_generator.h
	g++ -std=c++11 $(OFLAGS) -I. -c cnf_generator.cpp

single_bit_solver.o: single_bit_solver.cpp single_bit_solver.h
	g++ -std=c++11 $(OFLAGS) -I. -c single_bit_solver.cpp

multi_bit_solver.o: multi_bit_solver.cpp multi_bit_solver.h
	g++ -std=c++11 $(OFLAGS) -I. -fopenmp -c multi_bit_solver.cpp  

concurrent_queue.o: concurrent_queue.cpp concurrent_queue.h
	g++ -std=c++11 $(OFLAGS) -I. -c concurrent_queue.cpp

main.o: main.cpp
	g++ -std=c++11 $(OFLAGS) -I. -c main.cpp

rng.o: rng.h
	g++ -std=c++11 $(OFLAGS) -I. -c rng.cpp

clean:
	rm parallel-sat parallel-sat-o0 *.o
