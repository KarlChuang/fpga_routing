#include <iostream>
#include "graph.h"

using namespace std;

int main(int argc, char** argv) {
  if (argc >= 2) {
    char* filepath = argv[1];
    Graph g = Graph(filepath);
    // cout << g << endl;
    g.traverse();
  }
}
