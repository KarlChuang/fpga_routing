#include <iostream>
#include <algorithm>
#include <vector>
// #include <sstream>
// #include <cstring>
// #include <math.h>
#include <iomanip>

#include "Dense"
#include "graph.h"

using namespace Eigen;
using namespace std;

void computeEdgeConstraints(VectorXd& vars, vector< vector<int> >& edgeColNum, vector<double>& edgeConstraints) {
  for (int i = 0; i < edgeColNum.size(); i += 1) {
    double constraint = 0;
    for (int j = 0; j < edgeColNum[i].size(); j += 1) {
      double var = vars(edgeColNum[i][j]);
      constraint += 1.0 / (2.0 * var);
    }
    edgeConstraints[i] = constraint;
  }
}

void computeGroupScores(VectorXd& vars, vector< vector<int> >& groupColNum, vector<double>& groupScores) {
  for (int i = 0; i < groupColNum.size(); i += 1) {
    double score = 0;
    for (int j = 0; j < groupColNum[i].size(); j += 1) {
      double var = vars(groupColNum[i][j]);
      score += 2.0 * var;
    }
    groupScores[i] = score;
  }
}

void computeGradient(
  double t,
  VectorXd& vars,
  vector<double>& edgeConstraints,
  vector<double>& groupScores,
  vector<int>& colEdgeId,
  vector< vector<int> >& colGroupId,
  VectorXd& gradient
) {
  for (int i = 0; i < vars.size() - 1; i += 1) {
    gradient(i) = (-1.0 / (2 * t * vars(i) * vars(i)));
    gradient(i) /= (1.0 - edgeConstraints[colEdgeId[i]]);
    for (int j = 0; j < colGroupId[i].size(); j += 1) {
      double addGradient = (2.0 / t);
      addGradient /= (vars(vars.size() - 1) - groupScores[colGroupId[i][j]]);
      gradient(i) += addGradient;
    }
    gradient(i) += (-1.0 / (t * (vars(i) - 1)));
    // cout << gradient(i) << endl;
    // cin.get();
  }
  gradient(vars.size() - 1) = 1.0;
  for (int i = 0; i < groupScores.size(); i += 1) {
    gradient(vars.size() - 1) += (-1.0 / (t * (vars(vars.size() - 1) - groupScores[i])));
  }
}

void computeHessian(
  double t,
  VectorXd& vars,
  vector<double>& edgeConstraints,
  vector<double>& groupScores,
  vector<int>& colEdgeId,
  vector< vector<int> >& colGroupId,
  MatrixXd& hessian
)  {
  for (int i = 0; i < vars.size() - 1; i += 1) {
    for (int j = 0; j <= i; j += 1) {
      double deno = (1.0 - edgeConstraints[colEdgeId[i]]);
      double mole = 0.0;
      if (i == j) {
        mole = (-1.0 / (vars(i) * vars(i) * vars(i))) * deno;
        mole -= (1.0 / (4 * vars(i) * vars(i) * vars(i) * vars(i)));
      } else {
        if (colEdgeId[i] == colEdgeId[j]) {
          mole = (-1.0 / (4 * vars(i) * vars(i) * vars(j) * vars(j)));
        }
      }
      hessian(i,j) = (-1.0 / t) * mole / (deno * deno);
      hessian(j,i) = (-1.0 / t) * mole / (deno * deno);
      // cout << setprecision(7) << hessian(i,j) << endl;

      if (i == j) {
        for (int k = 0; k < colGroupId[i].size(); k += 1) {
          double addHessian = (4.0 / t);
          addHessian /= (vars(vars.size() - 1) - groupScores[colGroupId[i][k]]);
          addHessian /= (vars(vars.size() - 1) - groupScores[colGroupId[i][k]]);
          hessian(i,j) += addHessian;
        }
        hessian(i,j) += (1.0 / (t * (vars(i) - 1) * (vars(i) - 1)));
      } else {
        vector<int> v(colGroupId[i].size() + colGroupId[j].size()); 
        vector<int>::iterator it, st;
        sort(colGroupId[i].begin(), colGroupId[i].end());
        sort(colGroupId[j].begin(), colGroupId[j].end());
        it = set_intersection(colGroupId[i].begin(), 
                              colGroupId[i].end(), 
                              colGroupId[j].begin(), 
                              colGroupId[j].end(), 
                              v.begin());
        if (st != it) {
          for (st = v.begin(); st != it; ++st) {
            double addHessian = (4.0 / t);
            addHessian /= (vars(vars.size() - 1) - groupScores[*st]);
            addHessian /= (vars(vars.size() - 1) - groupScores[*st]);
            hessian(i,j) += addHessian;
            hessian(j,i) += addHessian;
          }
        }
      }

      // cout << setprecision(7) << hessian(i,j) << endl;
      // cin.get();
    }
  }
  for (int i = 0; i < vars.size() - 1; i += 1) {
    hessian(i,vars.size() - 1) = 0;
    hessian(vars.size() - 1,i) = 0;
    for (int k = 0; k < colGroupId[i].size(); k += 1) {
      double addHessian = (-2.0 / t);
      addHessian /= (vars(vars.size() - 1) - groupScores[colGroupId[i][k]]);
      addHessian /= (vars(vars.size() - 1) - groupScores[colGroupId[i][k]]);
      hessian(i,vars.size() - 1) += addHessian;
      hessian(vars.size() - 1,i) += addHessian;
    }
  }
  hessian(vars.size() - 1,vars.size() - 1) = 0.0;
  for (int i = 0; i < groupScores.size(); i += 1) {
    double addHessian = (1.0 / t);
    addHessian /= (vars(vars.size() - 1) - groupScores[i]);
    addHessian /= (vars(vars.size() - 1) - groupScores[i]);
    hessian(vars.size() - 1,vars.size() - 1) += addHessian;
  }
}

void Graph::solveConvex(double t) {
  vector< vector<int> > netEdgeId(netNum);
  vector< vector<int> > netColNum(netNum);
  vector< vector<int> > netEdgeWeight(netNum);

  int Ncol = nSignal + 1;
  VectorXd vars(Ncol);

  int j = 0;
  double initWeight = 0.0;
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
      vars(j) = double(weight) + 0.1;
      initWeight += vars(j);
      // set column name
      // int signalId = 1000000 * (edgeIdx) + edg.signals[edgNetIdx].netId;
      // stringstream ss;
      // ss << "x" << signalId;
      // string str = ss.str();
      // char* cstr = &str[0];
      // set_col_name(lp, j, cstr);
      j += 1;
    }
  }
  vars(j) = initWeight;
  // set_col_name(lp, j, "t");
  runTimeManage("Save netEdgeId, netColNum; set column name, bounds, and basis");

  vector< vector<int> > edgeColNum(edgeNum);
  vector< vector<int> > groupColNum(netGroupNum);
  vector<int> colEdgeId(Ncol - 1);
  vector< vector<int> > colGroupId(Ncol - 1);
  j = 0;
  for (int edgeIdx = 0; edgeIdx < edgeNum; edgeIdx += 1) {
    Edge& edg = _edges[edgeIdx];
    for (int edgNetIdx = 0; edgNetIdx < edg.signals.size(); edgNetIdx += 1) {
      // set edge constrain
      edgeColNum[edgeIdx].push_back(j);
      colEdgeId[j] = edgeIdx;
      j += 1;
    }
  }
  runTimeManage("Set edge constrain");

  for (int netGroupIdx = 0; netGroupIdx < netGroupNum; netGroupIdx += 1) {
    for (int nId = 0; nId < _netGroups[netGroupIdx].nets.size(); nId += 1) {
      int netIdx = _netGroups[netGroupIdx].nets[nId]->id;
      for (int k = 0; k < netEdgeId[netIdx].size(); k += 1) {
        // set net group constrain
        groupColNum[netGroupIdx].push_back(netColNum[netIdx][k]);
        colGroupId[netColNum[netIdx][k]].push_back(netGroupIdx);
      }
    }
  }
  runTimeManage("Set net group constrain");



  vector<double> edgeConstraints(edgeNum);
  vector<double> groupScores(netGroupNum);
  VectorXd gradient(Ncol);
  MatrixXd hessain(Ncol, Ncol);
  computeEdgeConstraints(vars, edgeColNum, edgeConstraints);
  // for (int i = 0; i < edgeNum; i += 1) {
  //   cout << edgeConstraints[i] << endl;
  // }
  computeGroupScores(vars, groupColNum, groupScores);
  // for (int i = 0; i < netGroupNum; i += 1) {
  //   cout << groupScores[i] << endl;
  // }
  computeGradient(t, vars, edgeConstraints, groupScores, colEdgeId, colGroupId, gradient);
  // cout << gradient << endl;
  runTimeManage("Compute Gradient");

  computeHessian(t, vars, edgeConstraints, groupScores, colEdgeId, colGroupId, hessain);
  runTimeManage("Compute Hessian");

  VectorXd step(Ncol);
  step = -hessain.inverse() * gradient;
  runTimeManage("Compute step");
}
