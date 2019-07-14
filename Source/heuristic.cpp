#include <vector>
#include <iostream>
#include <fstream>
#include <omp.h>
#include <time.h>
#include <ctime>
#include <cstdlib>
#include <stack>
#include <math.h>
#include <unistd.h>
#include <sys/stat.h>
#include "parser.h"
using namespace std;

#define TRUE 1
#define FALSE 0
#define VALID 1
#define NOT_VALID 0
#define ITERATED_LOCAL_SEARCH 1
#define SIMULATED_ANNEALING 2
#define SA_OFFSET 3

int g_threadCount;
vector<int> g_bestSolution;
int g_testId;

bool fileExists(const std::string& filename){
    struct stat buf;
    if (stat(filename.c_str(), &buf) != -1){
        return true;
    }
    return false;
}

// Write solution and time to file
//  -> problem file
//  -> metaheuristic isUsed
//  -> stop condition
//  -> elapsed time
//  -> best value found
int saveResultsOld(int meta, double time, string filename, int stopCond, int initVal, int bestVal){
  ofstream outFile;
  outFile.open("./output.csv", fstream::app);
  outFile << "Filename:" << "\t" << filename << endl;
  outFile << "Metaheuristic:"<<  "\t" << meta << endl;
  outFile << "Threads:" <<  "\t" << g_threadCount << endl;
  outFile << "Iterations:" <<  "\t" << stopCond << endl;
  outFile << "Time elapsed:" <<  "\t" << time << endl;
  outFile << "Initial Value:"<<  "\t" << initVal << endl;
  outFile << "Best Value: "<<  "\t" << bestVal << endl << endl;
  outFile.close();
  return 0;
}

int saveResults(int meta, double time, string filename, int stopCond, int initVal, int bestVal){
  ofstream outFile;
  if (fileExists("./output_" + std::to_string(g_testId) + ".csv") == false){
    outFile.open("./output_" + std::to_string(g_testId) + ".csv", fstream::app);
    outFile << "Filename:" << "\t";
    outFile << "Metaheuristic:"<<  "\t";
    outFile << "Threads:" <<  "\t";
    outFile << "Iterations:" <<  "\t";
    outFile << "Time elapsed:" <<  "\t";
    outFile << "Initial Value:"<<  "\t";
    outFile << "Best Value: "<<  "\t" << endl;
  }
  else{
    outFile.open("./output_" + std::to_string(g_testId) + ".csv", fstream::app);
  }
  outFile << filename;
  outFile << "\t" << meta;
  outFile << "\t" << g_threadCount;
  outFile << "\t" << stopCond;
  outFile << "\t" << time;
  outFile << "\t" << initVal;
  outFile << "\t" << bestVal << endl;
  outFile.close();
  return 0;
}

// Display the full solution path in order
void displaySolution(vector<int> *solution){
  int i;
  cout << "[Path]" << endl;
  for (i = 0; i < (*solution).size(); i++){
    cout << "Position #" << i << " node: " << (*solution)[i] << endl;
  }
  cout << endl;
}

// Verifies if the order restriction is satisfied for a given node.
//  -> iterates through the solution previous to node "i" position
//  -> if node "j" is in the solution, add 1 to "j" entry of auxiliary vector
//  -> check this vector vs the restrictions of "i" col in edgeWeight matrix
//  -> if any restriction isn't met, return invalid, otherwise, return valid
int checkOrder(int node, int position, Graph* wGraph, vector<int> *solution){
  int i;
  vector<int> auxSolution(wGraph->dimension,0);
  for (i = 0; i < position; i++){
    auxSolution[(*solution)[i]] = 1;
  }
  for (i = 0; i < wGraph->dimension; i++){
    //cout << "For node " << i << " is " <<  auxSolution[i] << endl;
    if ((wGraph->edgeMatrix[node][i] == -1) && (auxSolution[i] != 1)) return NOT_VALID;
  }
  return VALID;
}

// For every node between former and current position, check if they require node
// If yes, return INVALID
int checkOrderBetween(int node, int formerPos, int currentPos, vector<int> *solution, Graph* wGraph){
  int i;
  for (i = (formerPos + 1); i < currentPos; i++){
    if (wGraph->edgeMatrix[(*solution)[i]][node] == -1) return NOT_VALID;
  }
  return VALID;
}

int checkOrderAll(vector<int> *solution, Graph* wGraph){
  int i;
  for(i = 1; i < wGraph->dimension - 2; i++){
    if (checkOrder((*solution)[i], i, wGraph, solution) == NOT_VALID) return NOT_VALID;
  }
  return VALID;
}

// Calculates the solution's value.
int calcValue(Graph* wGraph, vector<int> solution){
  int i, sum = 0, part;
  for(i = 0; i < solution.size() - 1; i++){
    part = wGraph->edgeMatrix[solution[i]][solution[i+1]];
    sum += part;
    //cout << "From node " << solution[i] << " to " << solution[i+1] << ": "<< part << endl;
    //cout << "current sum is " << sum << endl;
  }
  return sum;
}

// Builds a (greedy) initial solution.
//  -> itereates, adding first node that has no restrictions to the solution
int p_buildInitial(Graph* wGraph){
  int i, node, val, pos, selNode, minVal;
  vector<int> isUsed(wGraph->dimension, 0);
  g_bestSolution.resize(wGraph->dimension);
  // Always initialize the first and last node as first and last of solution
  g_bestSolution[0] = 0;
  g_bestSolution[wGraph->dimension - 1] = wGraph->dimension - 1;

  for (pos = 1; pos < wGraph->dimension - 1; pos++){
    minVal = -1;
    for (node = 1; node <  wGraph->dimension - 1; node++){
      val = wGraph->edgeMatrix[g_bestSolution[pos-1]][node];
      if (checkOrder(node, pos, wGraph, &g_bestSolution) == VALID && isUsed[node] == 0 &&
      (val > -1) && ((val <= minVal) || minVal == -1)){
        minVal = val;
        selNode = node;
        //cout << "Node is " << node << endl;
      }
    }
    g_bestSolution[pos] = selNode;
    isUsed[selNode] = 1;
  }
  //displaySolution(&g_bestSolution);
  val = calcValue(wGraph, g_bestSolution);
  //cout << "Initial solution value is: " << val << endl;
  return val;
}

// Parallelized heuristic, reaches a local maximum/minimum.
//  -> iterates through neighborhood to find the best switch candidate
//  -> neighborhood is...
//  -> verify is switching position invalidates solution, complexity is O(N)
int p_localSearch(Graph* wGraph, vector<int> *solution){
  int i, node, auxnode, pos, oldVal = 0, newVal = 0;
  vector<int> auxSolution;
  stack<int> posStack;
  auxSolution.resize(wGraph->dimension);
  for (i = 1; i < (wGraph->dimension - 1); i++){
    posStack.push(i);
  }
  while(!(posStack.empty())){
    pos = posStack.top();
    posStack.pop();
    node = (*solution)[pos];
    for(i = 1; i < (wGraph->dimension - 1); i++){
      if(i != pos){
        auxnode = (*solution)[i];
        auxSolution = (*solution);
        auxSolution[i] = node;
        auxSolution[pos] = auxnode;
        oldVal = calcValue(wGraph, (*solution));
        newVal = calcValue(wGraph, auxSolution);
        if (oldVal >= newVal && pos < i){
          if(checkOrder(auxnode, pos, wGraph, solution) == VALID
            && checkOrderBetween(node, pos, i, solution, wGraph) == VALID){
              /*cout << "1st. Swapping positions i:" << i << " and pos:" << pos << endl;
              cout << "They represent nodes i:" << auxnode << " and pos:" << node << endl;
              cout << "Pre Swap: " << endl;
              displaySolution(&g_bestSolution);*/
              (*solution)[i] = node;
              (*solution)[pos] = auxnode;
              //displaySolution(&g_bestSolution);
              //cout << endl;
              pos = i;
          }
        }
        else if (oldVal >= newVal && pos > i){
          if(checkOrder(node, i, wGraph, solution) == VALID
            && checkOrderBetween(auxnode, i, pos, solution, wGraph) == VALID){
              /*cout << "2nd. Swapping positions i:" << i << " and pos:" << pos << endl;
              cout << "They represent nodes i:" << auxnode << " and pos:" << node << endl;
              cout << "Pre Swap: " << endl;
              displaySolution(&g_bestSolution);*/
              (*solution)[i] = node;
              (*solution)[pos] = auxnode;
              //displaySolution(&g_bestSolution);
              //cout << endl;
              pos = i;
            }
        }
      }
        // check node against another neighbor
      }
    // get another node position
  }
}

// Auxiliary to Parallelized Iterated Local Search
//  -> stochastic alteration to existing solution
void disturbILS(Graph* wGraph, vector<int> *solutionCandidate, int seed){
  int i, auxNode, swapIndex, swapTotal;
  swapTotal = 1 + wGraph->dimension;
  srand(seed);
  for(i = 0; i < swapTotal; i++){
    swapIndex = rand() % (wGraph->dimension - 2) + 1;
    // cout << "Node to switch is " << swapIndex << endl;
    // Check if by switching "index + 1 to position "index" position solution remains valid
    while(swapIndex < wGraph->dimension -3 &&
          checkOrder((*solutionCandidate)[swapIndex + 1], swapIndex, wGraph, solutionCandidate) == VALID){
      auxNode = (*solutionCandidate)[swapIndex + 1];
      (*solutionCandidate)[swapIndex + 1] = (*solutionCandidate)[swapIndex];
      (*solutionCandidate)[swapIndex] = auxNode;
      swapIndex++;
    }
  }
}

// Parallelized Iterated Local Search metaheuristic
//  -> finds an initial g_bestSolution
//  -> calls local search over initial solution to find best solution
//  -> iterates until it reaches the Stop Conditions:
//    -> disturbs the best solution found
//    -> calls local search on distrubed solution
//    -> if result is better than existing best solution, replace it
void p_iteratedLS(Graph* wGraph, int threadCount, int stopCond, string filename, int testId){
  int i, j, initVal = 0, bestVal = 0, candidateVal = 0;
  vector<int> solutionCandidate;
  struct timespec startTime, endTime;
  //
  g_threadCount = threadCount;
  clock_gettime(CLOCK_MONOTONIC, &startTime);
  initVal = p_buildInitial(wGraph);
  p_localSearch(wGraph, &g_bestSolution);
  solutionCandidate.resize(wGraph->dimension);
  bestVal = calcValue(wGraph, g_bestSolution);
  omp_set_dynamic(0);
  omp_set_num_threads(threadCount);
  #pragma omp parallel private (solutionCandidate)
  {
    #pragma omp for nowait
    for (i = 0; i < stopCond; i++){
      solutionCandidate = g_bestSolution;
      disturbILS(wGraph, &solutionCandidate, i); // disturbs best soltion and places it in candidate
      //displaySolution(&solutionCandidate);
      p_localSearch(wGraph, &solutionCandidate);
      candidateVal = calcValue(wGraph, solutionCandidate);
      //cout << "Current candidate val is " << candidateVal << endl;
      //cout << "Current best val is "<< bestVal << endl;
      #pragma omp critical
      {
        if (candidateVal < bestVal){
          //displaySolution(&solutionCandidate);
          g_bestSolution = solutionCandidate;
          bestVal = candidateVal;
        }
      }
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &endTime);
  //Elapsed time in seconds
  double elapsedTime = (endTime.tv_sec - startTime.tv_sec) +
                  (endTime.tv_nsec - startTime.tv_nsec)/1000000000.0;
  //displaySolution(&g_bestSolution);
  cout << "Best value found is: " << calcValue(wGraph, g_bestSolution) << endl;
  g_testId = testId;
  saveResults(ITERATED_LOCAL_SEARCH, elapsedTime, filename, stopCond, initVal, bestVal);
}

// Calculates probabilities based off system entropy / "chaos"
//  -> receives
int probSwitch(int deltaValue, int temperature){
  int prob = 0;
  prob = round(100 * (1 / (1 + exp((double)deltaValue/(double)temperature))));
  //cout << "Prob is:" << prob << endl;
  //usleep(1000000);
  return prob;
}

// Performs local search in a limited neighborhood using probabilities
int p_localSearchSA(Graph* wGraph, vector<int> *solution, int stopCond){
  int i, k, node, auxnode, pos, oldVal = 0, newVal = 0, dieRoll;
  double entropy, decay;
  vector<int> auxSolution;
  stack<int> posStack;
  auxSolution.resize(wGraph->dimension);
  for (i = 1; i < (wGraph->dimension - 1); i++){
    posStack.push(i);
  }
  omp_set_dynamic(0);
  omp_set_num_threads(g_threadCount);
  entropy = 100.0;
  decay = 100.0 / (double) stopCond;
  while(entropy >= 0){
    pos = (int) rand() % (wGraph->dimension - 2) + 1;
    //cout << "Before change " << endl;
    //displaySolution(solution);
    #pragma omp parallel private (auxSolution, auxnode, i, node)
    {
      auxSolution = (*solution);
      #pragma omp for
      for(k = -2; k <= 2; k++){
        if (pos <= 2){
          i = SA_OFFSET + k;
        }
        else if (pos >= wGraph->dimension - 3){
          i = wGraph->dimension - SA_OFFSET + k - 1;
        }
        else{
          i = pos + k;
        }
        if(i != pos){
          oldVal = calcValue(wGraph, auxSolution);
          auxnode = auxSolution[i];
          node = auxSolution[pos];
          auxSolution[i] = node;
          auxSolution[pos] = auxnode;
          newVal = calcValue(wGraph, auxSolution);
          if (pos < i){
            if (checkOrderAll(&auxSolution, wGraph) == VALID){
              dieRoll = rand() % 100 + 1;
              #pragma omp critical
              {
                if(probSwitch(oldVal - newVal, entropy) <= dieRoll){
                    (*solution) = auxSolution;
                    //cout << "auxnode is " << auxnode << " in position " << pos << endl;
                    //cout << "node is " << node << " in position " << i << endl;
                    //displaySolution(solution);
                    //usleep(1000000);
                }
              }
            }
          }
          else if (pos > i ){
            if(checkOrderAll(&auxSolution, wGraph) == VALID){
              dieRoll = rand() % 100 + 1;
              #pragma omp critical
              {
                if(probSwitch(oldVal - newVal, entropy) <= dieRoll){
                  (*solution) = auxSolution;
                  //cout << "auxnode is " << auxnode << " in position " << pos << endl;
                  //cout << "node is " << node << " in position " << i << endl;
                  //displaySolution(solution);
                  //usleep(1000000);
                }
              }
            }
          }
        }
          // check node against another neighbor
      }
    } // End of parallel section
    // get another node position
    entropy = entropy - decay;
  }
}

// Simulated Annealing metaheuristic
//  ->
//  ->
void p_simulatedAnnealing(Graph* wGraph, int threadCount, int stopCond, string filename, int testId){
  int i, j, initVal = 0, bestVal = 0, candidateVal = 0, done = FALSE;
  vector<int> solutionCandidate;
  struct timespec startTime, endTime;
  //
  g_threadCount = threadCount;
  clock_gettime(CLOCK_MONOTONIC, &startTime);
  initVal = p_buildInitial(wGraph);
  solutionCandidate.resize(wGraph->dimension);
  solutionCandidate = g_bestSolution;
  p_localSearchSA(wGraph, &solutionCandidate, stopCond);
  candidateVal = calcValue(wGraph, solutionCandidate);
  bestVal = calcValue(wGraph, g_bestSolution);
  if (candidateVal <= bestVal){
    g_bestSolution = solutionCandidate;
  }
  p_localSearch(wGraph, &g_bestSolution);
  bestVal = calcValue(wGraph, g_bestSolution);
  clock_gettime(CLOCK_MONOTONIC, &endTime);
  //Elapsed time in seconds
  double elapsedTime = (endTime.tv_sec - startTime.tv_sec) +
                  (endTime.tv_nsec - startTime.tv_nsec)/1000000000.0;
  //displaySolution(&g_bestSolution);
  cout << "Best value found is: " << calcValue(wGraph, g_bestSolution) << endl;
  g_testId = testId;
  saveResults(SIMULATED_ANNEALING, elapsedTime, filename, stopCond, initVal, bestVal);
}
