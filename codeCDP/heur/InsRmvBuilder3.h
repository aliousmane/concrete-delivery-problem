#ifndef INSRMV_BUILDER3_H
#define INSRMV_BUILDER3_H

#include "InsRmvBuilder.h"
#include "ListMoveVrp.h"
#include "../Data.h"
#include <vector>
#include <iostream>

class InsRmvBuilder3 : public InsRmvBuilder {
public:

    explicit InsRmvBuilder3(Data &prob) {
        insrmv = InsRmvMethodFast(prob);
    }

    void GetBestInsertion(Sol &s, const std::vector<int> &listId, const std::vector<Driver *> &driversList,
                          Move<Delivery, Driver, MoveVrp> &best) override {
        const int NB_ITER = 1;
        int NBR_ITER_WITHOUT_SOL = 0;
        while (NBR_ITER_WITHOUT_SOL < NB_ITER) {
            ListMoveVrp l_moves;
            for (int delID: listId) {
                Delivery *del = s.GetDelivery(delID);
                assert(del != nullptr);
                for (Driver *d: driversList) {
                    if (std::find(Sol::TabuFleet[del->id].begin(), Sol::TabuFleet[del->id].end(), d->id) !=
                        Sol::TabuFleet[del->id].end())
                        continue;
                    insrmv.cancel = false;
                    Move<Delivery, Driver, MoveVrp> m;
                    InsertCost(s, del, d, m);
                    if (m.IsFeasible) {
                        l_moves.Add(m);
                    }
                }
            }
            if (l_moves.Count() > 0) {
//                l_moves.Show();
//                std::cout<<"********\n";
                best = l_moves.GetRandom();
//                l_moves.Show();
//                std::cout<<"best \n";
//                best.Show();
//                exit(1);
                NBR_ITER_WITHOUT_SOL = NB_ITER;
            } else {
                NBR_ITER_WITHOUT_SOL++;
                continue;
            }
        }
    }

    void GetBestInsertion(Sol &s, const std::vector<int> &listId,
                          const std::vector<Driver *> &driversList,
                          ListMove<Delivery, Driver, MoveVrp> *list_moves) override {

        for (int delID: listId) {
            Delivery *del = s.GetDelivery(delID);
            assert(del != nullptr);
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
            if (Parameters::SHOW) {
//                list_moves->Show();
            }
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

    void SetServiceParams(Sol &s, Delivery *n, Driver *d, int demand) override {
        Depot *dep = s.GetDepot(n->depotID);
        insrmv.UNLOADING_DURATION = Data::UnloadingTime(n, demand, d);
        insrmv.LOAD_DURATION = Data::LoadingTime(dep, demand);
        insrmv.ADJUSTMENT_DURATION = Parameters::ADJUSTMENT_DURATION;

        Order *o = s.GetOrder(n->orderID);
        Node *prec_del_of_cust = s.CustomerPrev[n->EndNodeID];

        insrmv.expected_del_time = s.EarlyTW(n);
        insrmv.real_del_time = s.EndServiceTime[prec_del_of_cust->id];

        insrmv.max_arrival_Time =
                s.LateTW(n) -
                (o->nbDelMax - n->rank) * s.GetData()->minDriverCap;
        if (insrmv.max_arrival_Time < insrmv.real_del_time) {
            insrmv.max_arrival_Time =
                    s.LateTW(n) -
                    std::max(o->nbDelMin - n->rank, 0) * s.GetData()->maxDriverCap;
        }

        insrmv.max_arrival_Time = std::max(insrmv.max_arrival_Time, insrmv.real_del_time);

        std::vector<std::pair<int, int>> _arrival;
        if (n->rank == 0) {
            int temps = Sol::StartBefore[n->id] == 0 ? s.EarlyTW(n) + Sol::minDelay[n->id] :
                        std::min(Sol::StartBefore[n->id], s.EarlyTW(n) + Sol::minDelay[n->id]);
//            temps = std::max(temps,insrmv.max_arrival_Time);
            _arrival.emplace_back(temps, temps);
        } else {
            int val = mat_func_get_rand_int(-Parameters::TIME_BTW_DELIVERY, Parameters::TIME_BTW_DELIVERY + 1);
            insrmv.max_arrival_Time = std::min(insrmv.max_arrival_Time, s.EndServiceTime[prec_del_of_cust->id] +
                                                                        Parameters::TIME_BTW_DELIVERY);
            int temps = Sol::StartBefore[n->id] == 0 ? s.EndServiceTime[prec_del_of_cust->id] + Sol::minDelay[n->id] :
                        std::min(Sol::StartBefore[n->id],
                                 s.EndServiceTime[prec_del_of_cust->id] + Sol::minDelay[n->id]);

//            temps = std::max(temps,insrmv.max_arrival_Time);
            _arrival.emplace_back(temps, temps);

        }
        if (_arrival[0].first > _arrival[0].second)//TODO || insrmv.max_arrival_Time < _arrival[0].second )
        {
            insrmv.cancel = true;
            return;
        }
        int remaining_demand = s.orderCapRestante[n->orderID]- demand;
        if(remaining_demand >0){
            int end_service = _arrival[0].second + insrmv.UNLOADING_DURATION;
            if(end_service + std::max(remaining_demand,s.GetData()->minDriverCap) >s.LateTW(n)){
                insrmv.cancel = true;
                return;
            }

        }


        int id1 = mat_func_get_rand_int(0, (int) _arrival.size());
        insrmv.expected_del_time = (int) mat_func_get_rand_double_between(
                _arrival[id1].first, _arrival[id1].second);
    }
};

#endif