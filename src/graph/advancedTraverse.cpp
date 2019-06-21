#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <cstdint>

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

void Graph::writeFile(string filename) {
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

void Graph::readOutputFile(string filename) {
  fstream file;
  string line;
  uint nSignal, edgId, edgWeight;

  file.open(filename);
  if (!file) {
    cerr << "Unable to open file " << filename << endl;
    exit(1);
  }

  for (uint netIdx = 0; netIdx < netNum; netIdx += 1) {
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

struct signalScore {
  int signal;
  int score;
  int weight;
};

static bool isSCSmaller(signalScore& sc1, signalScore& sc2) {
  if (sc1.weight == sc2.weight)
    return sc1.score < sc2.score;
  return sc1.weight < sc2.weight;
}

void Graph::adaptILP() {
  vector<signalScore> signalSort;
  for (int i = 0; i < edgeNum; i += 1) {
    // sort the signal
    double constrainScore = 0;
    signalSort.resize(_edges[i].signals.size());
    for (int j = 0; j < _edges[i].signals.size(); j += 1) {
      uint signalNetId = _edges[i].signals[j].netId;
      int signalWeight = _edges[i].signals[j].weight;
      Net& net = _nets[signalNetId];
      int scoreMax = 0;
      for (int k = 0; k < net.groupIds.size(); k += 1) {
        uint signalGroupId = net.groupIds[k];
        uint signalGroupScore = _netGroups[signalGroupId].score;
        if (signalGroupScore > scoreMax)
          scoreMax = signalGroupScore;
      }
      signalScore sc = { j, scoreMax, signalWeight };
      signalSort[j] = sc;
      constrainScore += 1 / double(signalWeight);
      // cout << signalWeight << endl;
    }
    sort(signalSort.begin(), signalSort.end(), isSCSmaller);

    // for (int j = 0; j < signalSort.size(); j += 1)
    //   cout << signalSort[j].signal << ' ' << signalSort[j].weight << ' ' << signalSort[j].score << endl;
    // cout << constrainScore << endl;

    while (constrainScore > 1.0) {
      int changedSignalIdx = signalSort[0].signal;
      int originWeight = _edges[i].signals[changedSignalIdx].weight;
      uint signalNetId = _edges[i].signals[changedSignalIdx].netId;
      // update constain
      constrainScore -= 1 / double(originWeight);
      constrainScore += 1 / double(originWeight + 2);
      // update signal weight of the edge
      _edges[i].signals[changedSignalIdx].weight += 2;
      // update signal sort score
      signalSort[0].score += 2;
      signalSort[0].weight += 2;
      // update net group score
      Net& net = _nets[signalNetId];
      for (int k = 0; k < net.groupIds.size(); k += 1) {
        uint signalGroupId = net.groupIds[k];
        _netGroups[signalGroupId].score += 2;
      }
      // sort signalSort if the first element weight is not smallest
      if (signalSort[0].weight > signalSort[1].weight) {
        sort(signalSort.begin(), signalSort.end(), isSCSmaller);
      }
    }

    // for (int j = 0; j < signalSort.size(); j += 1)
    //   cout << signalSort[j].signal << ' ' << signalSort[j].weight << ' ' << signalSort[j].score << endl;
    // cout << constrainScore << endl;

    // cout << "\r" << i << "/" << edgeNum << ' ' << signalSort.size() << flush;
    // exit(1);
  }
  // cout << endl;
}

void Graph::sillyOut() {
  vector< vector<int> > netEdgeId(netNum);
  vector< vector<int> > netEdgeWeight(netNum);
  for (uint edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
    Edge& edg = _edges[edgeIdx];
    for (uint edgNetIdx = 0; edgNetIdx < edg.signals.size(); edgNetIdx += 1) {
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
    writeFile("output/sampleOutput.txt", netEdgeId, netEdgeWeight);
  }
}

void Graph::changeOrder() {
  cout << "Change Order!" << endl;
  return;
}
