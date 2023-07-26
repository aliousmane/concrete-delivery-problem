
#ifndef SORT_RELATED_H
#define SORT_RELATED_H

#include "Driver.h"
#include "Node.h"
#include "Relatedness.h"
#include "RemoveOperator.h"
#include "lib/mathfunc.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <vector>

template <class NodeT, class DriverT> struct SortDriver {
  NodeT *selectedNode;
  Relatedness<NodeT, DriverT> *rel;

  bool operator()(NodeT *n1, NodeT *n2) {
    return (rel->GetRelatedness(n1, selectedNode) <
            rel->GetRelatedness(n2, selectedNode));
  }
};
template <class NodeT, class DriverT> class SortNode {
public:
  static void radixSortGreatDemand(std::vector<NodeT *> &nodes, double max_demand);
  static void radixSortEarlyTW(std::vector<NodeT *> &nodes, double max_early_tw);
  static void radixSortMinWidthTW(std::vector<NodeT *> &nodes, double max_early_tw);
  static void radixSortIncreasingTW(std::vector<NodeT *> &nodes, double max_early_tw);
  static void radixSortKinable(std::vector<NodeT *> &nodes, double max_late_tw);
  static void radixSortLateTW(std::vector<NodeT *> &nodes, double max_late_tw);
  static void SortSameDemand(std::vector<NodeT *> &nodes);
};
#endif
