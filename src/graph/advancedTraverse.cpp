#include <iostream>
#include <fstream>
#include <math.h>

#include "graph.h"

using namespace std;

void Graph::writeFile(string filename, vector< vector<int> >& netEdgeId, vector< vector<int> >& netEdgeWeight) {
  fstream file;
  file.open(filename, ios::out | ios::trunc);
  for (uint netIdx = 0; netIdx < netEdgeId.size(); netIdx += 1) {
    file << netEdgeId[netIdx].size() << endl;
    for (uint netEdgeIdx = 0; netEdgeIdx < netEdgeId[netIdx].size(); netEdgeIdx += 1) {
      file << netEdgeId[netIdx][netEdgeIdx] << ' ';
      file << netEdgeWeight[netIdx][netEdgeIdx] << endl;
    }
  }
}

void Graph::sillyOut() {
  vector< vector<int> > netEdgeId(netNum);
  vector< vector<int> > netEdgeWeight(netNum);
  for (uint edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
    Edge& edg = _edges[edgeIdx];
    for (uint edgNetIdx = 0; edgNetIdx < edg.signalNets.size(); edgNetIdx += 1) {
      int weight = edg.signal;
      if (edg.signal % 2 == 1) {
        if (edgNetIdx < edg.signal / 2) {
          weight = weight - 1;
        } else {
          weight = weight + 1;
        }
      }
      netEdgeId[edg.signalNets[edgNetIdx]].push_back(edgeIdx);
      netEdgeWeight[edg.signalNets[edgNetIdx]].push_back(weight);
    }
    writeFile("output/sampleOutput.txt", netEdgeId, netEdgeWeight);
  }
}

void Graph::changeOrder() {
  cout << "Change Order!" << endl;
  return;
}
