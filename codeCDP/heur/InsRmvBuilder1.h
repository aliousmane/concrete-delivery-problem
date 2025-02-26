#ifndef INSRMV_BUILDER1_H
#define INSRMV_BUILDER1_H

#include "InsRmvBuilder.h"
#include "ListMoveVrp.h"
#include "../Data.h"
#include <vector>
#include <iostream>

class InsRmvBuilder1 : public InsRmvBuilder {
public:

    explicit InsRmvBuilder1(Data &prob) {
        insrmv = InsRmvMethodFast(prob);
    }

    void GetBestInsertion(Sol &s, const std::vector<int> &listId, const std::vector<Driver *> &driversList,
                          Move<Delivery, Driver, MoveVrp> &best) override {
        ListMoveVrp l_moves;
        for (int delID: listId) {
            Delivery *del = s.GetDelivery(delID);
            assert(del != nullptr);
            for (Driver *d: driversList) {
                insrmv.cancel = false;
                Move<Delivery, Driver, MoveVrp> m;
                InsertCost(s, del, d, m);
                if (m.IsFeasible) {
                    l_moves.Add(m);
                }
            }
        }
        if (l_moves.Count() > 0) {
            best = l_moves.GetRandom();
        }
    }

    void InsertCost(Sol &s, Delivery *n, Driver *d,
                    Move<Delivery, Driver, MoveVrp> &mo) override {
        mo.IsFeasible = false;
        mo.DeltaCost = Cost(false);
        mo.n = n;
        mo.to = d;
        mo.demand = std::min(d->capacity, s.orderCapRestante[n->orderID]);
        Dock *dock = s.GetDock(n->dockID);
        Cost newcost(false);
        Node *prev = s.CustomerPrev[n->EndNodeID];
        if (d == s.GetDriverAssignedTo(prev)) {
            if (s.Travel(n, dock) + s.Travel(dock, n) >
                Parameters::TIME_BTW_DELIVERY) {
                return;
            }
        }

        ListMoveVrp list_moves;

        SetServiceParams(s, n, d, mo.demand);
        if (insrmv.cancel) {
            return;
        }

        mo = insrmv.GetCost(s, n, d, newcost, mo.demand, &list_moves);
    }

    void GetBestInsertion(Sol &s, const std::vector<int> &listId,
                          const std::vector<Driver *> &driversList,
                          ListMove<Delivery, Driver, MoveVrp> *list_moves) override {
        for (int delID: listId) {
            Delivery *del = s.GetDelivery(delID);
            for (Driver *d: driversList) {
                if (std::find(Sol::TabuFleet[del->id].begin(), Sol::TabuFleet[del->id].end(), d->id) !=
                    Sol::TabuFleet[del->id].end())
                    continue;
                insrmv.cancel = false;
                Move<Delivery, Driver, MoveVrp> m;
                InsertCost(s, del, d, m);
                if (m.IsFeasible) {
                    list_moves->Add(m);
                }
            }
        }
        if (list_moves->Count() > 0) {
            list_moves->Sort();
        }
    }


    void SetServiceParams(Sol &s, Delivery *n, Driver *d, int demand)

    override {
        Depot *dep = s.GetDepot(n->depotID);
        insrmv.
                UNLOADING_DURATION = Data::UnloadingTime(n, demand, d);
        insrmv.
                LOAD_DURATION = Data::LoadingTime(dep, demand);
        insrmv.
                ADJUSTMENT_DURATION = Parameters::ADJUSTMENT_DURATION;

        Order *o = s.GetOrder(n->orderID);
        Node *prec_del_of_cust = s.CustomerPrev[n->EndNodeID];

        insrmv.
                expected_del_time = s.EarlyTW(n);
        insrmv.
                real_del_time = s.EndServiceTime[prec_del_of_cust->id];

        insrmv.
                max_arrival_Time =
                s.LateTW(n) -
                (o->nbDelMax - n->rank) * s.GetData()->minDriverCap;
        if (insrmv.max_arrival_Time < insrmv.real_del_time) {
            insrmv.
                    max_arrival_Time =
                    s.LateTW(n) -
                    std::max(o->nbDelMin - n->rank, 0) * s.GetData()->maxDriverCap;
        }

        insrmv.
                max_arrival_Time = std::max(insrmv.max_arrival_Time, insrmv.real_del_time);

        std::vector<std::pair<double, double>> _arrival;
        if (n->rank == 0) {
            _arrival.
                    emplace_back(s
                                         .
                                                 EarlyTW(n)
                                 - Parameters::TIME_BTW_DELIVERY, insrmv.max_arrival_Time - Sol::minDelay[n->id]);
        } else {
            int val = mat_func_get_rand_int(-Parameters::TIME_BTW_DELIVERY, Parameters::TIME_BTW_DELIVERY + 1);
            insrmv.
                    max_arrival_Time = std::min(insrmv.max_arrival_Time, s.EndServiceTime[prec_del_of_cust->id] +
                                                                         Parameters::TIME_BTW_DELIVERY);

            _arrival.
                    emplace_back(
                    std::min(insrmv.max_arrival_Time, std::max(s.EndServiceTime[prec_del_of_cust->id] + val,
                                                               s.EndServiceTime[prec_del_of_cust->id] +
                                                               Sol::minDelay[n->id])),
                    insrmv
                            .max_arrival_Time);

        }
        if (_arrival[0].first > _arrival[0].second) {
            insrmv.
                    cancel = true;
            return;
        }
        int id1 = mat_func_get_rand_int(0, (int) _arrival.size());
        insrmv.
                expected_del_time = (int) mat_func_get_rand_double_between(
                _arrival[id1].first, _arrival[id1].second);
    }
};

#endif