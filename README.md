# Parallel SAT Solving using Bit-level Operations

## TITLE

Parallel SAT Solver
Sacha Bartholme - David Noursi

## SUMMARY

We are going to implement a non-deterministic parallel k-SAT using data parallelism on a single-core CPU.

## BACKGROUND

The Boolean satisfiability problem (abbreviated SAT) is the problem of determining if there exists a configuration that satisfies a given Boolean formula. SAT is the first problem that was proven to be NP-complete. We find many projects of past 15618 editions that parallelized the SAT problem using GPU/CPUs and deterministic backtracking based approaches like DPLL, but our project and algorithm will differ from these approaches in the following ways

-   The solver we will implement is an incomplete solver, which means that there is no guarantee that it will decide whether the formula is satisfiable or not
-   The solver is is non-deterministic
-   We will implement data parallelism instead of task parallelism by operating on multi-bit boolean variables (probably 128 - 256 at a time) instead of single booleans and leveraging SIMD instructions on x86/86. The idea is that the algorithm spends most of its time performing arithmetic operations on booleans
-   The solver we will use is much faster in practice

For our project, we will implement and parallelize an algorithm based on the UnitMarch algorithm [1] which was designed at CMU.

## THE CHALLENGE

The challenges are the following

-   Obtaining significant speedup while staying on a single core is hard, but we will try to beat the equivalent sequential version (using single booleans) as well as GPU and multi-core CPU based version using a complete solver (DFS / backtracking / DPLL / CDCL etc…) implemented in SAT solving competitions [2] (e.g. ManySAR, MiniSAT etc…) and by previous students of this class
-   Learning how to use SIMD instructions properly. In assignment 1, we cheated by using a C++ vector library to mimic the behavior of SIMD instructions. Here, we will use the SSE / AVX / AVX2 / MMX registers and SIMD instruction set of the CPU
-   Communication is an issue too, because after reading the above-mentioned paper [1] there will probably be a need to spawn 1 or 2 parallel routines doing some kind of independent work and communicate their results to the algorithm. Remember that multi-bit boolean assignments are the center of gravity of the algorithm. So even if the implementation would be faster on a multi-core (because of these separate routines), we insist on having the best possible speedup on a single core.
-   The fact that we operate on multi-bit booleans makes simple things very hard. Consider the very common task of unit propagation [3] in almost every SAT solver. This has to be adapted to support multi-bit variables.

## RESOURCES

GHC Machine, Eight-core, 3.0 GHz Intel Core i7 processor
Pittsburgh Super Computer - Regular Memory - Pittsburgh Super Computer (optional)

There is no codebase that we can start from as the code described in the paper is closed-source (or in any case we were not able to find it). We will simply start from scratch in C++. The paper we are using as reference is [1].

## GOALS AND DELIVERABLES

### PLAN TO ACHIEVE

We plan to implement the non-deterministic algorithm using multi-bit boolean variables as well as a most equivalent algorithm using single-bit boolean variables. Then we shall be able to show the improvement of the former by benchmarking it on a lot of different SAT formulas (some randomized, and some manufactured edge cases). The ideal speedup should be the size of the boolean vectors (e.g. 256 if using AVX). We should seek to be as close as possible to this ideal speedup.

Additionally, we are trading off correctness (the solver is non-complete) for speed. This means that the algorithm should be significantly faster on a single core than other competition-grade algorithms on a single core. We are using the single core as a common benchmarking criteria. It would be easy to design an algorithm faster than ours with an endless number of cores.

### HOPE TO ACHIEVE

If things progress faster than planned, we will hook up a practical application of SAT in order to make our presentation more digestible. Remember that any problem in NP problem can be reduced to SAT which is NP-complete in polynomial time. A nice application would be the famous Sudoku puzzle which can almost directly be reduced to SAT.

### Goals in case the work goes more slowly

In the worst case, our parallel algorithm will offer a negligible speedup (less or equal than 2x) but will still be correct in most cases (remember that the solver is non-complete).

### Demo

If time permits, we will hook up our algorithm to a Sudoku solver and use our SAT solver as the backend. We will then show how fast it is compared to other algorithms. The time
difference will be in the order of a few seconds (we can make the Sudoku grid arbitrarily large, or we can solve arbitrarily many 9x9 Sudokus).

In any case there will be charts showing the speedup relative to sequential implementations and
winners of previous SAT competition (we intend to beat those).

## PLATFORM CHOICE

It makes sense to use the Gates machines as we only need a single core x86/64 processor that can support some basic SIMD instructions. We could also use the PSC cluster but we would not be using the resources to their full extent anyway.

We chose C++ because we are both familiar with the langues, it is a directly compiled language (fast) and it is more convenient to use SIMD instructions when the language is close to the machine.

## SCHEDULE

-   Week 1: Read the paper and fully understand how the algorithm works.

-   Week 2: Implement the algorithm using 1 bit booleans. Produce a lot of tests.

-   Week 3: Implement the algorithm using n-bit booleans and test it.

-   Week 4: Same as week 3.

-   Week 5: Benchmark the algorithm, draw charts and graphs, find a way to compare it to other algorithms on the same input.

-   Week 6: Last minute optimization of our parallel program and write report.

## MILESTONE

### WORK COMPLETED SO FAR

So far we have implemented the single bit version of the algorithm (according to the paper linked below) which will be our reference when benchmarking the multibit version. This helped us grasp the idea of the algorithm and better understand its spirit before jumping into the multibit version. It works extremely well and terminates after a few iterations (a few trials) most of the time. This is why we tried to implement a generator of hard SAT problems that the algorithm would have a hard time solving. This turned out to to be more complicated than we thought. We managed to implement a generator (see cnf_generator.cpp) using [4] as an inspiration but the problems are not hard enough to be useful. The task was time consuming and certainly not the main focus of the project so we figured we would just download problems from previous SAT competitions.

The multibit version was implemented as well but only uses bitwise operations on 32 bit integers as parallel boolean operations from now. This made it easier to debug and analyse. We read x86/64 documentation to understand what SIMD instructions are available to us and how they can be used. The next step would be to incorporate SIMD instructions to increase the boolean throughput to at least 256.

### HOW WE ARE DOING WITH RESPECT TO GOALS AND DELIVERABLES

We are doing great. We firmly believe that we will be able to achieve all our goals, even the nice to haves.

### POSTER SESSION

We will hook up our algorithm to a Sudoku solver and use our SAT solver as the backend. We will then show how fast it is compared to other algorithms. The time
difference will be in the order of a few seconds (we can make the Sudoku grid arbitrarily large, or we can solve arbitrarily many 9x9 Sudokus).

In any case there will be charts showing the speedup relative to sequential implementations (e.g. the single bit version that we implemented) and maybe even winners of previous SAT competition (we intend to beat those).

### NEW SCHEDULE

-   Week 4.0: Implement a working version of the multibit algorithm without SIMD instructions (Sacha & David).

-   Week 4.5: Implement a working version of the multibit algorithm without SIMD instructions (Sacha & David).

-   Week 5.0: Add SIMD instructions to produce even greater boolean throughput and speedup (Sacha). Build the sudoku solver (David).

-   Week 5.5: Find a way to compare it to other algorithms on the same input (Sacha & David).

-   Week 6.0: Benchmark the algorithm, draw charts and graphs (Sacha & David).

-   Week 6.5: Last minute optimizations of our parallel program (Sacha) and write report (David).

### PRELIMINARY RESULTS

Not yet.

### ISSUES THAT CONCERN US THE MOST

We hope that it will be possible to download SAT solvers from previous SAT competitions, build/run them and hook them up to our tests so we can compare them to our algorithm. If all the above works, we hope that we aren't too bad (not more than 2-3x slower). We have to stay humble as these solvers are built by entire teams of researchers and mathematicians who spent a lot of time and resources on it.

## REFERENCES

-   [1] Heule, Marijn & Maaren, Hans. (2008). Parallel SAT Solving using Bit-level Operations. JSAT. 4. 99-116. 10.3233/SAT190040.
-   [2] http://www.satcompetition.org/
-   [3] https://en.wikipedia.org/wiki/Unit_propagation
-   [4] Escamocher, G., O'Sullivan, B., & Prestwich, S. D. (2019). Generating difficult sat instances by preventing triangles. arXiv preprint arXiv:1903.03592.
