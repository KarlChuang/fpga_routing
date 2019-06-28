#include <iostream>
#include <iomanip>
#include <cstring>

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
  if (argc >= 4) {
    char* filepath = argv[1];
    char* outputfile = argv[2];
    char* mode = argv[3];
    Graph g = Graph(filepath);
    // cout << g << endl;
    runTimeManage("Create graph");

    g.traverse();
    runTimeManage("Traverse graph");

    if (strcmp(mode, "ilp") == 0) {
      float acc_range = 0.11;
      int timeout = 3600;
      bool setInt = false;
      bool outputResult = false;
      g.solveILP(acc_range, timeout, setInt, outputResult);
      runTimeManage("Solve ILP finished");

      g.adaptILP();
      runTimeManage("Adapt ILP result");
      g.writeFile(outputfile);
    } else if (strcmp(mode, "greedy") == 0) {
      g.sillyOut(outputfile);
      runTimeManage("Silly output");
    }
    runTimeManage("total time", true);
  } else {
    cout << "Please run `./bin/main ${input file} ${output file} ${mode}`" << endl;
  }
}
