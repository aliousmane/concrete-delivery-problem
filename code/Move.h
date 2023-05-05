/* Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 */

#ifndef _MOVE_H_
#define _MOVE_H_
#include "Solution.h"
#include "lib/mathfunc.h"
#include <cstdlib>

template <class NodeT, class DriverT, class MoveT> class Move {
public:
  NodeT *n;
  DriverT *to;
  double demand;
  Cost DeltaCost;
  double DeltaDistance;
  double waste;
  double arrival_dock, arrival_del;
  bool IsFeasible;
  MoveT move;

  Move()
      : n(NULL), to(NULL), IsFeasible(false), demand(0),
         DeltaDistance(INFINITY),
        DeltaCost(false), arrival_dock(0), arrival_del(0),
        waste(0) {}

  bool operator<(const Move<NodeT, DriverT, MoveT> &m) const {
      if(this->n == m.n){
          return (this->arrival_dock <= m.arrival_dock);
      }
    return (DeltaCost < m.DeltaCost);
  }

  bool operator<=(const Move<NodeT, DriverT, MoveT> &m)const {
    return (DeltaCost <= m.DeltaCost);
  }

  bool operator<(const Move<NodeT, DriverT, MoveT> *m)const {
    return (DeltaCost < m->DeltaCost);
  }
  bool operator<=(const Move<NodeT, DriverT, MoveT> *m) const{
    return (DeltaCost <= m->DeltaCost);
  }
  void Show() {
    printf("driver %d(%d) insert (%d-%d) after %d \n", to->no, to->id, move.dock->id,
           n->id, move.prev->id);
    // printf("Plant %d insert %d after %d \n", depot->depotLoc, dock->id,
    // move.prevOfDepot->id);
  }
};

#endif
