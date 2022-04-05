parallel-sat: cnf_generator.o main.o solver.o rng.o
	g++ -std=c++17 -I. -o parallel-sat cnf_generator.o main.o solver.o rng.o

cnf_generator.o: cnf_generator.cpp cnf_generator.h
	g++ -std=c++17 -I. -c cnf_generator.cpp

solver.o: solver.cpp solver.h
	g++ -std=c++17 -I. -c solver.cpp

main.o: main.cpp
	g++ -std=c++17 -I. -c main.cpp

rng.o: rng.h
	g++ -std=c++17 -I. -c rng.cpp

clean:
	rm parallel-sat *.o *.gch