#include <iostream>
#include <fstream>
#include <sstream>

#include "graph.h"

using namespace std;

void Node::addNeighbor(Node* nPtr, uint newEdgeId) {
  Node::Neighbor newNeighbor = { nPtr, newEdgeId };
  neighbors.push_back(newNeighbor);
}

Graph::Graph(char* filepath) {
  ifstream inFile;
  inFile.open(filepath);
  if (!inFile) {
    cerr << "Unable to open file " << filepath << endl;
    exit(1);
  }
  string line, temp;

  // set nodeNum, edgeNum, netNum, netGroupNum
  getline(inFile, line);
  stringstream ss;
  ss << line;
  ss >> nodeNum >> edgeNum >> netNum >> netGroupNum;

  // set _nodes
  _nodes = new Node[nodeNum];

  // set node ID
  for (uint i = 0; i < nodeNum; i += 1) {
    _nodes[i].setId(i);
  }

  // set node neightbors
  for (uint i = 0; i < edgeNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint from, to;
    ss >> from >> to;
    _nodes[from].addNeighbor(&_nodes[to], i);
    _nodes[to].addNeighbor(&_nodes[from], i);
  }

  // set net
  _nets = new Net[netNum];
  for (uint i = 0; i < netNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint from, to;
    ss >> from;
    _nets[i].setSource(&_nodes[from]);
    while (ss >> to) {
      _nets[i].addDestination(&_nodes[to]);
    }
  }

  // set net Group
  for (int i = 0; i < netGroupNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint id;
    while (ss >> id) {
      // cout << id << ' ';
    }
    // cout << endl;
  }
}
