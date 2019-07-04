#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "parser.h"
#include "heuristic.h"
using namespace std;
// Constants
#define NUM_ARGS 4
#define ITERATED_LOCAL_SEARCH 0
#define SIMULATED_ANNEALING 1

// Check if arguments match program definition.
int argsInvalid(int argc){
  if (argc != NUM_ARGS){
    cout << "ERROR: Incorrect number of arguments" << endl;
    cout << "Got " << argc << " arguments " << NUM_ARGS << endl;
    cout << "Call program as: ./optimize <opt> <threads> <filename>" << endl;
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
  for (i = 0; i < dim; i++){
    for (j = 0; j < dim; j++){
      cout << matrix[i][j] << "\t";
    }
    cout << endl;
  }
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
  string filename = argv[3];
  string filepath = "./Data/" + filename;
  Graph* wGraph = new Graph(filepath);
  cout << "Processing file: " << filepath << endl;
  cout << "Graph dimension: " << wGraph->dimension << endl;
  cout << "Number of threads selected: " << threadCount << endl;
  displayMatrix(wGraph->edgeMatrix, wGraph->dimension);
  switch(heuristic){
    case ITERATED_LOCAL_SEARCH:
      p_iteratedLS(wGraph, threadCount);
      break;
    case SIMULATED_ANNEALING:
      p_simulatedAnnealing();
      break;
    default:
      cout << "ERROR: Invalid heuristic selected." << endl;
  }
  return 0;
}
