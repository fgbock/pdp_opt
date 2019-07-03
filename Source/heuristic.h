#include "parser.h"

// Parallelized heuristic, reaches a local maximum/minimum.
int p_localSearch();

// Parallelized metaheuristic, iterates calls to local search.
int p_iteratedLS(Graph* weightGraph, int threadCount);
