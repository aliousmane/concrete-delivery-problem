#ifndef INSRMV_BUILDER_H
#define INSRMV_BUILDER_H

#include "InsRmvMethodFast.h"
#include "MoveVrp.h"
#include "../Solution.h"
#include "../Node.h"
#include "../Driver.h"
#include "../Move.h"
#include <vector>
class InsRmvBuilder
{

public:

    virtual void InsertCost(Sol &s, Delivery *n, Driver *d, Move<Delivery, Driver, MoveVrp> &m)
    {
        insrmv.InsertCost(s, n, d, m);
    }
    virtual void InsertCost(Sol &s, Delivery *n, Driver *d, ListMove<Delivery, Driver, MoveVrp> *temp_moves)
    {
        insrmv.InsertCost(s, n, d, temp_moves);
    }

    void ApplyInsertMove(Sol &s, Move<Delivery, Driver, MoveVrp> &m)
    {
        insrmv.ApplyInsertMove(s, m);
    }

    virtual void GetBestInsertion(Sol &s, const std::vector<int> &listId, const std::vector<Driver *> &driversList,
                                  Move<Delivery, Driver, MoveVrp> &best)
    {
        insrmv.GetBestInsertion(s, listId, driversList, best);
    }
    virtual void GetBestInsertion(Sol &s, const std::vector<int> &listId,
                          const std::vector<Driver *> &driversList,
                          ListMove<Delivery, Driver, MoveVrp> *list_moves) {
        insrmv.GetBestInsertion(s, listId, driversList, list_moves);
    }
    virtual void SetServiceParams(Sol &s,  Delivery *n, Driver *d, int demand)
    {
        insrmv.SetServiceParams(s,  n, d, demand);
    }
    virtual void repairSolution(Sol &s)
    {
        insrmv.repairSolution(s);
    }
    virtual void repairSolution(Sol &s,Order *o)
    {
        insrmv.repairSolution(s, o);
    }
    virtual void repairSolution(Sol &s,Customer *c)
    {
        insrmv.repairSolution(s,c);
    }

    virtual Move<Delivery, Driver, MoveVrp>
    GetCost(Sol &s,  Delivery *n, Driver *d, Cost &solcost,
            double demand, ListMove<Delivery, Driver, MoveVrp> *temp_moves)
    {
        return insrmv.GetCost(s,  n, d, solcost, demand, temp_moves);
    }
    virtual void FillStructures( Sol &s, std::vector<Customer *> & customersList,
                                           std::vector<Driver *> & driversList){
        insrmv.FillStructures(  s, customersList,driversList);
    }

    InsRmvMethodFast insrmv;
};

#endif