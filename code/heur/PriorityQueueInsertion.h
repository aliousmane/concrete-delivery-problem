#ifndef SEQUENTIAL_PriorityQueueInsertion_VRP_H
#define SEQUENTIAL_PriorityQueueInsertion_VRP_H

#include "../Move.h"
#include "../Relatedness.h"
#include "../Solution.h"
#include "../SortRelated.h"
#include "../lib/Chrono.h"
#include "../lib/mathfunc.h"
#include "ListMoveVrp.h"
#include "MoveVrp.h"
#include "../TempsPriorite.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <vector>
#include "InsRmvBuilder.h"
#include "../InsertOperator.h"

class PriorityQueueInsertion : public InsertOperator<Node, Driver> {
public:
  PriorityQueueInsertion(Data &prob,
                         InsRmvBuilder &insrmv)
      : _data(prob),
        _insrmv(insrmv), k(0),
        customersList(0), removedList(0) {
    Init();
  }

  void Insert(Sol& s) override;
  void FillQueue(Sol &s,std::vector<Customer *> &list);
  void SetK(int _k) { k = _k; }
  std::string name;

private:
  void InsertByRules(Sol& s);
  void Insert(Sol &s,std::vector<Customer *> &list);
  void DecreaseQueue(Sol &s,Delivery *del, Delivery *next_del, Customer *c);
  void Init() {
    priority_file = TempsPriorite(&_data, _data.GetNodeCount());
    for (int i = 0; i < _data.GetDepotCount(); i++) {
      depotList.push_back(_data.GetDepot(i));
    }
    for (int i = 0; i < _data.GetDriverCount(); i++)
      driversList.push_back(_data.GetDriver(i));

    for (int i = 0; i < _data.GetCustomerCount(); i++) {
      customersList.emplace_back(_data.GetCustomer(i));
    }
  }
  TempsPriorite priority_file;
  int k;
  InsRmvBuilder&_insrmv;
  Data& _data;

  std::vector<Depot *> depotList;
  std::vector<Customer *> customersList;
  std::vector<Customer *> removedList;
  std::vector<Driver *> driversList;
};

#endif
