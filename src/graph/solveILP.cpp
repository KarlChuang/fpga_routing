#include <iostream>
#include <sstream>
#include <cstring>

#include "lp_lib.h"
#include "graph.h"

void Graph::createILP() {
  vector< vector<int> > netEdgeId(netNum);
  vector< vector<int> > netColNum(netNum);
  vector< vector<int> > netEdgeWeight(netNum);
  uint Ncol = 0;
  // vector< vector<int> > netEdgeWeight(netNum);
  int j = 1;
  for (uint edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
    Edge& edg = _edges[edgeIdx];
    for (uint edgNetIdx = 0; edgNetIdx < edg.signalNets.size(); edgNetIdx += 1) {
      netEdgeId[edg.signalNets[edgNetIdx]].push_back(edgeIdx);
      netColNum[edg.signalNets[edgNetIdx]].push_back(j);
      j += 1;
    }
    Ncol += edg.signal;
  }
  Ncol += 1; // add t
  
  lprec *lp;
  int *colno = NULL, ret = 0;
  REAL *row = NULL;
  lp = make_lp(0, Ncol);
  if(lp == NULL) {
    ret = 1;
    cout << "Error: couldn't construct a new model..." << endl;
  }

  if(ret == 0) {
    j = 1;
    for (uint edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
      Edge& edg = _edges[edgeIdx];
      for (uint edgNetIdx = 0; edgNetIdx < edg.signalNets.size(); edgNetIdx += 1) {
        int signalId = 1000000 * (edgeIdx) + edg.signalNets[edgNetIdx];
        stringstream ss;
        ss << "x" << signalId;
        string str = ss.str();
        char* cstr = (char *)alloca(str.size() + 1);
        memcpy(cstr, str.c_str(), str.size() + 1);
        set_col_name(lp, j, cstr);
        set_int(lp, j, TRUE);
        j += 1;
      }
    }
    set_col_name(lp, j, "t");
    set_int(lp, j, TRUE);
  }

  if(ret == 0) {
    set_add_rowmode(lp, TRUE);
    j = 1;
    for (uint edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
      Edge& edg = _edges[edgeIdx];
      float signalfloat = float(edg.signal);
      colno = (int *) malloc(edg.signal * sizeof(*colno));
      row = (REAL *) malloc(edg.signal * sizeof(*row));
      for (uint edgNetIdx = 0; edgNetIdx < edg.signalNets.size(); edgNetIdx += 1) {
        // int signalId = 1000000 * (edgeIdx) + edg.signalNets[edgNetIdx];
        colno[edgNetIdx] = j;
        row[edgNetIdx] = 1;
        j += 1;
      }
      if (edg.signalNets.size() > 0) {
        float constrain = signalfloat * signalfloat / 2;
        if(!add_constraintex(lp, edg.signal, row, colno, GE, constrain))
          ret = 3;
      }
      free(row);
      free(colno);
    }

    for (uint netGroupIdx = 0; netGroupIdx < netGroupNum; netGroupIdx += 1) {
      int size = 0;
      for (uint nId = 0; nId < _netGroups[netGroupIdx].nets.size(); nId += 1) {
        int netIdx = _netGroups[netGroupIdx].nets[nId]->id;
        size += netEdgeId[netIdx].size();
      }
      size += 1; // add t
      colno = (int *) malloc(size * sizeof(*colno));
      row = (REAL *) malloc(size * sizeof(*row));

      j = 0;
      for (uint nId = 0; nId < _netGroups[netGroupIdx].nets.size(); nId += 1) {
        int netIdx = _netGroups[netGroupIdx].nets[nId]->id;
        for (uint k = 0; k < netEdgeId[netIdx].size(); k += 1) {
          colno[j] = netColNum[netIdx][k];
          row[j] = 2;
          j += 1;
        }
      }
      colno[size - 1] = Ncol;
      row[size - 1] = -1;
      j += 1;
      if (size > 1) {
        if(!add_constraintex(lp, size, row, colno, LE, 0))
          ret = 3;
      }
      free(row);
      free(colno);
    }
    set_add_rowmode(lp, FALSE);
  }

  if(ret == 0) {
    colno = (int *) malloc(1 * sizeof(*colno));
    row = (REAL *) malloc(1 * sizeof(*row));
    for (int i = 1; i < Ncol; i += 1) {
      colno[0] = i;
      row[0] = 1;
      if(!add_constraintex(lp, 1, row, colno, GE, 1))
        ret = 3;
    }
    free(row);
    free(colno);
  }

  if(ret == 0) {
    colno = (int *) malloc(1 * sizeof(*colno));
    row = (REAL *) malloc(1 * sizeof(*row));

    colno[0] = Ncol;
    row[0] = 1;

    /* set the objective in lpsolve */
    if(!set_obj_fnex(lp, 1, row, colno))
      ret = 4;
    free(row);
    free(colno);
  }

  if (ret == 0) {
    set_minim(lp);
    // write_LP(lp, stdout);
    set_verbose(lp, IMPORTANT);
    ret = solve(lp);
    if(ret == OPTIMAL)
      ret = 0;
    else
      ret = 5;
  }

  if(ret == 0) {
    row = (REAL *) malloc(Ncol * sizeof(*row));
    printf("Objective value: %f\n", get_objective(lp));
    get_variables(lp, row);
    for (uint nId = 0; nId < netNum; nId += 1) {
      for(j = 0; j < netColNum[nId].size(); j++) {
        int weight = row[netColNum[nId][j] - 1] * 2;
        netEdgeWeight[nId].push_back(weight);
        // cout << netEdgeId[nId][j] << ' ' << get_col_name(lp, netColNum[nId][j]) << ' ' << weight << endl;
      }
    }
      // printf("%s: %f\n", get_col_name(lp, j + 1), row[j]);
    free(row);
  }

  if(lp != NULL) {
    delete_lp(lp);
  }

  writeFile("output/sampleOutput.txt", netEdgeId, netEdgeWeight);

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