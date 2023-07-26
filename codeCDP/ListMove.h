/* Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 */

#ifndef LIST_MOVE_H
#define LIST_MOVE_H

#include "Move.h"
#include <vector>

template <class NodeT, class DriverT, class MoveT>
class ListMove
{
public:
  ListMove() = default;
  virtual void Add(Move<NodeT, DriverT, MoveT> &m) = 0;
  virtual int Count() = 0;
  virtual void Insert(int i, Move<NodeT, DriverT, MoveT> &m) = 0;
  virtual Move<NodeT, DriverT, MoveT> &Get(int i) = 0;
  virtual Move<NodeT, DriverT, MoveT> Extract() = 0;
  void Insert( ListMove<NodeT, DriverT, MoveT> &to_insert);

  virtual void Sort() = 0;
  virtual Move<NodeT, DriverT, MoveT> &GetRandom() = 0;
  virtual void Resize(int n) = 0;
  virtual void partial_Sort(int k) = 0;
  virtual void Show() = 0;

private:
};

#endif
