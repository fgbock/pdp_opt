#include <vector>
#include <list>
#include <omp.h>
#include "parser.h"
using namespace std;

// Builds a (greedy) initial solution.
int buildInitial(Graph* wGraph);

// Verifies if the order restriction is satisfied for a given node.
int checkOrder(int node, int position);

// Parallelized heuristic, reaches a local maximum/minimum.
int p_localSearch();

// Parallelized metaheuristic, system entropy based local search.
void p_simulatedAnnealing();

// Parallelized metaheuristic, iterates calls to local search.
void p_iteratedLS(Graph* wGraph, int threadCount);
