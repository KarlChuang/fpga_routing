#include <iostream>
#include <fstream>
#include <sstream>

#include "graph.h"

using namespace std;

Graph::Graph(char* filepath) {
  ifstream inFile;
  inFile.open(filepath);
  if (!inFile) {
    cerr << "Unable to open file " << filepath << endl;
    exit(1);
  }
  string line, temp;
  getline(inFile, line);
  stringstream ss;
  ss << line;
  ss >> nodeNum >> edgeNum >> netNum >> netGroupNum;
  for (int i = 0; i < edgeNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint from, to;
    ss >> from >> to;
    cout << from << ' ' << to << endl;
  }
  cout << "---" << endl;
  for (int i = 0; i < netNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint from, to;
    ss >> from;
    cout << from << ' ';
    while (ss >> to) {
      cout << to << ' ';
    }
    cout << endl;
  }
  cout << "---" << endl;
  for (int i = 0; i < netGroupNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint id;
    while (ss >> id) {
      cout << id << ' ';
    }
    cout << endl;
  }
  cout << "---" << endl;
}

Node::Node() {

}
