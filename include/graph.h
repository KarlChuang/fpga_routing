#include <vector>

using namespace std;

class Node {
  friend ostream &operator<<(ostream &, Node const &);

public:
  class Edge {
    uint getTotalWeight(uint signal);
    Node* ptr1;
    Node* ptr2;
  public:
    uint weight;
    uint signal;
    bool set;
    uint netSignal;
    void addSignal();
    Edge() : weight(2), signal(0), set(false), netSignal(0) {}
    void setNode(Node* p1, Node* p2) { ptr1 = p1; ptr2 = p2; }
    Node* getTo(Node* from) { return (ptr1 == from) ? ptr2 : ptr1; }
  };
  struct Neighbor {
    Node* nodePtr;
    Edge* edgePtr;
  };

  uint id;
  vector<Neighbor> neighbors;
  void setId(uint newId) { id = newId; }
  void addNeighbor(Node*, Edge*);
  
  // Method for graph.traverve()
  uint accWeight;
  vector<Edge*> fromPtrs;
  vector<Edge*> toPtrs;
  void setNeighborWeight(Edge*);
  void setComingNeighbor();
  void setGoingNeighbor();
  void finalSet();
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
  Node::Edge* _edges;
public:
  uint nodeNum, edgeNum, netNum, netGroupNum;
  Graph(char* filepath);
  ~Graph() { delete [] _nodes; delete [] _nets; delete [] _edges; }
  void resetNodesAccWeight();
  void traverse();
};

ostream &operator<<(ostream &, Node const &);
ostream &operator<<(ostream &, Net const &);
ostream &operator<<(ostream &, Graph const &);
