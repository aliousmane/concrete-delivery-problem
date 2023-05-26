/* Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
 */

#ifndef _MOVE_H_
#define _MOVE_H_

#include "Solution.h"
#include "lib/mathfunc.h"
//#include <cstdlib>
#include <sstream>

template<class NodeT, class DriverT, class MoveT>
class Move {
public:
    NodeT *n;
    DriverT *to;
    int demand;
    Cost DeltaCost;
    double DeltaDistance;
    int waste;
    int arrival_dock, arrival_del;
    bool IsFeasible;
    int FailureCause;
    int clientDriver{0};
    int allDriver{0};
    MoveT move;

    Move()
            : n(nullptr), to(nullptr), IsFeasible(false), demand(0),
              DeltaDistance(INFINITY),
              DeltaCost(false), arrival_dock(0), arrival_del(0),
              waste(0), FailureCause(Parameters::FAILURECAUSE::NONE) {}

    Move(NodeT *n, DriverT *to) : n(n), to(to), IsFeasible(false), demand(0),
                                  DeltaDistance(INFINITY),
                                  DeltaCost(false), arrival_dock(0), arrival_del(0),
                                  waste(0), FailureCause(Parameters::FAILURECAUSE::NONE) {}

    bool operator<(const Move<NodeT, DriverT, MoveT> &m) const {
        if (this->n == m.n) {
            return (this->arrival_del <= m.arrival_del);
        }
        return (DeltaCost < m.DeltaCost);
    }

    bool operator<=(const Move<NodeT, DriverT, MoveT> &m) const {
        return (DeltaCost <= m.DeltaCost);
    }

    bool operator<(const Move<NodeT, DriverT, MoveT> *m) const {
        return (DeltaCost < m->DeltaCost);
    }

    bool operator<=(const Move<NodeT, DriverT, MoveT> *m) const {
        return (DeltaCost <= m->DeltaCost);
    }
    std::string toString(){
        std::stringstream ss;
        ss <<  n->id << "(" << n->orderID << ")--" << "("
             << n->rank << ")|"
             << waste << " --" << arrival_del << "--(" << clientDriver << "-"<<allDriver<< ")-- F:"
             << FailureCause <<"->" << DeltaCost.waitingCost << std::endl;
        return ss.str();
    }
    void Show() {
        printf("driver %ld(%d) insert (%d-%d) after %d at %d with waste %d \n", to->no, to->id, move.dock->id,
               n->id, move.prev->id, arrival_del, waste);
        // printf("Plant %d insert %d after %d \n", depot->depotLoc, dock->id,
        // move.prevOfDepot->id);
    }
};
template<class NodeT, class DriverT, class MoveT>
class MoveHashFunction {
public:
    size_t operator()(const Move<NodeT, DriverT, MoveT> &m) const {
        return std::hash<std::string>()(m.toString());
    }

    size_t operator()(const std::string &s) const {
        return std::hash<std::string>()(s) << 1;
    };
};
#endif
