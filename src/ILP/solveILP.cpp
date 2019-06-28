#include <iostream>
#include <sstream>
#include <cstring>
#include <math.h>

#include "lp_lib.h"
#include "graph.h"

void Graph::solveILP(float acc_range, int timeout, bool setInt, bool outputResult) {
  vector< vector<int> > netEdgeId(netNum);
  vector< vector<int> > netColNum(netNum);
  vector< vector<int> > netEdgeWeight(netNum);
  int Ncol = nSignal + 1;
  lprec *lp;
  int *colno = NULL, *colno2 = NULL, ret = 0;
  REAL *row = NULL, *row2 = NULL;

  // create lp
  lp = make_lp(0, Ncol);
  if(lp == NULL) {
    ret = 1;
    cout << "Error: couldn't construct a new model..." << endl;
  }
  runTimeManage("create lp");

  // alloccate colume temp memory
  colno = (int *) malloc((Ncol + 1) * sizeof(*colno));
  row = (REAL *) malloc((Ncol + 1) * sizeof(*row));
  colno2 = (int *) malloc(2 * sizeof(*colno2));
  row2 = (REAL *) malloc(2 * sizeof(*row2));
  if((colno == NULL) || (row == NULL) || (colno2 == NULL) || (row2 == NULL)) {
    ret = 2;
    cout << "Error: memory allocate error" << endl;
  }
  runTimeManage("Alocate memory");

  // set column variable
  int j = 1;
  for (int edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
    Edge& edg = _edges[edgeIdx];
    for (int edgNetIdx = 0; edgNetIdx < edg.signals.size(); edgNetIdx += 1) {
      netEdgeId[edg.signals[edgNetIdx].netId].push_back(edgeIdx);
      netColNum[edg.signals[edgNetIdx].netId].push_back(j);

      // set edge basis weight
      int weight = edg.signal;
      if (edg.signal % 2 == 1) {
        if (edgNetIdx < edg.signal / 2) {
          weight = weight - 1;
        } else {
          weight = weight + 1;
        }
      }
      weight = weight / 2;
      colno[j] = weight;

      // set column name
      int signalId = 1000000 * (edgeIdx) + edg.signals[edgNetIdx].netId;
      stringstream ss;
      ss << "x" << signalId;
      string str = ss.str();
      char* cstr = &str[0];
      set_col_name(lp, j, cstr);

      // set vcolumn to integer and > 0
      if (setInt)
        set_int(lp, j, TRUE);
      // int acc_range = 30;
      set_bounds(lp, j, (((weight - weight * acc_range) < 0.5) ? 0.5 : weight - weight * acc_range), weight + weight * acc_range);
      // set_bounds(lp, j, 0.5, weight + acc_range);
      j += 1;
    }
  }
  colno[j] = get_infinite(lp);
  set_col_name(lp, j, "t");
  if (setInt)
    set_int(lp, j, TRUE);
  if (!set_basis(lp, colno, FALSE)) {
    ret = 1;
    cout << "Error: cannot set basis" << endl;
  }
  runTimeManage("Save netEdgeId, netColNum; set column name, bounds, and basis");

  // set edge constrain
  if(ret == 0) {
    set_add_rowmode(lp, TRUE);
    j = 1;
    for (int edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
      Edge& edg = _edges[edgeIdx];
      float signalfloat = float(edg.signal);
      for (int edgNetIdx = 0; edgNetIdx < edg.signals.size(); edgNetIdx += 1) {
        // set edge constrain
        colno[edgNetIdx] = j;
        row[edgNetIdx] = 1;
        j += 1;
      }
      if (edg.signals.size() > 0) {
        float constrain = signalfloat * signalfloat / 2;
        if(!add_constraintex(lp, edg.signal, row, colno, GE, constrain))
          ret = 3;
      }
    }
    runTimeManage("Set edge constrain");

    for (int netGroupIdx = 0; netGroupIdx < netGroupNum; netGroupIdx += 1) {
      j = 0;
      int size = 0;
      for (int nId = 0; nId < _netGroups[netGroupIdx].nets.size(); nId += 1) {
        int netIdx = _netGroups[netGroupIdx].nets[nId]->id;
        for (int k = 0; k < netEdgeId[netIdx].size(); k += 1) {
          // set net group constrain        
          colno[j] = netColNum[netIdx][k];
          row[j] = 2;
          j += 1;
        }
        size += netEdgeId[netIdx].size();
      }
      size += 1;
      colno[size - 1] = Ncol;
      row[size - 1] = -1;
      j += 1;
      if (size > 1) {
        if(!add_constraintex(lp, size, row, colno, LE, 0))
          ret = 3;
      }
    }
    runTimeManage("Set net group constrain");

    set_add_rowmode(lp, FALSE);
  }

  if(ret == 0) {
    colno[0] = Ncol;
    row[0] = 1;
    // set the objective in lpsolve
    if(!set_obj_fnex(lp, 1, row, colno))
      ret = 4;
    runTimeManage("Set objective function");
  }

  if (ret == 0) {
    set_minim(lp);
    // write_LP(lp, stdout);
    set_verbose(lp, IMPORTANT);
    set_timeout(lp, timeout); // timeout 1 min

    runTimeManage("Set min, verbose and timeout");

    ret = solve(lp);

    if(ret == OPTIMAL) {
      ret = 0;
      runTimeManage("ILP solve finds optimal solution");
    } else if (ret == SUBOPTIMAL) {
      ret = 0;
      runTimeManage("ILP solve finds suboptomal solution");  
    } else {
      ret = 5;
      cout << "No solution is found" << endl;
    }
  }

  if(ret == 0) {
    cout << "Objective value: " << int(get_objective(lp)) << endl;
    get_variables(lp, row);
    for (int nId = 0; nId < netNum; nId += 1) {
      for(j = 0; j < netColNum[nId].size(); j++) {
        int weight = round(row[netColNum[nId][j] - 1]) * 2;
        netEdgeWeight[nId].push_back(weight);

        // add weight to the Edge.signalWeights 
        int edgId = netEdgeId[nId][j];
        _edges[edgId].setWeight(nId, weight);

        // set NetGroup.score
        for (int k = 0; k < _nets[nId].groupIds.size(); k += 1)
          _netGroups[_nets[nId].groupIds[k]].score += weight;
      }
    }
  }

  // free memory
  if(row != NULL) free(row);
  if(row2 != NULL) free(row2);
  if(colno != NULL) free(colno);
  if(colno2 != NULL) free(colno2);
  if(lp != NULL) delete_lp(lp);

  if (ret == 0 && outputResult) {
    writeFile("output/output_lp_solve.txt", netEdgeId, netEdgeWeight);
  }

  // cout << "---" << endl;
  // for (int i = 0; i < edgeNum; i += 1) {
  //   cout << "edge(" << i << ") \tsignal " << _edges[i].signal;
  //   cout << " \tgroup signal";
  //   for (int k = 0; k < _edges[i].groupSignal.size(); k += 1)
  //     cout << " " << _edges[i].groupSignal[k];
  //   cout << endl;
  // }
  // cout << "---" << endl;
}