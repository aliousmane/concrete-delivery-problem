
#ifndef GRASP_METHOD_H
#define GRASP_METHOD_H

#include "BestSolutionList.h"
#include "IAlgorithm.h"
#include "ISolutionList.h"
#include "InsRmvMethod.h"
#include "InsertOperator.h"
#include "RemoveOperator.h"
#include "Solution.h"
#include "heur/MoveVrp.h"
#include "lib/Chrono.h"
#include "lib/mathfunc.h"
#include <functional>
#include <cmath>
#include <cstring>
#include <unordered_map>

#include "heur/ListMoveVrp.h"


class MyHashFunction {
public:
  size_t operator()(const Sol &s) const {
    return std::hash<std::string>()(s.toString());
  }
  size_t operator()(const std::string &s) const {
    return std::hash<std::string>()(s) << 1;
  };
};

template <class NodeT, class DriverT> struct grasp_remove_operator {
  int no;
  double w;
  double nb_selected;
  double score;
  int nb;
  double interval1;
  double interval2;
  RemoveOperator<NodeT, DriverT> *opt;
};

template <class NodeT, class DriverT> struct grasp_insert_operator {
  InsertOperator<NodeT, DriverT> *opt;
  int no;
  double w;
  double nb_selected;
  double score;
  int nb;
  double interval1;
  double interval2;
};

template <class NodeT, class DriverT>
class GRASP : public IAlgorithm<NodeT, DriverT> {
public:
  GRASP() : _insrmv(nullptr),_data(nullptr) {
    _iterator_count = 25000;
    _temperature = 0.9896;
    _pourcentage_max = 0.4;
    _pourcentage_min = 0.1;
    _max_nb_items = 60;
    _min_nb_items = 30;
    _sigma1 = 4.5;
    _sigma2 = 2.5;
    _sigma3 = 0.5;
    _p = 0.6;
    _temperature_iter_init = 0;
    _acceptation_gap = 0.7;
    _max_time = 99999999;
    _chrono_check_iter = 100;
  }

  GRASP(InsRmvMethod<Node, Driver, MoveVrp> *insrmv, Data *pr)
      : _insrmv(insrmv), _data(pr) {
    _iterator_count = 25000;
    _temperature = 0.9896;
    _pourcentage_max = 0.4;
    _pourcentage_min = 0.1;
    _max_nb_items = 60;
    _min_nb_items = 30;
    _sigma1 = 4.5;
    _sigma2 = 2.5;
    _sigma3 = 0.5;
    _p = 0.6;
    _temperature_iter_init = 0;
    _acceptation_gap = 0.7;
    _max_time = 99999999;
    _chrono_check_iter = 100;
  }
  explicit GRASP(Data *pr) : _insrmv(nullptr), _data(pr) {
    _iterator_count = 25000;
    _temperature = 0.9896;
    _pourcentage_max = 0.4;
    _pourcentage_min = 0.1;
    _max_nb_items = 60;
    _min_nb_items = 30;
    _sigma1 = 4.5;
    _sigma2 = 2.5;
    _sigma3 = 0.5;
    _p = 0.6;
    _temperature_iter_init = 0;
    _acceptation_gap = 0.7;
    _max_time = 99999999;
    _chrono_check_iter = 100;
  }
  InsRmvMethod<Node, Driver, MoveVrp> *_insrmv;
  bool verbose{false};
  void Optimize(Sol &s,bool first_improvement=false);

  void Optimize(Sol &s, ISolutionList<NodeT, DriverT> *best_sol_list);
  void Optimize(Sol &s, ISolutionList<NodeT, DriverT> *best_sol_list, std::unordered_map<std::string, Sol,
   MyHashFunction> * umap, bool first_improvement=false);

  void SetItemMinRemoved(int minp)
  {
    _min_nb_items = minp;
  }
  void SetItemMaxRemoved(int maxp) { _max_nb_items = maxp; }
  void SetPercentageMin(double minp) { _pourcentage_min = minp; }
  void SetPercentageMax(double maxp) { _pourcentage_max = maxp; }
  void SetTemperatureIterInit(double temp) { _temperature_iter_init = temp; }
  void SetTemperature(double temp) { _temperature = temp; }
  void SetIterationCount(int it) { _iterator_count = it; }
  void SetMaxTime(int maxtime) { _max_time = maxtime; }
  void SetChronoCheckIter(int iter) { _chrono_check_iter = iter; }
  void SetAcceptationGap(double a) { _acceptation_gap = a; }
  void AddInsertOperatorLp(InsertOperator<NodeT, DriverT> *opt) {
    grasp_insert_operator<NodeT, DriverT> o;
    o.opt = opt;
    grasp_insert_operators.push_back(o);
  }
  void AddInsertOperatorVrp(InsertOperator<NodeT, DriverT> *opt) {
    grasp_insert_operator<NodeT, DriverT> o;
    o.opt = opt;
    grasp_insert_operators.push_back(o);
  }

  void AddRemoveOperatorLp(RemoveOperator<NodeT, DriverT> *opt) {
    grasp_remove_operator<NodeT, DriverT> o;
    o.opt = opt;
    grasp_remove_operators.push_back(o);
  }
  void AddRemoveOperatorVrp(RemoveOperator<NodeT, DriverT> *opt) {
    grasp_remove_operator<NodeT, DriverT> o;
    o.opt = opt;
    grasp_remove_operators.push_back(o);
  }

  void AddPostAlnsAlgorithm(IAlgorithm<NodeT, DriverT> *algo) {
    _post_alns_algorithms.push_back(algo);
  }

private:
  void Init() {
    for (size_t j = 0; j < grasp_insert_operators.size(); j++) {
      grasp_insert_operators[j].w = 0;
      grasp_insert_operators[j].nb = 0;
      grasp_insert_operators[j].nb_selected = 1;
      grasp_insert_operators[j].score = 1;
    }
    for (size_t j = 0; j < grasp_remove_operators.size(); j++) {
      grasp_remove_operators[j].w = 0;
      grasp_remove_operators[j].nb = 0;
      grasp_remove_operators[j].nb_selected = 1;
      grasp_remove_operators[j].score = 1;
    }
  }
  std::vector<double> upperBound;

  void Update() {
    UpdateVrp();
  }

  void UpdateVrp() {

    for (size_t j = 0; j < grasp_insert_operators.size(); j++) {
      grasp_insert_operators[j].w = grasp_insert_operators[j].w * (1 - _p) +
                                    grasp_insert_operators[j].score /
                                        grasp_insert_operators[j].nb_selected *
                                        _p;
      grasp_insert_operators[j].nb_selected = 1;
      grasp_insert_operators[j].score = 0;
    }

    for (size_t j = 0; j < grasp_remove_operators.size(); j++) {
      grasp_remove_operators[j].w = grasp_remove_operators[j].w * (1 - _p) +
                                    grasp_remove_operators[j].score /
                                        grasp_remove_operators[j].nb_selected *
                                        _p;
      grasp_remove_operators[j].nb_selected = 1;
      grasp_remove_operators[j].score = 0;
    }
  }

  void DoPostAlnsAlgorithms(Sol &sol,
                            ISolutionList<NodeT, DriverT> *best_sol_list) {
    for (size_t i = 0; i < _post_alns_algorithms.size(); i++) {
      if (best_sol_list == nullptr)
        _post_alns_algorithms[i]->Optimize(sol);
      else
        _post_alns_algorithms[i]->Optimize(sol, best_sol_list);
    }
  }

  grasp_remove_operator<NodeT, DriverT> *GetRemoveOperatorVrp() {
    if (grasp_remove_operators.size() == 0)
      return nullptr;
    double sumw = 0;
    size_t i;
    for (i = 0; i < grasp_remove_operators.size(); i++)
      sumw += grasp_remove_operators[i].w;

    double interval = 0;
    for (i = 0; i < grasp_remove_operators.size(); i++) {
      grasp_remove_operators[i].interval1 = interval;
      interval += grasp_remove_operators[i].w / sumw;
      grasp_remove_operators[i].interval2 = interval;
    }

    double k = mat_func_get_rand_double();
    for (i = 0; i < grasp_remove_operators.size(); i++)
      if (grasp_remove_operators[i].interval1 <= k &&
          k <= grasp_remove_operators[i].interval2)
        return &grasp_remove_operators[i];
    return &grasp_remove_operators[grasp_remove_operators.size() - 1];
  }

  grasp_insert_operator<NodeT, DriverT> *GetInsertOperatorVrp() {
    double sumw = 0;
    size_t i;
    for (i = 0; i < grasp_insert_operators.size(); i++)
      sumw += grasp_insert_operators[i].w;

    double interval = 0;
    for (i = 0; i < grasp_insert_operators.size(); i++) {
      grasp_insert_operators[i].interval1 = interval;
      interval += grasp_insert_operators[i].w / sumw;
      grasp_insert_operators[i].interval2 = interval;
    }

    double k = mat_func_get_rand_double();
    for (i = 0; i < grasp_insert_operators.size(); i++)
      if (grasp_insert_operators[i].interval1 <= k &&
          k <= grasp_insert_operators[i].interval2)
        return &grasp_insert_operators[i];
    return &grasp_insert_operators[grasp_insert_operators.size() - 1];
  }
  Data *_data;
  int _iterator_count;
  double _temperature;
  double _pourcentage_max;
  double _pourcentage_min;
  int _max_nb_items;
  int _min_nb_items;
  double _sigma1;
  double _sigma2;
  double _sigma3;
  double _p;
  double _temperature_iter_init;
  double _acceptation_gap;

  std::vector<grasp_remove_operator<NodeT, DriverT>> grasp_remove_operators;
  std::vector<grasp_insert_operator<NodeT, DriverT>> grasp_insert_operators;

  int _max_time;          // max resolution time
  int _chrono_check_iter; // number of iteration before a check of the chrono
                          // (might cost alot)
  ChronoCpuNoStop _chrono;

  std::vector<IAlgorithm<NodeT, DriverT> *> _post_alns_algorithms;
};

#endif
