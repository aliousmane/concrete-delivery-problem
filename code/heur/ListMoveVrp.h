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
  bool operator()(Move<Delivery, Driver, MoveVrp> &m1,
                  Move<Delivery, Driver, MoveVrp> &m2) ;
};

class ListMoveVrp : public ListMove<Delivery, Driver, MoveVrp> {
public:
  ListMoveVrp();
  void Clear();
  void Add(Move<Delivery, Driver, MoveVrp> &m);
  int Count();
  void Insert(int i, Move<Delivery, Driver, MoveVrp> &m);
  Move<Delivery, Driver, MoveVrp> &Get(int i);
  Move<Delivery, Driver, MoveVrp>Extract();

  void Sort();
  Move<Delivery, Driver, MoveVrp> &GetRandom();
  void Resize(int n);
  void partial_Sort(int k);
  void Show();
  std::vector<Move<Delivery, Driver, MoveVrp>> _moves;

private:
  ListMoveVrpSorter move_sorter;
  static bool Sort1(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
  static bool Sort2(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
  static bool Sort3(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
  static bool Sort4(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
};

#endif
