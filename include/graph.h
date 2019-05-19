#include <vector>

using namespace std;

class Node {
  friend ostream &operator<<(ostream &, Node const &);
  struct Neighbor {
    Node* ptr;
    uint edgeId;
  };
public:
  uint id;
  vector<Neighbor> neighbors;
  void setId(uint newId) { id = newId; }
  void addNeighbor(Node*, uint);
};

class Net {
  friend ostream &operator<<(ostream &, Net const &);
public:
  Node* source;
  vector<Node*> destinations;
  void setSource(Node* ptr) { source = ptr; }
  void addDestination(Node* ptr) { destinations.push_back(ptr); }
};

class Graph {
  friend ostream &operator<<(ostream &, Graph const &);
  Node* _nodes;
  Net* _nets;
public:
  uint nodeNum, edgeNum, netNum, netGroupNum;
  Graph(char* filepath);
  ~Graph() { delete [] _nodes; }
};

ostream &operator<<(ostream &, Node const &);
ostream &operator<<(ostream &, Net const &);
ostream &operator<<(ostream &, Graph const &);
