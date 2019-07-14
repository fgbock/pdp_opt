#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "parser.h"
#include "heuristic.h"
using namespace std;
// Constants
#define NUM_ARGS 6
#define ITERATED_LOCAL_SEARCH 1
#define SIMULATED_ANNEALING 2
#define TABU_SEARCH 3
#define GENETIC 4
#define GRASP 5
#define PARTITIONED_ILS 6

// Check if arguments match program definition.
int argsInvalid(int argc){
  if (argc != NUM_ARGS){
    cout << "ERROR: Incorrect number of arguments" << endl;
    cout << "Got " << argc << " arguments " << NUM_ARGS << endl;
    cout << "Call program as: ./optimizer <opt> <threads> <iter> <filename>" << endl;
    return 1;
  }
  return 0;
}

// Get number of threads to be used for execution.
int getArgVal(char* arg){
  std::istringstream strstream(arg);
  int val;
  if (!(strstream >> val) || !strstream.eof()){
    std::cerr << "ERROR: Invalid number." << arg << endl;
  }
  return val;
}

// Debugging function, print part of the graph in the terminal.
void displayMatrix(vector<vector<int> > matrix, int dim){
  int i, j;
  if (dim > 10) dim = 10;
  cout << "[Matrix]" << endl;
  for (i = 0; i < dim; i++){
    for (j = 0; j < dim; j++){
      cout << matrix[i][j] << "\t";
    }
    cout << endl;
  }
  cout << endl;
}

// Given:
//  -> metaheuristic code, integer
//  -> number of threads to use, integer
//  -> filename of the problem description, string
// Output:
//  -> solution value and time used.
int main(int argc, char* argv[]){
  if (argsInvalid(argc)) return -1;
  int heuristic = getArgVal(argv[1]);
  int threadCount = getArgVal(argv[2]);
  int stopCond = getArgVal(argv[3]);
  int testId = getArgVal(argv[4]);
  string filename = argv[5];
  string filepath = /*"./Data/" +*/ filename;
  Graph* wGraph = new Graph(filepath);
  //
  cout << endl << "Processing file: " << filepath << endl;
  cout << "Graph dimension: " << wGraph->dimension << endl;
  cout << "Number of threads " << threadCount << endl << endl;
  displayMatrix(wGraph->edgeMatrix, wGraph->dimension);
  switch(heuristic){
    case ITERATED_LOCAL_SEARCH:
      cout << "Executing iterated local search." << endl;
      p_iteratedLS(wGraph, threadCount, stopCond, filename, testId);
      break;
    case SIMULATED_ANNEALING:
      cout << "Executing simulated annealing." << endl;
      p_simulatedAnnealing(wGraph, threadCount, stopCond, filename, testId);
      break;
    default:
      cout << "ERROR: Invalid heuristic selected." << endl;
  }
  return 0;
}
