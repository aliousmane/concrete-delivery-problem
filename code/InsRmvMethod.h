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
#ifndef VRP_INSRMV_METHOD
#define VRP_INSRMV_METHOD

#include "ListMove.h"
#include "Move.h"
#include "Solution.h"
#include <iostream>
#include <vector>

template <class NodeT, class DriverT, class MoveT> class InsRmvMethod {
public:
  InsRmvMethod() : _noise(0) {}
  virtual ~InsRmvMethod() = default;

  virtual void InsertCost(Sol &s, NodeT *n, DriverT *d,
                          Move<NodeT, DriverT, MoveT> &m) = 0;

  // Calculate the gain of removing the customer from the solution
  virtual void RemoveCost(Sol &s, NodeT *n, Move<NodeT, DriverT, MoveT> &m) = 0;

  virtual void CancelMove(Sol &s, Move<NodeT, DriverT, MoveT> &m) {}

  // Fill a list with all the customers we have to insert in the solution
  virtual void FillInsertionList(Sol &s, std::vector<NodeT *> &list) {}


  virtual void GetBestInsertion(Sol &s, const std::vector<int> &listId,
                                const std::vector<DriverT *> &driversList,
                                Move<NodeT, DriverT, MoveT> &best) = 0;

  void SetNoise(double noise) { _noise = noise; }
  double GetNoise() { return _noise; }

  virtual void repairSolution(Sol &s) = 0;

  virtual void SetServiceParams(Sol &s, NodeT *n, DriverT *d,
                                int demand) = 0;

  virtual Move<NodeT, DriverT, MoveT>
  GetCost(
          Sol &s,  NodeT *n, DriverT *d, Cost &solcost, int demand,
          ListMove<NodeT, DriverT, MoveT> *temp_moves)=0;

  virtual void ApplyInsertMove(Sol &s, Move<NodeT, DriverT, MoveT> &m)=0 ;

private:
  double _noise;
};

#endif