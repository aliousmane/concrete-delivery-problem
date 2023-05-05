//
// Created by Ousmane Ali on 2023-04-28.
//

#include "InsRmvMethodFast.h"

using namespace std;
InsRmvMethodFast::InsRmvMethodFast(Data &data): _data(&data) {

}

void InsRmvMethodFast::GetBestInsertion(Sol &s, const vector<int> &listId,
                                        const vector<Driver *> &driversList,
                                        Move<Delivery, Driver, MoveVrp> &best)
{}

void InsRmvMethodFast::InsertCost(Sol &s, Delivery *n, Driver *d,
                                  Move<Delivery, Driver, MoveVrp> &mo){

}
void InsRmvMethodFast::InsertCost(Sol &s, Delivery *n, Driver *d,
                                  ListMove<Delivery, Driver, MoveVrp> *list_moves)
{}
void InsRmvMethodFast::RemoveCost(Sol &s, Delivery *n,
                                  Move<Delivery, Driver, MoveVrp> &m)
{}
void InsRmvMethodFast::CancelMove(Sol &s, Move<Delivery, Driver, MoveVrp> &m)
{}
void InsRmvMethodFast::FillInsertionList(Sol &s, std::vector<Delivery *> &list)
{}

void InsRmvMethodFast::SetServiceParams(Sol &s,  Delivery *n, Driver *d,
                                        int demand){
    Depot * dep = s.GetDepot(n->depotID);
    UNLOADING_DURATION = Data::UnloadingTime(n,demand,d);
    LOAD_DURATION = Data::LoadingTime(dep,demand);
    ADJUSTMENT_DURATION = Parameters::ADJUSTMENT_DURATION;

    Order *o = s.GetOrder(n->orderID);
    Node *prec_del_of_cust = s.CustomerPrev[n->EndNodeID];

    expected_del_time = s.EarlyTW(n);
    real_del_time = s.EndServiceTime[prec_del_of_cust->id];

    max_arrival_Time =
            s.LateTW(n) -
            (o->nbDelMax - n->rank) * s.GetData()->minDriverCap;
    if(max_arrival_Time < real_del_time){
        max_arrival_Time =
                s.LateTW(n) -
                std::max(o->nbDelMin-n->rank,0) * s.GetData()->maxDriverCap;
    }

    max_arrival_Time = std::max(max_arrival_Time, real_del_time);

    std::vector<std::pair<double, double>> _arrival;
    if(n->rank==0){
        _arrival.emplace_back(s.EarlyTW(n) - Parameters::TIME_BTW_DELIVERY, max_arrival_Time);

    }
    else
    {
        int val = mat_func_get_rand_int(-Parameters::TIME_BTW_DELIVERY, Parameters::TIME_BTW_DELIVERY + 1);
        max_arrival_Time = std::min(max_arrival_Time, s.EndServiceTime[prec_del_of_cust->id] +
                                                                    Parameters::TIME_BTW_DELIVERY);
        _arrival.emplace_back(
                s.EndServiceTime[prec_del_of_cust->id]+val,
                max_arrival_Time);

        if (_arrival[0].first > _arrival[0].second)
        {
            cancel = true;
            return;
        }
    }

    int id1 = mat_func_get_rand_int(0, (int)_arrival.size());
    expected_del_time = (int)mat_func_get_rand_double_between(
            _arrival[id1].first, _arrival[id1].second);

}
void InsRmvMethodFast::repairSolution(Sol &s) {

}

void InsRmvMethodFast::ApplyInsertMove(Sol &s, Move<Delivery, Driver, MoveVrp>
        &m) {
    s.InsertAfter(m.move.dock, m.move.prev,m.to);
    s.InsertAfter(m.n, m.move.dock,m.to);
    s.AssignDeliveryToCustomer(m.n);
    s.ArrivalTime[m.move.dock->id] = m.arrival_dock;
    s.ArrivalTime[m.n->id] = m.arrival_del;
    s.DepotAssignTo[m.move.dock->id] = m.move.depot;
    assert(m.arrival_dock <= m.arrival_del + s.Travel(m.move.dock, m.n));
}

Move<Delivery, Driver, MoveVrp> InsRmvMethodFast::GetCost(
        Sol &s, Delivery *n, Driver *d, Cost &solcost, int demand,
        ListMove<Delivery, Driver, MoveVrp> *temp_moves)
{
    Dock * dock = s.GetDock(n->dockID);
    int ELT = expected_del_time - s.Travel(dock, n) - ADJUSTMENT_DURATION -
                 LOAD_DURATION;
    auto *prec_del_of_cust = dynamic_cast<Delivery*>( s.CustomerPrev[n->EndNodeID]);
    if (prec_del_of_cust!= nullptr)
    {
        Dock *prec_dock = s.GetDock(prec_del_of_cust->dockID);
        ELT = std::max(ELT, s.EndServiceTime[prec_dock->id]);
    }
    TimeSlot loadSlot = TimeSlot(ELT, ELT + LOAD_DURATION);
    bool show = false;
    Node *prev = s.GetNode(d->StartNodeID);
    while (prev->type!=Parameters::END_LINK)
    {
        assert(d == s.DriverAssignTo[prev->id]);
        Cost newcost(false);
        if (prev->type==Parameters::DOCK)
        {
            prev = s.DriverNext[prev->id];
            continue;
        }
        Move<Delivery, Driver, MoveVrp> m;
        Dock *next_dock = dynamic_cast<Dock*>(s.DriverNext[prev->id]);
        Delivery *next_del = nullptr;
        if (next_dock != nullptr) {
            next_del = dynamic_cast<Delivery *> (s.DriverNext[next_dock->id]);
            if ( ((next_del->orderID == n->orderID) and (n->rank > next_del->rank))
            or (s.LateTW(next_del) < s.EarlyTW(n))
            or (ELT > s.LateTW(next_del)))
            {
                    prev = next_del;
                    continue;
            }
        }
        Depot *dep = s.GetDepot(dock->depotID);
        ELT = std::max(ELT, s.DepartureTime[prev->id] + s.Travel(prev, dock));

        loadSlot = TimeSlot(ELT, ELT + LOAD_DURATION);
        TimeSlot unloadSlot = TimeSlot();
        int node_arr = ELT;
        node_arr += LOAD_DURATION + ADJUSTMENT_DURATION;
        node_arr += s.Travel(dock, n);
        unloadSlot.lower = node_arr;

        if (show)
        {
            cout << "ELT " << ELT << " Distance " << s.Travel(dock, n) << endl;
        }

        if (node_arr > s.LateTW(n))
        {
            prev = (next_del == nullptr) ? s.DriverNext[prev->id]  : next_del;
            continue;
        }
        if (prec_del_of_cust!= nullptr)
        {
            const int max_arrival = s.EndServiceTime[prec_del_of_cust->id]  + Parameters::TIME_BTW_DELIVERY;
            if (node_arr > max_arrival)
            {
                if (Parameters::PENALTY_COST)
                {
                    newcost.lateDeliveryCost +=
                            std::max(0, (node_arr - max_arrival)) *
                            Parameters::LATE_ARRIVAL_PENALTY;
                }
                else
                {
                    if (show)
                    {
                        cout << "node_arr > "
                                "s.nodeServiceIntervals[prec_del_of_cust->id].upper +  "
                                "Parameters::TIME_BTW_DELIVERY "
                             << endl;
                    }
                    int delay = node_arr-max_arrival;
//                    cout<<n->id<<" violates delay for "<<delay<<" min. LTW= "<<s.LateTW(n)<< " arrival = "<<node_arr <<endl;
//                    if(s.WaitingTime[prec_del_of_cust->id] <0)
//                    {
                        if(delay - s.WaitingTime[prec_del_of_cust->id] < Parameters::TIME_BTW_DELIVERY){
                            Sol::FailureCause[n->id]=Parameters::FAILURECAUSE::DELAY;
                            Sol::minDelay[n->id] = (Sol::minDelay[n->id]==0)  ? delay: std::min(Sol::minDelay[n->id],delay);
                        }
//                    }

                    prev = (next_del == nullptr) ? s.DriverNext[prev->id]  : next_del;
                    continue;
                }

            }
        }
        Sol::SetTimingCost(n,  node_arr, real_del_time,
                           s.EarlyTW(n), newcost);

        node_arr = std::max(node_arr, real_del_time);

        if (prec_del_of_cust!= nullptr)
        {
            assert(node_arr >= s.EndServiceTime[prec_del_of_cust->id]);
        }
        if (node_arr > max_arrival_Time)
        {
            //TODO
            //newcost.lateDeliveryCost += std::max(0, (node_arr - max_arrival_Time));
            //prev = s.DriverNext[prev->id];
            // continue;
        }
        node_arr += UNLOADING_DURATION;
        if (node_arr > s.LateTW(n))
        {
            if (Parameters::PENALTY_COST)
            {
                newcost.lateDeliveryCost += std::max(0, (s.LateTW(n) - node_arr)) *
                                            Parameters::LATE_ARRIVAL_PENALTY;
            }
            else
            {
                if (show)
                {
                    cout << "node_arr > s.LateTW(n) " << endl;
                }
                prev = (next_del == nullptr) ? s.DriverNext[prev->id]  : next_del;
                continue;
            }
        }
        unloadSlot.upper = node_arr;
        node_arr += Parameters::CLEANING_DURATION;

        TimeSlot intv = TimeSlot(loadSlot.lower, node_arr);
        auto it = s.driverWorkingIntervals[d->id].find(intv);
        if (it != s.driverWorkingIntervals[d->id].end())
        {
            // newcost.lateDeliveryCost +=
            //     std::max(0.,
            //              (std::abs(it->lower-loadSlot.upper))) *
            //     Parameters::LATE_ARRIVAL_PENALTY;
            if (show)
            {
                cout << "it != s.driverWorkingIntervals[d->id].end())" << endl;
            }
//            cout<<" driver "<<d->id <<" was busy with "<< it->nodeID <<" for node "<<n->id<<endl;
            prev = (next_del == nullptr) ? s.DriverNext[prev->id]  : next_del;
            continue;
        }

        if (next_dock!= nullptr)
        {
            if (node_arr + s.Travel(n, next_dock) + s.Travel(next_dock, next_del) >
                s.StartServiceTime[next_del->id])
            {
                if (Parameters::PENALTY_COST)
                {
                    newcost.lateDeliveryCost +=
                            std::max(0, (node_arr + s.Travel(n, next_dock) +
                                          s.Travel(next_dock, next_del) -
                                          s.StartServiceTime[next_del->id])) *
                            Parameters::LATE_ARRIVAL_PENALTY;
                }
                else
                {
                    if (show)
                    {
                        cout << "node_arr + s.Travel(n, next) + s.Travel(next, next_del) > "
                                " s.StartServiceTime[next_del->id] "
                             << endl;
                    }
                    prev = next_del;
                    continue;
                }
            }
            if (node_arr + s.Travel(n, next_dock) + s.Travel(next_dock, next_del) +
                Data::UnloadingTime(next_del,next_del->demand,d) >
                s.LateTW(next_del))
            {
                if (Parameters::PENALTY_COST)
                {
                    newcost.lateDeliveryCost +=
                            std::max(0, (node_arr + s.Travel(n, next_dock) +
                                          s.Travel(next_dock, next_del) +
                                    Data::UnloadingTime(next_del,next_del->demand,d) - s.LateTW(next_del))) *
                            Parameters::LATE_ARRIVAL_PENALTY;
                }
                else
                {
                    if (show)
                    {
                        cout << "node_arr + s.Travel(n, next) + s.Travel(next, next_del) "
                                "+next_del->unload_duration > LateTW(next_del) "
                             << endl;
                    }
                    prev = next_del;
                    continue;
                }
            }
        }
        if (show)
        {
            cout << "Installation possible " << endl;
        }
        newcost.travelCost = s.Travel(prev, dock) + s.Travel(dock, n) +
                             s.Travel(prev->distID, d->distID) - s.Travel(prev->distID, d->distID);

        newcost.waitingCost = newcost.clientWaitingCost + newcost.truckWaitingCost;
        newcost.undeliveredCost = 0;
        newcost.satisfiedCost = demand;
        newcost.isFeasible = true;
        newcost += s.updateCost;
        if (newcost.lateDeliveryCost > 100)
        {
            prev = s.DriverNext[prev->id];
            continue;
        }

        m.n = n;
        m.to = d;
        m.move.dock = dock;
        m.move.del = n;
        m.move.prev = prev;
        m.move.depot = dep;
        m.arrival_dock = ELT;
        m.arrival_del = unloadSlot.lower;
        m.DeltaCost = newcost;
        m.DeltaDistance = newcost.travelCost;
        m.demand = demand;
        m.IsFeasible = true;
        m.waste = s.orderCapRestante[n->orderID] - d->capacity;
        temp_moves->Add(m);
        assert(m.arrival_dock <= m.arrival_del + s.Travel(dock, n));

        prev = s.DriverNext[prev->id];
    }

    Move<Delivery, Driver, MoveVrp> best;
    if (temp_moves->Count() > 0)
    {
        // best = temp_moves->Get(0);
        best = temp_moves->GetRandom();
    }
    solcost = best.DeltaCost;
    return best;
}