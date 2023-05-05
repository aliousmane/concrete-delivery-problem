/* Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 */

#ifndef LIST_MOVE_VRP_H
#define LIST_MOVE_VRP_H

#include "../Driver.h"
#include "../ListMove.h"
#include "../Move.h"
#include "../Node.h"
#include "MoveVrp.h"
#include <vector>

struct ListMoveVrpSorter {
  bool operator()(Move<Delivery, Driver, MoveVrp> &n1,
                  Move<Delivery, Driver, MoveVrp> &n2) {
    if (n1.DeltaCost.lateDeliveryCost < n2.DeltaCost.lateDeliveryCost) {
      return true;
    } else if (n1.DeltaCost.lateDeliveryCost == n2.DeltaCost.lateDeliveryCost) {
      if (n1.waste == 0)
        return true;
      else if (n2.waste == 0)
        return false;
      if (n1.arrival_del < n2.arrival_del) {
        return true;
      } else {
        if (n1.waste == n2.waste) {
          return (n1.to->capacity > n2.to->capacity);
        } else
          return (std::abs(n1.waste) < std::abs(n2.waste));
      }
    }
    return false;
  }
};

class ListMoveVrp : public ListMove<Delivery, Driver, MoveVrp> {
public:
  ListMoveVrp();
  void Clear();
  void Add(Move<Delivery, Driver, MoveVrp> &m);
  int Count();
  void Insert(int i, Move<Delivery, Driver, MoveVrp> &m);
  Move<Delivery, Driver, MoveVrp> &Get(int i);
  void Sort();
  Move<Delivery, Driver, MoveVrp> &GetRandom();
  void Resize(int n);
  void partial_Sort(int k);
  void Show();
  std::vector<Move<Delivery, Driver, MoveVrp>> _moves;

private:
  ListMoveVrpSorter move_sorter;
};

#endif
