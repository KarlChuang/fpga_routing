#include <vector>

using namespace std;

class Graph {
  public:
    uint nodeNum, edgeNum, netNum, netGroupNum;
    Graph(char* filepath);
    void sayhi();
};

class Node {
  struct Neighbor {
    Node* n;
    uint edgId;
  };
  public:
    Node();
    vector<Neighbor> neighbors;
};


