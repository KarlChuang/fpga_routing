#include <iostream>
#include <fstream>
#include <math.h>

#include "graph.h"

using namespace std;

void Graph::sillyOut() {
  fstream file;
  file.open("output/samepleOutput.txt", ios::out | ios::trunc);
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
  }

  for (uint netIdx = 0; netIdx < netEdgeId.size(); netIdx += 1) {
    file << netEdgeId[netIdx].size() << endl;
    for (uint netEdgeIdx = 0; netEdgeIdx < netEdgeId[netIdx].size(); netEdgeIdx += 1) {
      file << netEdgeId[netIdx][netEdgeIdx] << ' ';
      file << netEdgeWeight[netIdx][netEdgeIdx] << endl;
    }
  }
  // file << "aaaa" << endl;
}

void Graph::createILP() {
  cout << "---" << endl;
  for (int i = 0; i < edgeNum; i += 1) {
    cout << "edge(" << i << ") \tsignal " << _edges[i].signal;
    cout << " \tgroup signal";
    for (int k = 0; k < _edges[i].groupSignal.size(); k += 1)
      cout << " " << _edges[i].groupSignal[k];
    cout << endl;
  }
  cout << "---" << endl;

  cout << "min t" << endl;
  vector< vector<int> > groupTemp(netGroupNum);
  for (int i = 0; i < edgeNum; i += 1) {
    float signalNum = float(_edges[i].signal);
    int j = 1;
    if (_edges[i].signal > 0) {
      for (int k = 0; k < _edges[i].groupSignal.size(); k += 1) {
        for (int r = 0; r < _edges[i].groupSignal[k]; r += 1) {
          int signalId = 1000000 * (i + 1) + j;
          if (j == 1)
            cout << "e" << signalId;
          else
            cout << " + " << "e" << signalId;
          j += 1;
          groupTemp[k].push_back(signalId);
        }
      }
      cout << " >= " << ceil(signalNum * signalNum / 2) << " >= " << signalNum * signalNum / 2 << endl;
    }
  }
  for (int i = 0; i < netGroupNum; i += 1) {
    for (int j = 0; j < groupTemp[i].size(); j += 1) {
      if (j == 0)
        cout << "2*e" << groupTemp[i][j];
      else
        cout << " + 2*e" << groupTemp[i][j];
    }
    cout << " <= t" << endl;
  }
}

void Graph::changeOrder() {
  cout << "Change Order!" << endl;
  return;
}
