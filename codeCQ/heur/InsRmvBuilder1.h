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
                if(d->overTime >= Parameters::MAX_OVERTIME){
                    continue;
                }
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
        mo.demand = (Sol::FixLoad[n->delID] == -1) ?
                    std::min(d->capacity, s.orderCapRestante[n->orderID]) : Sol::FixLoad[n->delID];

        if (mo.demand > d->capacity) return;

        Dock *dock = s.GetDock(n->dockID);
        Cost newcost(false);
        Node *prev = s.CustomerPrev[n->EndNodeID];
        if (d == s.GetDriverAssignedTo(prev)) {
            if (s.Travel(n, dock) + s.Travel(dock, n) >
                s.GetTimeBtwDel(n)) {
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
                if(d->overTime >= Parameters::MAX_OVERTIME){
                    continue;
                }
                insrmv.cancel = false;
                Move<Delivery, Driver, MoveVrp> m;
                InsertCost(s, del, d, list_moves);
            }
        }
        if (list_moves->Count() > 0) {
            list_moves->Sort();
        }
    }

    void InsertCost(Sol &s, Delivery *n, Driver *d,
                    ListMove<Delivery, Driver, MoveVrp> *temp_moves) override {
        Move<Delivery, Driver, MoveVrp> mo;
        mo.IsFeasible = false;
        mo.DeltaCost = Cost(false);
        mo.n = n;
        mo.to = d;
        mo.demand = (Sol::FixLoad[n->delID] == -1) ?
                    std::min(d->capacity, s.orderCapRestante[n->orderID]) : Sol::FixLoad[n->delID];

        if (mo.demand > d->capacity) return;

        Dock *dock = s.GetDock(n->dockID);
        Cost newcost(false);
        Node *prev = s.CustomerPrev[n->EndNodeID];
        if (d == s.GetDriverAssignedTo(prev)) {
            if (s.Travel(n, dock) + s.Travel(dock, n) >
                s.GetTimeBtwDel(n)) {
                return;
            }
        }
        SetServiceParams(s, n, d, mo.demand);
        if (insrmv.cancel) {
            return;
        }
        mo = GetCost(s, n, d, newcost, mo.demand, temp_moves);
    }


    void SetServiceParams(Sol &s, Delivery *n, Driver *d, double demand) override {
        Depot *dep = s.GetDepot(n->depotID);
        insrmv.UNLOADING_DURATION = Data::UnloadingTime(n, demand, d);
        insrmv.LOAD_DURATION = Data::LoadingTime(dep, demand);
        insrmv.ADJUSTMENT_DURATION = Parameters::ADJUSTMENT_DURATION;
        if (Sol::FixStartLoad[n->delID] == -1) {

            Order *o = s.GetOrder(n->orderID);
            Node *prec_del_of_cust = s.CustomerPrev[n->EndNodeID];

            insrmv.expected_del_time = s.EarlyTW(n);
            insrmv.real_del_time = s.EndServiceTime[prec_del_of_cust->id];

            insrmv.max_arrival_Time = insrmv.real_del_time + s.GetTimeBtwDel(n);

            std::vector<std::pair<double, double>> _arrival;
            if (prec_del_of_cust->type != Parameters::DELIVERY) {
                insrmv.max_arrival_Time += 5;
                _arrival.
                        emplace_back(s.EarlyTW(n) - s.GetTimeBtwDel(n) - 1,
                                     insrmv.max_arrival_Time - Sol::minDelay[n->id]);
            } else {
                int val = mat_func_get_rand_int(-s.GetTimeBtwDel(n), s.GetTimeBtwDel(n) + 1);
                insrmv.max_arrival_Time = std::min(insrmv.max_arrival_Time, s.EndServiceTime[prec_del_of_cust->id] +
                                                                            s.GetTimeBtwDel(n));
                _arrival.
                        emplace_back(
                        std::min(insrmv.max_arrival_Time, std::max(s.EndServiceTime[prec_del_of_cust->id] + val,
                                                                   s.EndServiceTime[prec_del_of_cust->id] +
                                                                   Sol::minDelay[n->id])), insrmv.max_arrival_Time);

            }
            if (_arrival[0].first > _arrival[0].second) {
                insrmv.cancel = true;
                return;
            }
            int id1 = mat_func_get_rand_int(0, (int) _arrival.size());
            insrmv.expected_del_time = (int) mat_func_get_rand_double_between(_arrival[id1].first,
                                                                              _arrival[id1].second);
        }
    }
};

#endif