
#include "InsRmvMethodFast.h"
#include "../Prompt.h"

using namespace std;

InsRmvMethodFast::InsRmvMethodFast(Data &data) : _data(&data) {}

void
InsRmvMethodFast::GetBestInsertion(Sol &s, const std::vector<int> &listId, const std::vector<Driver *> &driversList,
                                   ListMove<Delivery, Driver, MoveVrp> *list_moves) {
    for (int delID: listId) {
        Delivery *del = s.GetDelivery(delID);
        assert(del != nullptr);
        for (Driver *d: driversList) {
            cancel = false;
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

void InsRmvMethodFast::SetServiceParams(Sol &s, Delivery *n, Driver *d, int demand) {
    Depot *dep = s.GetDepot(n->depotID);
    UNLOADING_DURATION = Data::UnloadingTime(n, demand, d);
    LOAD_DURATION = Data::LoadingTime(dep, demand);
    ADJUSTMENT_DURATION = Parameters::ADJUSTMENT_DURATION;

    Order *o = s.GetOrder(n->orderID);
    Node *prec_del_of_cust = s.CustomerPrev[n->EndNodeID];

    expected_del_time = s.EarlyTW(n);
    real_del_time = s.EndServiceTime[prec_del_of_cust->id];

    max_arrival_Time =
            s.LateTW(n) -
            (o->nbDelMax - n->rank) * s.GetData()->minDriverCap;
    if (max_arrival_Time < real_del_time) {
        max_arrival_Time =
                s.LateTW(n) -
                std::max(o->nbDelMin - n->rank, 0) * s.GetData()->maxDriverCap;
    }

    max_arrival_Time = std::max(max_arrival_Time, real_del_time);

    std::vector<std::pair<double, double>> _arrival;
    if (n->rank == 0) {
        _arrival.emplace_back(s.EarlyTW(n) - Parameters::TIME_BTW_DELIVERY, max_arrival_Time);

    } else {
        int val = mat_func_get_rand_int(-Parameters::TIME_BTW_DELIVERY, Parameters::TIME_BTW_DELIVERY + 1);
        max_arrival_Time = std::min(max_arrival_Time, s.EndServiceTime[prec_del_of_cust->id] +
                                                      Parameters::TIME_BTW_DELIVERY);
        _arrival.emplace_back(
                s.EndServiceTime[prec_del_of_cust->id] + val,
                max_arrival_Time);

        if (_arrival[0].first > _arrival[0].second) {
            cancel = true;
            return;
        }
    }

    int id1 = mat_func_get_rand_int(0, (int) _arrival.size());
    expected_del_time = (int) mat_func_get_rand_double_between(
            _arrival[id1].first, _arrival[id1].second);

}

void InsRmvMethodFast::ApplyInsertMove(Sol &s, Move<Delivery, Driver, MoveVrp> &m) {
    s.InsertAfter(m.move.dock, m.move.prev, m.to);
    s.InsertAfter(m.n, m.move.dock, m.to);
    s.AssignDeliveryToCustomer(m.n);
    s.ArrivalTime[m.move.dock->id] = m.arrival_dock;
    s.ArrivalTime[m.n->id] = m.arrival_del;
    s.DepotAssignTo[m.move.dock->id] = m.move.depot;
    assert(m.arrival_dock <= m.arrival_del + s.Travel(m.move.dock, m.n));
}

Move<Delivery, Driver, MoveVrp> InsRmvMethodFast::GetCost(Sol &s, Delivery *n, Driver *d, Cost &solcost, int demand,
                                                          ListMove<Delivery, Driver, MoveVrp> *temp_moves) {
    Customer *c = s.GetCustomer(n->custID);
    Dock *dock = s.GetDock(n->dockID);
    Depot *dep = s.GetDepot(dock->depotID);
    int ELT = expected_del_time - s.Travel(dock, n) - ADJUSTMENT_DURATION - LOAD_DURATION;
    auto *prec_del_of_cust = dynamic_cast<Delivery *>( s.CustomerPrev[n->EndNodeID]);
    if (Parameters::SHOW) {
//        Prompt::print({"Try insertion with", to_string(d->id)});
//        s.ShowSlot(d);
//        s.Show(d);
    }
    if (prec_del_of_cust != nullptr) {
        Dock *prec_dock = s.GetDock(prec_del_of_cust->dockID);
        ELT = std::max(ELT, s.EndServiceTime[prec_dock->id]);
    }
    TimeSlot loadSlot = TimeSlot(ELT, ELT + LOAD_DURATION);
    Node *prev = s.GetNode(d->StartNodeID);
    while (prev->type != Parameters::END_LINK) {
        assert(d == s.DriverAssignTo[prev->id]);
        Cost newcost(false);
        if (prev->type == Parameters::DOCK) {
            prev = s.DriverNext[prev->id];
            continue;
        }
        Delivery *prev_del = nullptr;
        if (prev->type == Parameters::DELIVERY) {
            prev_del = dynamic_cast<Delivery *> (prev);
            if (s.LateTW(n) <= s.EarlyTW(prev_del)) {
                break;
            }
        }
        if (Parameters::SHOW) {
//            Prompt::print({"Try Insert after", to_string(prev->id)});
        }

        Dock *next_dock = dynamic_cast<Dock *>(s.DriverNext[prev->id]);
        Delivery *next_del = nullptr;
        if (next_dock != nullptr) {
            next_del = dynamic_cast<Delivery *> (s.DriverNext[next_dock->id]);
            if (((next_del->orderID == n->orderID) and (n->rank > next_del->rank))
                or (s.LateTW(next_del) < s.EarlyTW(n))
                or (ELT > s.LateTW(next_del))) {
                prev = next_del;
                continue;
            }

        }
        Move <Delivery, Driver, MoveVrp> m(n, d);
        ELT = std::max(ELT, s.DepartureTime[prev->id] + s.Travel(prev, dock));
        loadSlot = TimeSlot(ELT, ELT + LOAD_DURATION);
        TimeSlot unloadSlot = TimeSlot();

        int node_arr = ELT;
        node_arr += LOAD_DURATION + ADJUSTMENT_DURATION;
        node_arr += s.Travel(dock, n);
        unloadSlot.lower = node_arr;

        if (Parameters::SHOW) {
//            Prompt::print({"ELT ", to_string(ELT), ":prev->L", to_string(s.Travel(prev, dock)), "L->D",
//                           to_string(s.Travel(dock, n))});
        }
        if (node_arr > s.LateTW(n)) {
            int delay = node_arr - s.LateTW(n);
            if (Parameters::SHOW) {
//                Prompt::print({"After ", to_string(prev->id), ",", to_string(n->id), "arrives",
//                               to_string(delay), "after  LTW  with driver", to_string(d->id)});
            }
// TODO           Sol::pullVisit[n->id] = (Sol::pullVisit[n->id]==0)  ? delay: std::min(Sol::pullVisit[n->id],delay);
            if (prev->type == Parameters::DELIVERY) {
//                m.FailureCause = Parameters::FAILURECAUSE::LATETW;
                m.move.prev = prev;
                m.arrival_del = node_arr;
                m.move.prevMaxTime = s.EndServiceTime[prev->id] - delay;
                m.IsFeasible = true;
//                temp_moves->Add(m);
            }
//            Sol::FailureCause[n->id] = Parameters::FAILURECAUSE::LATETW;
            prev = (next_del == nullptr) ? s.DriverNext[prev->id] : next_del;
            continue;
        }
        if (prec_del_of_cust != nullptr) {
            const int max_arrival = s.EndServiceTime[prec_del_of_cust->id] + Parameters::TIME_BTW_DELIVERY;
            if (node_arr > max_arrival) {
                int delay = node_arr - max_arrival;
                if (delay > n->rank * Parameters::TIME_BTW_DELIVERY) {
                    prev = (next_del == nullptr) ? s.DriverNext[prev->id] : next_del;
                    continue;
                }
//                m.FailureCause = Parameters::FAILURECAUSE::DELAY;
                m.move.prev = prec_del_of_cust;
                m.move.prevMaxTime = delay;
                m.IsFeasible = true;
//              temp_moves->Add(m);

                Sol::FailureCause[n->id] = Parameters::FAILURECAUSE::DELAY;
                Sol::pushVisit[n->id] = (Sol::pushVisit[n->id] == 0) ? delay : std::min(Sol::pushVisit[n->id], delay);

//                Sol::minDelay[prec_del_of_cust->id] = Sol::pushVisit[n->id];

//                Sol::pushVisit[n->id]=0;
                Sol::FailureCount[n->id]++;
                if (Parameters::PENALTY_COST) {
                    newcost.lateDeliveryCost +=
                            std::max(0, delay) *
                            Parameters::LATE_ARRIVAL_PENALTY;
                } else {
                    if (Parameters::SHOW) {
                        Prompt::print(
                                {"node_arr > s.nodeServiceIntervals[", to_string(prec_del_of_cust->id), "].upper + Parameters::TIME_BTW_DELIVERY"});
                        Prompt::print(
                                {to_string(n->id), "(", to_string(n->rank), ") violates delay for ",
                                 to_string(Sol::FailureCount[n->id]), "times",
                                 to_string(delay), " min. LTW=",
                                 to_string(s.LateTW(n)), "arrival = ", to_string(node_arr),
                                 to_string(Sol::FailureCause[n->id])});
                    }
//
                    prev = (next_del == nullptr) ? s.DriverNext[prev->id] : next_del;
                    continue;
                }
            }
        }
        Sol::SetTimingCost(n, node_arr, real_del_time, s.EarlyTW(n), newcost);
        node_arr = std::max(node_arr, real_del_time);

        if (false) {
            if (node_arr > max_arrival_Time) {
                if (Parameters::SHOW) {
                    Prompt::print({"node_arr > max_arrival_Time ", to_string(max_arrival_Time)});
                }
//            m.FailureCause = Parameters::FAILURECAUSE::LATETW;
                m.move.prev = prev;
                m.move.prevMaxTime = s.EndServiceTime[prev->id] - (node_arr - max_arrival_Time);
                m.IsFeasible = true;
//            temp_moves->Add(m);
                if (Parameters::PENALTY_COST) {
                    newcost.lateDeliveryCost += std::max(0, (node_arr - max_arrival_Time));
                } else {
                    prev = s.DriverNext[prev->id];
                    continue;
                }
            }
        }
        node_arr += UNLOADING_DURATION;
        if (node_arr > s.LateTW(n)) {
            if (Parameters::SHOW) {
                Prompt::print({"node_arr > s.LateTW(n) "});
            }
            if (prev->type == Parameters::DELIVERY) {
//                    m.FailureCause = Parameters::FAILURECAUSE::LATETW;
                if (prev_del->custID != n->custID) {
                    m.move.prev = prev;
                    m.move.prevMaxTime = s.EndServiceTime[prev->id] - (node_arr - s.LateTW(n));
                    m.IsFeasible = true;
                    //                temp_moves->Add(m);
                    Sol::FailureCause[n->id] = Parameters::PUSH_PREC_CUSTOMER;
                    Sol::CustomerConflict[n->custID].insert(dynamic_cast<Delivery *>(prev)->custID);
                }
            }
//                if (Parameters::PENALTY_COST) {
//                    newcost.lateDeliveryCost += std::max(0, (s.LateTW(n) - node_arr)) *
//                                                Parameters::LATE_ARRIVAL_PENALTY;
//                }
//                else
            {
                prev = (next_del == nullptr) ? s.DriverNext[prev->id] : next_del;
                continue;
            }
        }
        unloadSlot.upper = node_arr;
        node_arr += Parameters::CLEANING_DURATION;

        TimeSlot intv = TimeSlot(loadSlot.lower, node_arr);
        auto it = s.driverWorkingIntervals[d->id].find(intv);
        if (it != s.driverWorkingIntervals[d->id].end()) {
            Depot *temp_dep = s.GetDepot(it->n.depotID);

            Customer *it_cust =  s.GetCustomer( dynamic_cast<Delivery*>(s.GetNode(it->n.id))->custID);
            if (Parameters::PENALTY_COST) {
                newcost.lateDeliveryCost +=
                        std::max(0,
                                 (std::abs(it->lower - loadSlot.upper))) *
                        Parameters::LATE_ARRIVAL_PENALTY;
            }
            if (Parameters::SHOW) {
                Prompt::print({"it != s.driverWorkingIntervals[", to_string(d->id), "].end())"});
                Prompt::print({" driver ", to_string(d->id), " was busy with ", to_string(it->nodeID), " for node ",
                               to_string(n->id),"cust", to_string( it_cust->custID)});
            }
            Sol::FailureCause[n->id] = Parameters::FAILURECAUSE::DRIVERBUSY;
            Sol::CustomerConflict[n->custID].insert(it_cust->custID);

            prev = (next_del == nullptr) ? s.DriverNext[prev->id] : next_del;
            continue;
        }
        if (next_dock != nullptr) {
            if (node_arr + s.Travel(n, next_dock) + s.Travel(next_dock, next_del) >
                std::max(s.StartServiceTime[next_del->id],
                         Sol::nodeMaxStartService[make_tuple(next_del->id, d->capacity)])) {
                if (Parameters::SHOW) {
//                    printf("fin %d + s.Travel(%d, %d) + s.Travel(%d, %d) =%d > s.StartServiceTime[next_del->id] , ETW %d NC%d\n",
//                           node_arr, n->id, next_dock->id, next_dock->id, next_del->id,
//                           node_arr + s.Travel(n, next_dock) + s.Travel(next_dock,
//                                                                        next_del), s.EarlyTW(next_del),next_del->custID);

                }
                if (Parameters::PENALTY_COST) {
                    newcost.lateDeliveryCost +=
                            std::max(0, (node_arr + s.Travel(n, next_dock) +
                                         s.Travel(next_dock, next_del) -
                                         s.StartServiceTime[next_del->id])) *
                            Parameters::LATE_ARRIVAL_PENALTY;
                } else {
                    prev = next_del;
                    continue;
                }
            }
            if (node_arr + s.Travel(n, next_dock) + s.Travel(next_dock, next_del) +
                Data::UnloadingTime(next_del, next_del->demand, d) >
                s.LateTW(next_del)) {
                if (Parameters::SHOW) {
                    Prompt::print(
                            {"node_arr + s.Travel(n, next) + s.Travel(next, next_del) + next_del->unload_duration > "
                             "LateTW(next_del) ","next client ", to_string(next_del->custID)});
                }
//                if (Parameters::PENALTY_COST) {
//                    newcost.lateDeliveryCost +=
//                            std::max(0, (node_arr + s.Travel(n, next_dock) +
//                                         s.Travel(next_dock, next_del) +
//                                         Data::UnloadingTime(next_del, next_del->demand, d) - s.LateTW(next_del))) *
//                            Parameters::LATE_ARRIVAL_PENALTY;
//                } else
                {

                    prev = next_del;
                    continue;
                }
            }
        }

        newcost.travelCost = s.Travel(prev, dock) + s.Travel(dock, n) +
                             s.Travel(prev->distID, d->distID) - s.Travel(prev->distID, d->distID);

        newcost.waitingCost = newcost.clientWaitingCost + newcost.truckWaitingCost;
        newcost.undeliveredCost=0;
        newcost.satisfiedCost = demand;
        newcost.isFeasible = true;
        newcost += s.updateCost;
        if (newcost.lateDeliveryCost > 100) {
            if (Parameters::SHOW) {
                Prompt::print({"newcost.lateDeliveryCost", to_string(newcost.lateDeliveryCost),
                               "> 100"});
            }
//            prev = s.DriverNext[prev->id];
//            continue;
        }
        if (Parameters::SHOW) {
//            Prompt::print({"Installation possible of del", to_string(n->id), "with driver ", to_string(d->id)});
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
        int rem = d->capacity - s.orderCapRestante[n->orderID];
        if (rem == 0) {
            m.waste = 0;
            m.DeltaCost.undeliveredCost -= s.GetOrder(n->orderID)->demand ;
        } else if (rem > 0) {
            m.waste = s.updateCost.waste + rem;
            m.DeltaCost.undeliveredCost -= s.GetOrder(n->orderID)->demand ;
        } else {
            m.waste = s.updateCost.waste + std::abs(rem + s.GetData()->minDriverCap);
        }

        //m.waste = s.updateCost.waste + d->capacity - o->demand ;
        std::set<int> driverUsed(s.driverUsed);
        std::set<int> clientdriverUsed(s.clientDriverUsed[c->custID]);
//
        driverUsed.insert(d->id);
        clientdriverUsed.insert(d->id);

        if(Parameters::DRIVER_USE==Parameters::MINIMIZEDRIVER::CLIENT ){
            m.allDriver = int(clientdriverUsed.size()) ;
        }
        else if(Parameters::DRIVER_USE==Parameters::MINIMIZEDRIVER::SOLUTION ){
            m.allDriver = int(driverUsed.size()) ;
        }
        else{
            m.clientDriver = int(clientdriverUsed.size()) ;
            m.allDriver = int(driverUsed.size());
        }

        temp_moves->Add(m);
        prev = s.DriverNext[prev->id];
    }

    Move <Delivery, Driver, MoveVrp> best;
    if (temp_moves->Count() > 0) {
        best = temp_moves->GetRandom();
    }
    solcost = best.DeltaCost;
    return best;
}

void InsRmvMethodFast::repairSolution(Sol &s, Order *o) {
    for (int count = s.GetDeliveryCount(o) - 1; count >= 0; count--) {
        Delivery *n = s.GetDelivery(o, count);
        if (s.GetDriverAssignedTo(n) == nullptr) continue;
        int delay = s.WaitingTime[n->id] + Parameters::TIME_BTW_DELIVERY;
        if (delay < 0 and n->rank > 0) {
            Delivery *n1 = s.GetDelivery(o, n->rank - 1);
            s.ArrivalTime[n1->id] = s.ArrivalTime[n1->id] -
                                    delay;//                        s.Update(s.GetDepot(n1->depotID),s.GetDock(n1->dockID),n1);
        }
    }
    s.Update(o);
}

void InsRmvMethodFast::repairSolution(Sol &s, Customer *c) {
}

void InsRmvMethodFast::FillStructures( Sol &s, std::vector<Customer *> & customersList,
                    std::vector<Driver *> & driversList){
    driversList.clear();
    if (not s.availableDrivers.empty())
    {
        for (int i : s.availableDrivers)
        {
            driversList.push_back(s.GetDriver(i));
        }
    }
    else{
        for (int i = 0; i < s.GetDriverCount(); i++)
            driversList.push_back(s.GetDriver(i));
    }
    customersList.clear();
    if (!s.keyCustomers.empty()) {
        for (int i: s.keyCustomers) {
            Customer *c = s.GetCustomer(i);
            if (s.isClientSatisfied(c))
                continue;
            customersList.emplace_back(c);
        }
    } else {
        for (int i = 0; i < s.GetCustomerCount(); i++) {
            Customer *c = s.GetCustomer(i);
            if (s.isClientSatisfied(c))
                continue;
            customersList.emplace_back(c);
        }
    }
}