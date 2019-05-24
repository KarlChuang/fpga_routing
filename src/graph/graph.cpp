#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "graph.h"

using namespace std;

void Node::addNeighbor(Node* nPtr, Edge* ePtr) {
  Node::Neighbor newNeighbor = { nPtr, ePtr };
  neighbors.push_back(newNeighbor);
}

uint Edge::getTotalWeight(uint signal) {
  if (signal % 2 == 0) {
    return signal * signal;
  } else {
    return (signal - 1) * (signal / 2) + (signal + 1) * ((signal + 1) / 2);
  }
}

void Edge::addSignal() {
  signal += 1;
  weight = getTotalWeight(signal + 1) - getTotalWeight(signal);
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
  _edges = new Edge[edgeNum];
  for (uint i = 0; i < edgeNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint from, to;
    ss >> from >> to;
    _edges[i].setNode(from, to);
    _nodes[from].addNeighbor(&_nodes[to], &_edges[i]);
    _nodes[to].addNeighbor(&_nodes[from], &_edges[i]);
  }

  // set net
  _nets = new Net[netNum];
  for (uint i = 0; i < netNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint from, to;
    ss >> from;
    _nets[i].setId(i);
    _nets[i].setSource(&_nodes[from]);
    while (ss >> to) {
      _nets[i].addDestination(&_nodes[to]);
    }
  }

  // set net Group
  _netGroups = new NetGroup[netGroupNum];
  for (uint i = 0; i < netGroupNum; i += 1) {
    getline(inFile, line);
    stringstream ss;
    ss << line;
    uint id;
    while (ss >> id) {
      _netGroups[i].nets.push_back(&_nets[id]);
    }
  }
}

void Graph::resetNodesAccWeight() {
  for (int i = 0; i < nodeNum; i += 1) {
    _nodes[i].accWeight = UINT_MAX;
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
    uint newWeight = accWeight + neighbors[i].edgePtr->weight;
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
  uint accSignal = 0;
  for (int i = 0; i < fromPtrs.size(); i += 1) {
    if (fromPtrs[i]->netSignal > accSignal) {
      fromPtrs[0] = fromPtrs[i];
      accSignal = fromPtrs[i]->netSignal;
    }
  }
  // cout << id << " " << accSignal << endl;
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

void Graph::traverse() {
  for (int netIdx = 0; netIdx < netNum; netIdx += 1) {
    resetNodesAccWeight();
    Net& net = _nets[netIdx];
    net.source->accWeight = 0;
    net.source->setNeighborWeight();
    // for (int i = 0; i < nodeNum; i += 1) {
    //   if (_nodes[i].fromPtrs.size() > 0) {
    //     cout << "node(" << i << ") \tfrom [ ";
    //     for (int j = 0; j < _nodes[i].fromPtrs.size(); j += 1) {
    //       cout << _nodes[i].fromPtrs[j]->getTo(&_nodes[i])->id << ' ';
    //     }
    //     cout << "] \taccumulated weight ";
    //     cout << _nodes[i].accWeight << endl;
    //   } else {
    //     cout << "node(" << i << ") \tSOURCE" << " \t\taccumulated weight " << _nodes[i].accWeight << endl;
    //   }
    // }
    // cout << "---" << endl;
    for (int i = 0; i < net.destinations.size(); i += 1) {
      net.destinations[i]->setComingNeighbor(_nodes);
    }
    // for (int i = 0; i < edgeNum; i += 1) {
    //   cout << "edge(" << i << ") \tnetSignal " << _edges[i].netSignal << endl;
    // }
    // for (int i = 0; i < nodeNum; i += 1) {
    //   cout << "node(" << i << ") to [";
    //   for (int j = 0; j < _nodes[i].toPtrs.size(); j += 1) {
    //     cout << _nodes[i].toPtrs[j]->getTo(&_nodes[i])->id << ' ';
    //   }
    //   cout << ']' << endl;
    // }
    net.source->setGoingNeighbor(_nodes);
    // for (int i = 0; i < edgeNum; i += 1) {
    //   cout << "edge(" << i << ") \tnetSignal " << _edges[i].netSignal << endl;
    // }
    for (int i = 0; i < net.destinations.size(); i += 1) {
      net.destinations[i]->finalSet(_nodes);
    }
    for (int i = 0; i < edgeNum; i += 1) {
      if (_edges[i].set) {
        _edges[i].addSignal();
      }
    }
    for (int i = 0; i < edgeNum; i += 1) {
      cout << "edge(" << i << ") \tset " << _edges[i].set << " \t\tsignal " << _edges[i].signal << " \tweight " << _edges[i].weight << endl;
    }
    cout << "--" << endl;
  }
}
