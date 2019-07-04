#include <vector>
#include <list>
#include <omp.h>
#include "parser.h"
using namespace std;

int g_threadCount;
vector<int> g_solution;

void displaySolution(){
  int i;
  for (i = 0; i < g_solution.size(); i++){
    cout << "Position #" << i << " node: " << g_solution[i] << endl;
  }
}

// Verifies if the order restriction is satisfied for a given node.
//  -> iterates through the solution previous to node "i" position
//  -> if node "j" is in the solution, add 1 to "j" entry of auxiliary vector
//  -> check this vector vs the restrictions of "i" col in edgeWeight matrix
int checkOrder(int node, int position, Graph* wGraph){
  int i;
  vector<int> auxSolution(wGraph->dimension,0);
  for (i = 0; i < position; i++){
    auxSolution[g_solution[i]] = 1;
  }
  for (i = 0; i < wGraph->dimension; i++){
    //cout << "For node " << i << " is " <<  auxSolution[i] << endl;
    if ((wGraph->edgeMatrix[node][i] == -1) && (auxSolution[i] != 1)) return 0;
  }
  return 1;
}

// Calculates the solution's value.
int calcValue(Graph* wGraph){
  int i, sum;
  for(i = 0; i < g_solution.size() - 1; i++){
    sum += wGraph->edgeMatrix[g_solution[i]][g_solution[i+1]];
  }
  return sum;
}


// Builds a (greedy) initial solution.
//  -> itereates, adding first node that has no restrictions to the solution
int buildInitial(Graph* wGraph){
  int i, node, val, pos, selNode, minVal;
  vector<int> isUsed(wGraph->dimension, 0);
  g_solution.resize(wGraph->dimension);
  // Always initialize the first and last node as first and last of solution
  g_solution[0] = 0;
  g_solution[wGraph->dimension - 1] = wGraph->dimension - 1;

  // Below temp sequential solution for testing
  for (pos = 1; pos < wGraph->dimension - 1; pos++){
    minVal = -1;
    for (node = 1; node <  wGraph->dimension - 1; node++){
      val = wGraph->edgeMatrix[g_solution[pos-1]][node];
      if (checkOrder(node, pos, wGraph) != 0 && isUsed[node] == 0 && (val > -1) && ((val <= minVal) || minVal == -1)){
        minVal = val;
        selNode = node;
        //cout << "Node is " << node << endl;
      }
    }
    g_solution[pos] = selNode;
    isUsed[selNode] = 1;
  }
  displaySolution();
  val = calcValue(wGraph);
  cout << "Initial solution value is: " << val << endl;
}


// Parallelized heuristic, reaches a local maximum/minimum.
//  -> iterates through neighborhood to find the best switch candidate
//  -> neighborhood is any node which may be swapped with the current node
int p_localSearch(){

}

// Parallelized metaheuristic, system entropy based local search.
void p_simulatedAnnealing(){

};

// Parallelized metaheuristic, iterates calls to local search.
void p_iteratedLS(Graph* wGraph, int threadCount){
  int i;
  g_threadCount = threadCount;
  omp_set_dynamic(0);
  omp_set_num_threads(threadCount);
  buildInitial(wGraph);
}
