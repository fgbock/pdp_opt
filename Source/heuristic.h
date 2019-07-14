#include <vector>
#include <list>
#include <omp.h>
#include "parser.h"
using namespace std;

// Parallelized metaheuristic, system entropy based local search.
void p_simulatedAnnealing(Graph* wGraph, int threadCount, int stopCond, string filename, int testId);

// Parallelized metaheuristic, iterates calls to local search.
void p_iteratedLS(Graph* wGraph, int threadCount, int stopCond, string filename, int testId);
