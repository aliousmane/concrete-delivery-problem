/*
 * Copyright Jean-Francois Cote 2012
 *
 * The code may be used for academic, non-commercial purposes only.
 *
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
 */
#ifndef BESTSOLUTIONLIST_H
#define BESTSOLUTIONLIST_H

#include "ISolutionList.h"
#include "Relatedness.h"
#include "Solution.h"
#include <list>
#include <memory>
#include <cstdio>

template <class NodeT, class DriverT>
class BestSolutionList : public ISolutionList<NodeT, DriverT> {
public:
  BestSolutionList(Data *prob, int max_count)
      : _data(prob), _max_list_count(max_count), _list(0),
        _related_functions(0), _nbitems(0) {}
  BestSolutionList()
      : _data(nullptr), _max_list_count(0), _list(0),
        _related_functions(0), _nbitems(0) {}
  ~BestSolutionList() {

    while (!_list.empty()) {
      sol_list_it it = _list.begin();
      Sol *s = *it;
      _list.erase(it);
      delete s;
    }
    _list.clear();
  }

  Sol *GetSolution(int i) {
    sol_list_it it = _list.begin();
    int j = 1;
    while (j++ < i)
      it++;
    return *it;
  }

  void GetSolutions(std::vector<Sol *> &v) {
    sol_list_it it = _list.begin();
    for (; it != _list.end(); it++)
      v.push_back(*it);
  }

  int GetSolutionCount() { return (int)_list.size(); }
  Data *GetDatalemDefinition() { return _data; }

  void Add(Relatedness<NodeT, DriverT> *func) {
    _related_functions.push_back(func);
  }

  void Add(Sol &s) {
    Cost d1 = s.GetLastCost();
    if (_list.size() == (size_t)_max_list_count) {
      sol_list_it it = _list.end();
      it--;
      Cost d2 = (*it)->GetLastCost();
      if (d2 <= d1)
        return;
      else {
        Sol *sol = *it;
        _list.erase(it);

        for (size_t i = 0; i < _related_functions.size(); i++)
          _related_functions[i]->Decrease(*sol);
        delete sol;
      }
    }

    for (size_t i = 0; i < _related_functions.size(); i++)
      _related_functions[i]->Increase(s);

    sol_list_it it = _list.begin();
    while (it != _list.end()) {
      Cost d2 = (*it)->GetLastCost();

      // avoid solutions with exactly the same cost
      if(s.toString() == (*it)->toString())
          return;
//      if (std::abs(d1.totalCost - d2.totalCost) <= 0.03)
//        return;
      else if (d1 < d2) {
        Sol *newsol = new Sol(s);
        _list.insert(it, newsol);
        break;
      }
      it++;
    }
    if (it == _list.end()) {
      // std::unique_ptr<Sol> newsol(new Sol(s));
      Sol *newsol = new Sol(s);
      _list.push_back(newsol);
    }
    _nbitems = _list.size();
  }

  void Add(BestSolutionList<NodeT, DriverT> &list) {
    _max_list_count += list._max_list_count;
    sol_list_it it = list._list.begin();
    while (it != list._list.end()) {
      _list.push_back((*it));
      it++;
    }
    list._list.clear();
    _nbitems = _list.size();
  }
  void Copie(BestSolutionList<NodeT, DriverT> &list) {
    _list.clear();
    _max_list_count = list._max_list_count;
    sol_list_it it = list._list.begin();
    while (it != list._list.end()) {
      Sol *s = new Sol(*(*it));
      _list.push_back(s);
      it++;
    }
    // list._list.clear();
  }

  void Show() {
    printf("List of best solutions count:%d\n", (int)_list.size());
    sol_list_it it = _list.begin();
    int i = 0;
    while (it != _list.end()) {
      double d = (*it)->GetLastCost().satisfiedCost;
      printf("i:%d cost:%.3lf\n", i++, d);
      it++;
    }
  }
  void Clear() {
    while (!_list.empty())
    {
      sol_list_it it = _list.begin();
      Sol *s = *it;
      _list.erase(it);
      delete s;
    }
    _list.clear();
    _nbitems = _list.size();
  }
  void Resize(int size) {
    if (size < _list.size()) {
      printf("Cannot resize this list to a smaller size. Current size:%d new "
             "size:%d\n",
             (int)_list.size(), size);
      exit(1);
    }
    _max_list_count = size;
  }
  int GetSize() { return _max_list_count; }
  
private:
  Data *_data;
  int _max_list_count;
  std::list<Sol *> _list; // sorted list of solution
  std::vector<Relatedness<NodeT, DriverT> *> _related_functions;
  typedef typename std::list<Sol *>::iterator sol_list_it;
  int _nbitems;
};

#endif
