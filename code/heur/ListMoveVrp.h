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
#include <unordered_map>
struct ListMoveVrpSorter {
  bool operator()(Move<Delivery, Driver, MoveVrp> &m1,
                  Move<Delivery, Driver, MoveVrp> &m2) ;
};

class ListMoveVrp : public ListMove<Delivery, Driver, MoveVrp> {
public:
  ListMoveVrp();
  void Clear();
  void Add(Move<Delivery, Driver, MoveVrp> &m) override;
  int Count() override;
  void Insert(int i, Move<Delivery, Driver, MoveVrp> &m) override;
  void Insert( ListMoveVrp &to_insert);
  Move<Delivery, Driver, MoveVrp> &Get(int i) override;
  Move<Delivery, Driver, MoveVrp>Extract() override;

  void Sort() override;
  Move<Delivery, Driver, MoveVrp> &GetRandom() override;
  void Resize(int n) override;
  void partial_Sort(int k) override;
  void Show() override;
  std::vector<Move<Delivery, Driver, MoveVrp>> _moves;

private:
    std::unordered_map<std::string, Move<Delivery, Driver, MoveVrp>  , MoveHashFunction<Delivery, Driver, MoveVrp> > moveMap;
  ListMoveVrpSorter move_sorter;
  static bool Sort1(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
  static bool Sort2(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
  static bool Sort3(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
  static bool Sort4(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
  static bool Sort5(Move<Delivery, Driver, MoveVrp> &m1,Move<Delivery, Driver, MoveVrp> &m2);
};

#endif
