#include <vector>
#include <string>
#include <time.h>

using namespace std;

extern clock_t START_TIME;
extern clock_t TEMP_TIME;

void runTimeManage(string errorMsg, bool finish=false);

struct Signal {
  int edgeId;
  int netId;
  int weight;
};

class Edge {
  int getTotalWeight(int signal);
  int nodeId1;
  int nodeId2;
public:
  int id;
  int weight;
  int signal;
  // vector<int> groupSignal;
  // vector<int> signalNets;
  // vector<int> signalWeights;
  vector<Signal> signals;

  Edge() : weight(2), signal(0), set(false), netSignal(0) {}
  void setId(int newId) { id = newId; }
  void setNode(int p1, int p2) { nodeId1 = p1; nodeId2 = p2; }
  void addSignal(int netId, int w);
  void setWeight(int, int);
  int getTo(int nodeId) { return (nodeId1 == nodeId) ? nodeId2 : nodeId1; }
  
  bool set;
  int netSignal;
};


class Node {
  friend ostream &operator<<(ostream &, Node const &);
  struct Neighbor {
    Node* nodePtr;
    Edge* edgePtr;
  };

public:
  int id;
  vector<Neighbor> neighbors;
  void setId(int newId) { id = newId; }
  void addNeighbor(Node*, Edge*);
  
  // Method for graph.traverve()
  int accWeight;
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
  int id;
  vector<int> groupIds;
  Node* source;
  vector<Node*> destinations;
  void setId(int newId) { id = newId; }
  void addGroupId(int newId) { groupIds.push_back(newId); }
  void setSource(Node* ptr) { source = ptr; }
  void addDestination(Node* ptr) { destinations.push_back(ptr); }
};

struct NetGroup {
  int id;
  vector<Net*> nets;
  int score;
};

class Graph {
  friend ostream &operator<<(ostream &, Graph const &);
  Node* _nodes;
  Net* _nets;
  Edge* _edges;
  NetGroup* _netGroups;
  // bool isNetGroupBigger(NetGroup*, NetGroup*);

public:
  int nodeNum, edgeNum, netNum, netGroupNum;
  int nSignal;
  Graph(char* filepath);
  ~Graph() { delete [] _nodes; delete [] _nets; delete [] _edges; delete [] _netGroups; }
  void resetNodesAccWeight();
  void traverse(NetGroup* netGroups = NULL);

  // deal with net group
  void sillyOut(char*);
  void writeFile(char*, vector< vector<int> >&, vector< vector<int> >&);
  void writeFile(char*);
  void readOutputFile(char*);
  void solveILP(float, int, bool, bool);
  void adaptILP();
  void changeOrder();
};

ostream &operator<<(ostream &, Node const &);
ostream &operator<<(ostream &, Net const &);
ostream &operator<<(ostream &, Graph const &);
