#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>

#include "graph.h"

using namespace std;

void Graph::writeFile(char* filename, vector< vector<int> >& netEdgeId, vector< vector<int> >& netEdgeWeight) {
  fstream file;
  file.open(filename, ios::out | ios::trunc);
  for (int netIdx = 0; netIdx < netEdgeId.size(); netIdx += 1) {
    file << netEdgeId[netIdx].size() << endl;
    for (int netEdgeIdx = 0; netEdgeIdx < netEdgeId[netIdx].size(); netEdgeIdx += 1) {
      file << netEdgeId[netIdx][netEdgeIdx] << ' ';
      file << netEdgeWeight[netIdx][netEdgeIdx] << endl;
    }
  }
}

void Graph::writeFile(char* filename) {
  vector< vector<int> > netEdgeId(netNum);
  vector< vector<int> > netEdgeWeight(netNum);
  for (int edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
    for (int edgSigIdx = 0; edgSigIdx < _edges[edgeIdx].signals.size(); edgSigIdx += 1) {
      Signal& sig = _edges[edgeIdx].signals[edgSigIdx];
      netEdgeId[sig.netId].push_back(edgeIdx);
      netEdgeWeight[sig.netId].push_back(sig.weight);
    }
  }
  writeFile(filename, netEdgeId, netEdgeWeight);
}

void Graph::readOutputFile(char* filename) {
  fstream file;
  string line;
  int nSignal, edgId, edgWeight;

  file.open(filename);
  if (!file) {
    cerr << "Unable to open file " << filename << endl;
    exit(1);
  }

  for (int netIdx = 0; netIdx < netNum; netIdx += 1) {
    // clear net vector 
    getline(file, line);
    stringstream ss;
    ss << line;
    ss >> nSignal;
    for (int sigIdx = 0; sigIdx < nSignal; sigIdx += 1) {
      getline(file, line);
      stringstream ss2;
      ss2 << line;
      ss2 >> edgId >> edgWeight;
      _edges[edgId].addSignal(netIdx, edgWeight);
      for (int k = 0; k < _nets[netIdx].groupIds.size(); k += 1)
        _netGroups[_nets[netIdx].groupIds[k]].score += edgWeight;
    }
  }
}

void Graph::sillyOut(char* filepath) {
  vector< vector<int> > netEdgeId(netNum);
  vector< vector<int> > netEdgeWeight(netNum);
  for (int edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
    Edge& edg = _edges[edgeIdx];
    for (int edgNetIdx = 0; edgNetIdx < edg.signals.size(); edgNetIdx += 1) {
      int weight = edg.signal;
      if (edg.signal % 2 == 1) {
        if (edgNetIdx < edg.signal / 2) {
          weight = weight - 1;
        } else {
          weight = weight + 1;
        }
      }
      netEdgeId[edg.signals[edgNetIdx].netId].push_back(edgeIdx);
      netEdgeWeight[edg.signals[edgNetIdx].netId].push_back(weight);
    }
    writeFile(filepath, netEdgeId, netEdgeWeight);
  }
}
