#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>

#include "graph.h"

using namespace std;

struct signalScore {
  int signal;
  int score;
  int weight;
};

static bool isSCSmaller(signalScore& sc1, signalScore& sc2) {
  // if (sc1.weight == sc2.weight)
  //   return sc1.score < sc2.score;
  // return sc1.weight < sc2.weight;
  if (sc1.score == sc2.score)
    return sc1.weight < sc2.weight;
  return sc1.score < sc2.score;
}

void Graph::adaptILP() {
  vector<signalScore> signalSort;
  for (int i = 0; i < edgeNum; i += 1) {
    // sort the signal
    double constrainScore = 0;
    signalSort.resize(_edges[i].signals.size());
    for (int j = 0; j < _edges[i].signals.size(); j += 1) {
      int signalNetId = _edges[i].signals[j].netId;
      int signalWeight = _edges[i].signals[j].weight;
      Net& net = _nets[signalNetId];
      int scoreMax = 0;
      for (int k = 0; k < net.groupIds.size(); k += 1) {
        int signalGroupId = net.groupIds[k];
        int signalGroupScore = _netGroups[signalGroupId].score;
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
      int signalNetId = _edges[i].signals[changedSignalIdx].netId;
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
        int signalGroupId = net.groupIds[k];
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
