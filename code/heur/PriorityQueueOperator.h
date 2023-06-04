

#ifndef _PRIORITY_QUEUE_OPERATOR_
#define _PRIORITY_QUEUE_OPERATOR_

#include "../InsertOperator.h"
#include "PriorityQueueInsertion.h"

/**
 * k=0, sort by increasing early TW
 * k=1, sort by decreasing demand
 * k=2, sort by increasing late TW
 * k=3, sort by decreasing early TW
 * k=4, sort by decreasing late TW
 * k=5, sort by decreasing demand
 * k=5, shuffle customer
 */
class PriorityQueueOperator : public InsertOperator<Customer, Driver> {
public:
  PriorityQueueOperator(PriorityQueueInsertion *insert_opt, int k,std::string const & name)
      : _insert_opt(insert_opt), _k(k),InsertOperator<Customer, Driver>(name) {
    _insert_opt->SetK(_k);
    _insert_opt->name=name;
  }

  void Insert(Sol &s) override {
    _insert_opt->SetK(_k);
    _insert_opt->Insert(s);
  }

private:
  int _k;
  PriorityQueueInsertion *_insert_opt;
};

#endif
