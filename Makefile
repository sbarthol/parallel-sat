all: sat.cpp
	g++ sat.cpp -o parallel-sat

clean:
	rm parallel-sat