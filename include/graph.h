#include <vector>

using namespace std;

class Edge {
  uint getTotalWeight(uint signal);
  uint nodeId1;
  uint nodeId2;
public:
  uint weight;
  uint signal;
  Edge() : weight(2), signal(0), set(false), netSignal(0) {}
  void setNode(uint p1, uint p2) { nodeId1 = p1; nodeId2 = p2; }
  void addSignal();
  uint getTo(uint nodeId) { return (nodeId1 == nodeId) ? nodeId2 : nodeId1; }
  
  bool set;
  uint netSignal;
};


class Node {
  friend ostream &operator<<(ostream &, Node const &);
  struct Neighbor {
    Node* nodePtr;
    Edge* edgePtr;
  };

public:
  uint id;
  vector<Neighbor> neighbors;
  void setId(uint newId) { id = newId; }
  void addNeighbor(Node*, Edge*);
  
  // Method for graph.traverve()
  uint accWeight;
  vector<Edge*> fromPtrs;
  vector<Edge*> toPtrs;
  void setNeighborWeight();
  void setComingNeighbor(Node*);
  void setGoingNeighbor(Node*);
  void finalSet(Node*);
};

class Net {
  friend ostream &operator<<(ostream &, Net const &);
public:
  uint id;
  Node* source;
  vector<Node*> destinations;
  void setId(uint newId) { id = newId; }
  void setSource(Node* ptr) { source = ptr; }
  void addDestination(Node* ptr) { destinations.push_back(ptr); }
};

struct NetGroup {
  vector<Net*> nets;
};

class Graph {
  friend ostream &operator<<(ostream &, Graph const &);
  Node* _nodes;
  Net* _nets;
  Edge* _edges;
  NetGroup* _netGroups;

public:
  uint nodeNum, edgeNum, netNum, netGroupNum;
  Graph(char* filepath);
  ~Graph() { delete [] _nodes; delete [] _nets; delete [] _edges; delete [] _netGroups; }
  void resetNodesAccWeight();
  void traverse();
};

ostream &operator<<(ostream &, Node const &);
ostream &operator<<(ostream &, Net const &);
ostream &operator<<(ostream &, Graph const &);
