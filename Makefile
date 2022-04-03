all: sat.cpp
	g++ -std=c++17 sat.cpp -o parallel-sat

clean:
	rm parallel-sat