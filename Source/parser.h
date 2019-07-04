#pragma once
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

// Class defines graph to be processed and its origin file.
class Graph{
  public:
    // Data
    vector<vector<int> > edgeMatrix;
    fstream dataFile;
    int dimension;
    // Methods
    void setDimension();
    void setGraph();
    // Constructor
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
