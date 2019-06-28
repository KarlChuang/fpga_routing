#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

#include "graph.h"

using namespace std;

#define INT_MAX 294967295

void Node::addNeighbor(Node* nPtr, Edge* ePtr) {
  Node::Neighbor newNeighbor = { nPtr, ePtr };
  neighbors.push_back(newNeighbor);
}

int Edge::getTotalWeight(int signal) {
  if (signal % 2 == 0) {
    return signal * signal;
  } else {
    return (signal - 1) * (signal / 2) + (signal + 1) * ((signal + 1) / 2);
  }
}

void Edge::addSignal(int netId, int w = 0) {
  signal += 1;
  Signal sig = {id, netId, w};
  signals.push_back(sig);
  weight = getTotalWeight(signal + 1) - getTotalWeight(signal);
}

void Edge::setWeight(int netId, int newWeight) {


  for (int i = 0; i < signals.size(); i += 1) {
    if (signals[i].netId == netId) {
      signals[i].weight = newWeight;
      return;
    }
  }
  cout << "Error: Not find the netId " << netId << " in Edge " << id << endl;
  // exit(1);
}

Graph::Graph(char* filepath) {
  ifstream inFile;
  inFile.open(filepath);
  if (!inFile) {
    cerr << "Unable to open file " << filepath << endl;
    // exit(1);
  }
  string line, temp;

  // set nodeNum, edgeNum, netNum, netGroupNum, nSignal
  getline(inFile, line);
  stringstream ss;
  ss << line;
  ss >> nodeNum >> edgeNum >> netNum >> netGroupNum;
  nSignal = 0;
  
  // set _nodes
  _nodes = new Node[nodeNum];

  // set node ID
  for (int i = 0; i < nodeNum; i += 1) {
    _nodes[i].setId(i);
  }

  // set node neightbors
  _edges = new Edge[edgeNum];
  for (int i = 0; i < edgeNum; i += 1) {
    _edges[i].setId(i);
    getline(inFile, line);
    stringstream ss;
    ss << line;
    int from, to;
    ss >> from >> to;
    _edges[i].setNode(from, to);
    _nodes[from].addNeighbor(&_nodes[to], &_edges[i]);
    _nodes[to].addNeighbor(&_nodes[from], &_edges[i]);
  }

  // set net
  _nets = new Net[netNum];
  for (int i = 0; i < netNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    int from, to;
    ss >> from;
    _nets[i].setId(i);
    _nets[i].setSource(&_nodes[from]);
    while (ss >> to) {
      _nets[i].addDestination(&_nodes[to]);
    }
  }

  // set net Group
  _netGroups = new NetGroup[netGroupNum];
  for (int i = 0; i < netGroupNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    int id;
    while (ss >> id) {
      _netGroups[i].nets.push_back(&_nets[id]);
      _nets[id].addGroupId(i);
    }
    _netGroups[i].id = i;
    _netGroups[i].score = 0;
  }
}

void Graph::resetNodesAccWeight() {
  for (int i = 0; i < nodeNum; i += 1) {
    _nodes[i].accWeight = INT_MAX;
    _nodes[i].fromPtrs.clear();
    _nodes[i].toPtrs.clear();
  }
  for (int i = 0; i < edgeNum; i += 1) {
    _edges[i].set = false;
    _edges[i].netSignal = 0;
  }
}

void Node::setNeighborWeight() {
  for (int i = 0; i < neighbors.size(); i += 1) {
    int newWeight = accWeight + neighbors[i].edgePtr->weight;
    if (newWeight < neighbors[i].nodePtr->accWeight) {
      neighbors[i].nodePtr->accWeight = newWeight;
      neighbors[i].nodePtr->fromPtrs.clear();
      neighbors[i].nodePtr->fromPtrs.push_back(neighbors[i].edgePtr);
      neighbors[i].nodePtr->setNeighborWeight();
    } else if (newWeight == neighbors[i].nodePtr->accWeight) {
      vector<Edge*>& newFromPtr = neighbors[i].nodePtr->fromPtrs;

      if (find(newFromPtr.begin(), newFromPtr.end(), neighbors[i].edgePtr) == newFromPtr.end()) {
        newFromPtr.push_back(neighbors[i].edgePtr);
      }
    }
  }
}

void Node::setComingNeighbor(Node* nodes) {
  for (int j = 0; j < fromPtrs.size(); j += 1) {
    Edge* setEdgePtr = fromPtrs[j];
    setEdgePtr->netSignal += 1;
    nodes[setEdgePtr->getTo(id)].setComingNeighbor(nodes);
    vector<Edge*>& newToPtrs = nodes[setEdgePtr->getTo(id)].toPtrs;

    if (find(newToPtrs.begin(), newToPtrs.end(), setEdgePtr) == newToPtrs.end()) {
      newToPtrs.push_back(setEdgePtr);
    }
  }
}

void Node::setGoingNeighbor(Node* nodes) {
  int accSignal = 0;
  for (int i = 0; i < fromPtrs.size(); i += 1) {
    if (fromPtrs[i]->netSignal > accSignal) {
      fromPtrs[0] = fromPtrs[i];
      accSignal = fromPtrs[i]->netSignal;
    }
  }
  for (int j = 0; j < toPtrs.size(); j += 1) {
    Edge* setEdgePtr = toPtrs[j];
    setEdgePtr->netSignal = setEdgePtr->netSignal + accSignal;
    nodes[setEdgePtr->getTo(id)].setGoingNeighbor(nodes);
  }
}

void Node::finalSet(Node* nodes) {
  if (fromPtrs.size() > 0) {
    fromPtrs[0]->set = true;
    nodes[fromPtrs[0]->getTo(id)].finalSet(nodes);
  }
}

void Graph::traverse(NetGroup* netGroups) {
  if (netGroups == NULL) {
    netGroups = _netGroups;
  }
  for (int j = 0; j < netNum; j += 1) {
    resetNodesAccWeight();
    Net* netPtr = &_nets[j];

    netPtr->source->accWeight = 0;
    netPtr->source->setNeighborWeight();

    for (int i = 0; i < netPtr->destinations.size(); i += 1) {
      netPtr->destinations[i]->setComingNeighbor(_nodes);
    }

    netPtr->source->setGoingNeighbor(_nodes);

    for (int i = 0; i < netPtr->destinations.size(); i += 1) {
      netPtr->destinations[i]->finalSet(_nodes);
    }

    for (int i = 0; i < edgeNum; i += 1) {
      if (_edges[i].set) {
        _edges[i].addSignal(netPtr->id);
        nSignal += 1;
      }
    }

    // for (int i = 0; i < edgeNum; i += 1) {
    //   cout << "edge(" << i << ") \tset " << _edges[i].set << " \t\tsignal " << _edges[i].signal << " \tweight " << _edges[i].weight; // << endl;
    //   cout << " \tnet";
    //   for (int k = 0; k < _edges[i].signals.size(); k += 1)
    //     cout << " " << _edges[i].signals[k].netId;
    //   cout << endl;
    // }
    // cout << "--" << endl;
  }
}
