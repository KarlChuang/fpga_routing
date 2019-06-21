#include <vector>
#include <string>
#include <time.h>
#include <cstdint>

using namespace std;

extern clock_t START_TIME;
extern clock_t TEMP_TIME;

void runTimeManage(string errorMsg, bool finish=false);

struct Signal {
  uint edgeId;
  uint netId;
  uint weight;
};

class Edge {
  uint getTotalWeight(uint signal);
  uint nodeId1;
  uint nodeId2;
public:
  uint id;
  uint weight;
  uint signal;
  // vector<uint> groupSignal;
  // vector<uint> signalNets;
  // vector<uint> signalWeights;
  vector<Signal> signals;

  Edge() : weight(2), signal(0), set(false), netSignal(0) {}
  void setId(uint newId) { id = newId; }
  void setNode(uint p1, uint p2) { nodeId1 = p1; nodeId2 = p2; }
  void addSignal(uint netId, uint w);
  void setWeight(uint, uint);
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
  vector<uint> groupIds;
  Node* source;
  vector<Node*> destinations;
  void setId(uint newId) { id = newId; }
  void addGroupId(uint newId) { groupIds.push_back(newId); }
  void setSource(Node* ptr) { source = ptr; }
  void addDestination(Node* ptr) { destinations.push_back(ptr); }
};

struct NetGroup {
  uint id;
  vector<Net*> nets;
  uint score;
};

class Graph {
  friend ostream &operator<<(ostream &, Graph const &);
  Node* _nodes;
  Net* _nets;
  Edge* _edges;
  NetGroup* _netGroups;
  // bool isNetGroupBigger(NetGroup*, NetGroup*);

public:
  uint nodeNum, edgeNum, netNum, netGroupNum;
  int nSignal;
  Graph(char* filepath);
  ~Graph() { delete [] _nodes; delete [] _nets; delete [] _edges; delete [] _netGroups; }
  void resetNodesAccWeight();
  void traverse(NetGroup* netGroups = NULL);

  // deal with net group
  void sillyOut();
  void writeFile(string, vector< vector<int> >&, vector< vector<int> >&);
  void writeFile(string);
  void readOutputFile(string);
  void createILP();
  void adaptILP();
  void changeOrder();
};

ostream &operator<<(ostream &, Node const &);
ostream &operator<<(ostream &, Net const &);
ostream &operator<<(ostream &, Graph const &);
