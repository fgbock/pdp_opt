#include <sstream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// Class defines graph to be processed and its origin file.
class Graph{
  public:
    vector<vector<int> > edgeMatrix;
    void setDimension();
    void setGraph();
    fstream dataFile;
    int dimension;
    Graph(string filename){
      this->dataFile.open(filename.c_str());
      if (this->dataFile.is_open()){
        setDimension();
        setGraph();
        this->dataFile.close();
      }
      else{
        exit(-1);
      }
    }
};

// Set graph adjacency matrix based off file data.
void Graph::setGraph(){
  int i, edgeValue;
  string line;
  do {
    getline(this->dataFile, line);
  }
  while(line.compare("EDGE_WEIGHT_SECTION") != 0);
  for (i = 0; i < this->dimension; i++){
    getline(this->dataFile, line);
    stringstream strstream(line);
    while (strstream >> edgeValue){
      this->edgeMatrix[i].push_back(edgeValue);
    }
  }
}

// Get graph dimensions from file data.
void Graph::setDimension(){
  int i;
  unsigned int char_pos;
  string line, straux;
  for(i = 0; i < 4; i++){
    getline(this->dataFile, line);
  }
  char_pos = line.find(" ");
  straux = line.substr(char_pos + 1);
  this->dimension = atoi(straux.c_str());
  this->edgeMatrix.resize(this->dimension);
}
