
#include "SortRelated.h"

template <class NodeT, class DriverT>
void SortNode<NodeT, DriverT>::radixSortGreatDemand(std::vector<NodeT *> &nodes,
                                                    double max_demand) {
  std::random_device rd;
  std::uniform_real_distribution<> dis(0, 1);

  int max_num_digits = log10(max_demand) + 1;

  std::vector<std::vector<NodeT *>> buckets(10);
  int factor = 1;

  for (int i = 0; i < max_num_digits; ++i) {
    for (NodeT *node : nodes) {
      int bucket_idx = int(double(node->demand) / factor) % 10;
      buckets[bucket_idx].push_back(node);
    }

    int node_idx = 0;
    for (int j = buckets.size() - 1; j >= 0; --j) {
      for (NodeT *node : buckets[j]) {
        nodes[node_idx++] = node;
      }
      buckets[j].clear();
    }
    factor *= 10;
  }

  // randomize nodes with same demand
  int n = nodes.size();
  for (int i = 0; i < n; i++) {
    if (i + 1 < n && nodes[i]->demand == nodes[i + 1]->demand) {
      if (nodes[i]->early_tw > nodes[i + 1]->early_tw)
        std::swap(nodes[i], nodes[i + 1]);
    }
  }
}

template <class NodeT, class DriverT>
void SortNode<NodeT, DriverT>::radixSortEarlyTW(std::vector<NodeT *> &nodes,
                                                double max_early_tw) {
  std::random_device rd;
  
  std::uniform_real_distribution<> dis(0, 1);

  int max_num_digits = log10(max_early_tw) + 1;

  std::vector<std::vector<NodeT *>> buckets(10);
  int factor = 1;

  for (int i = 0; i < max_num_digits; ++i) {
    for (NodeT *node : nodes) {
      int bucket_idx = int(double(node->early_tw) / factor) % 10;
      buckets[bucket_idx].push_back(node);
    }

    int node_idx = 0;
    for (int j = 0; j <= buckets.size() - 1; ++j) {
      for (NodeT *node : buckets[j]) {
        nodes[node_idx++] = node;
      }
      buckets[j].clear();
    }
    factor *= 10;
  }

  // randomize nodes with same early tw
  int n = nodes.size();
  for (int i = 0; i < n; i++) {
    if (i + 1 < n && nodes[i]->early_tw == nodes[i + 1]->early_tw) {
      // if (dis(Parameters::RANDOM_GEN) < nodes[i]->demand / (nodes[i]->demand + nodes[i +
      // 1]->demand)) 	std::swap(nodes[i], nodes[i + 1]);
      if (nodes[i]->late_tw > nodes[i + 1]->late_tw)
        std::swap(nodes[i], nodes[i + 1]);
    }
  }
}

template <class NodeT, class DriverT>
void SortNode<NodeT, DriverT>::radixSortLateTW(std::vector<NodeT *> &nodes,
                                               double max_late_tw) {
  std::random_device rd;
  
  std::uniform_real_distribution<> dis(0, 1);

  int max_num_digits = log10(max_late_tw) + 1;

  std::vector<std::vector<NodeT *>> buckets(10);
  int factor = 1;

  for (int i = 0; i < max_num_digits; ++i) {
    for (NodeT *node : nodes) {
      int bucket_idx = int(double(node->late_tw) / factor) % 10;
      buckets[bucket_idx].push_back(node);
    }

    int node_idx = 0;
    for (int j = 0; j <= buckets.size() - 1; ++j) {
      for (NodeT *node : buckets[j]) {
        nodes[node_idx++] = node;
      }
      buckets[j].clear();
    }
    factor *= 10;
  }

  // randomize nodes with same early tw
  int n = nodes.size();
  for (int i = 0; i < n; i++) {
    if (i + 1 < n && nodes[i]->late_tw == nodes[i + 1]->late_tw) {
      // if (dis(Parameters::RANDOM_GEN) < nodes[i]->demand / (nodes[i]->demand +
      // nodes[i+1]->demand)) 	std::swap(nodes[i], nodes[i + 1]);
      if (nodes[i]->early_tw > nodes[i + 1]->early_tw)
        std::swap(nodes[i], nodes[i + 1]);
      // if (dis(Parameters::RANDOM_GEN) < 0.5)
      //	std::swap(nodes[i], nodes[i + 1]);
    }
  }
}

template <class NodeT, class DriverT>
void SortNode<NodeT, DriverT>::radixSortKinable(std::vector<NodeT *> &nodes,
                                                double max_late_tw) {
  std::random_device rd;
  
  std::uniform_real_distribution<> dis(0, 1);

  int max_num_digits = log10(max_late_tw) + 1;

  std::vector<std::vector<NodeT *>> buckets(10);
  int factor = 1;

  for (int i = 0; i < max_num_digits; ++i) {
    for (NodeT *node : nodes) {
      int bucket_idx = int(double(node->late_tw) / factor) % 10;
      buckets[bucket_idx].push_back(node);
    }

    int node_idx = 0;
    for (int j = 0; j <= buckets.size() - 1; ++j) {
      for (NodeT *node : buckets[j]) {
        nodes[node_idx++] = node;
      }
      buckets[j].clear();
    }
    factor *= 10;
  }

  int n = nodes.size();
  for (int i = 0; i < n; i++) {
    if (i + 1 < n && nodes[i]->late_tw == nodes[i + 1]->late_tw) {
      if (nodes[i]->demand < nodes[i + 1]->demand) {
        std::swap(nodes[i], nodes[i + 1]);
      } else if (nodes[i]->demand < nodes[i + 1]->demand) {
        if (nodes[i]->early_tw > nodes[i + 1]->early_tw) {
          std::swap(nodes[i], nodes[i + 1]);
        }
      }
    }
  }
}

// nodes is already sort, randomize the order of nodes with same demand
template <class NodeT, class DriverT>
void SortNode<NodeT, DriverT>::SortSameDemand(std::vector<NodeT *> &nodes) {
  std::random_device rd;
  
  std::uniform_real_distribution<> dis(0, 1);

  // randomize nodes with same demand
  int n = nodes.size();
  for (int i = 0; i < n; i++) {
    if (i + 1 < n && nodes[i]->demand == nodes[i + 1]->demand) {

      if (dis(Parameters::RANDOM_GEN) < 0.5)
        std::swap(nodes[i], nodes[i + 1]);
    }
  }
}

template <class NodeT, class DriverT>
void SortNode<NodeT, DriverT>::radixSortIncreasingTW(
    std::vector<NodeT *> &nodes, double max_early_tw) {

  int max_num_digits = log10(max_early_tw) + 1;

  std::vector<std::vector<NodeT *>> buckets(10);
  int factor = 1;

  for (int i = 0; i < max_num_digits; ++i) {
    for (NodeT *node : nodes) {
      int bucket_idx = int(double(node->early_tw) / factor) % 10;
      buckets[bucket_idx].push_back(node);
    }

    int node_idx = 0;
    for (int j = 0; j <= buckets.size() - 1; ++j) {
      for (NodeT *node : buckets[j]) {
        nodes[node_idx++] = node;
      }
      buckets[j].clear();
    }
    factor *= 10;
  }

  // randomize nodes with same early tw
  int n = nodes.size();
  for (int i = 0; i < n; i++) {
    if (i + 1 < n && nodes[i]->early_tw == nodes[i + 1]->early_tw) {
      // if (dis(Parameters::RANDOM_GEN) < nodes[i]->demand / (nodes[i]->demand + nodes[i +
      // 1]->demand)) 	std::swap(nodes[i], nodes[i + 1]);
      if (nodes[i]->late_tw > nodes[i + 1]->late_tw)
        std::swap(nodes[i], nodes[i + 1]);
    }
  }
}

template class SortNode<Customer, Driver>;
