#include <iostream>

#include "graph.h"

using namespace std;

ostream &operator<<(ostream &os, Net const &m) {
  os << "(Group";
  for (int i = 0; i < m.groupIds.size(); i += 1)
    os << ' ' << m.groupIds[i];
  os << ") "<< m.source->id << " ->";
  for (int i = 0; i < m.destinations.size(); i += 1) {
    os << ' ' << m.destinations[i]->id;
  }
  return os;
}

ostream &operator<<(ostream &os, Node const &m) {
  os << m.id << " ->";
  for (int i = 0; i < m.neighbors.size(); i += 1) {
    os << ' ' << m.neighbors[i].nodePtr->id;
  }
  return os;
}

ostream &operator<<(ostream &os, Graph const &m) {
  os << "node: " << m.nodeNum << " edge: " << m.edgeNum << " net: " << m.netNum << " net group: " << m.netGroupNum << endl;
  os << "link:" << endl;
  for (int i = 0; i < m.nodeNum; i += 1) {
    os << m._nodes[i] << endl;
  }
  os << "net:" << endl;
  for (int i = 0; i < m.netNum; i += 1) {
    os << m._nets[i] << endl;
  }
  os << "net group:" << endl;
  for (int i = 0; i < m.netGroupNum; i += 1) {
    for (int j = 0; j < m._netGroups[i].nets.size(); j += 1) {
      os << m._netGroups[i].nets[j]->id << ' ';
    }
    os << endl;
  }
  return os;
}
