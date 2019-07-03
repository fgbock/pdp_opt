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
