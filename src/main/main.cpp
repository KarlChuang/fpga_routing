#include <iostream>
#include <iomanip>
#include "graph.h"
#include "ILP.h"

using namespace std;

clock_t START_TIME;
clock_t TEMP_TIME;

void runTimeManage(string errorMsg, bool finish) {
  float t = ((float)(clock() - TEMP_TIME)) / CLOCKS_PER_SEC;
  if (finish)
    t = ((float)(clock() - START_TIME)) / CLOCKS_PER_SEC;
  int min = int(t / 60);
  float sec = t - 60 * min;
  cout << ">>> " << errorMsg << " (";
  if (min > 0)
    cout << min << " m " << int(sec) << " s) " << endl;
  else if (sec >= 10)
    cout << int(sec) << " s) " << endl;
  else
    cout << fixed << setprecision(2) << sec << " s) " << endl;

  TEMP_TIME = clock();
}

int main(int argc, char** argv) {
  START_TIME = clock();
  TEMP_TIME = clock();
  if (argc >= 2) {
    char* filepath = argv[1];
    Graph g = Graph(filepath);
    // cout << g << endl;
    runTimeManage("Create graph");

    g.traverse();
    runTimeManage("Traverse graph");

    g.createILP();

    // g.sillyOut();
    // runTimeManage("Silly output");

    // g.readOutputFile("output/synopsys01-1_ilp.txt");
    // g.readOutputFile("output/sampleOutput.txt");
    g.adaptILP();
    // runTimeManage("Adapt ILP result");
    g.writeFile("output/sampleOutputAdp.txt");

    runTimeManage("total time", true);
  }
  // demo();
}
