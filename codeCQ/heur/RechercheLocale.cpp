#include "RechercheLocale.h"

using namespace std;

void RechercheLocale::Run(Sol &s) {
    cout << "Start LS after " << s.heurName << s.GetLastCost() << endl;
    Prompt::print(s.unscheduledCustomers);
    keyCustomer = s.keyCustomers;
    bestCost = s.GetLastCost();
    RelocateStartLoad(s);
    if (!s.unscheduledCustomers.empty()) {
        UnscheduledFirst(s);
        RemoveAndReschedule(s);
    }
    LoadBackward(s);
    RelocateDriver(s);
//    if (s.unscheduledCustomers.empty())
    {
        RunAllFeasible(s);
    }
    LoadBackward(s);
    RelocateDriver(s);
    RelocateStartLoad(s);
    cout << "End LS" << s.GetLastCost() << endl;
    s.keyCustomers = keyCustomer;
}

bool RechercheLocale::UnscheduledFirst(Sol &s) {
    bool sortie = false;
    Sol cur(s.GetData());
    cout << "UnscheduledFirst" << endl;
    keyCustomer = s.keyCustomers;
    cur.keyCustomers = s.unscheduledCustomers;
    CDPSolver::SolveInstance(cur, *cur.GetData(), 1);
    cur.keyCustomers = s.satisfiedCustomers;
    CDPSolver::BuildOnSolution(cur, *cur.GetData(), 1);
    if (cur < s) {
        s = cur;
        s.keyCustomers = keyCustomer;
        sortie = true;
    }
    if (sortie) {
//        cout << "improves" << s.GetLastCost() << endl;
    }
    return sortie;
}

bool RechercheLocale::RelocateStartLoad(Sol &s) {
    cout << "RelocateStartLoad" << s.GetLastCost() << endl;
    keyCustomer = s.keyCustomers;
    runtime = Parameters::GetElapsedTime();
    bool sortie = false;
    found = true;
    while (found) {
        found = false;
        if (s.lateCustomers.empty()) break;
        if (Parameters::GetElapsedTime() - runtime > 1000) break;
        for (int cID: s.lateCustomers) {
            Customer *c = s.GetCustomer(cID);
            auto *del = dynamic_cast<Delivery *>( s.CustomerNext[c->StartNodeID]);
            Depot *dep = s.GetDepot(del->depotID);

            double start = s.nodeServiceIntervals[del->id].lower + s.WaitingTime[del->id];
            TimeSlot bestSlot = TimeSlot(start, start + Data::LoadingTime(dep, s.DeliveryLoad[del->delID]));
            auto it_load = s.depotLoadingIntervals[dep->depotID].find(bestSlot);
            if (it_load != s.depotLoadingIntervals[dep->depotID].end()) {
                if (it_load != s.depotLoadingIntervals[dep->depotID].begin()) {
                    auto it_prev = std::prev(it_load);
//                    cout << "prev_slot " << *it_prev << " " << *it_load << " " << endl;
                    double prev_end = it_prev->upper;
                    if (prev_end + bestSlot.upper - bestSlot.lower < start) {
//                        cout << it_prev->upper << " -" << it_prev->upper + it_load->upper - it_load->lower << endl;
                        Sol cur(s);
                        Node *next_node = s.CustomerNext[del->id];
                        while (next_node->type != Parameters::TypeNode::END_LINK) {
                            if (next_node->type == Parameters::DELIVERY) {
                                cur.UnassignDelivery({dynamic_cast<Delivery *>(next_node)});
                            }
                            next_node = s.CustomerNext[next_node->id];
                        }
                        cur.UnassignDelivery({del});
                        cur.keyCustomers = s.unscheduledCustomers;
                        cur.keyCustomers.insert(c->custID);
                        Sol::FixStartLoad[del->delID] = prev_end;
                        CDPSolver::BuildOnSolution(cur, *cur.GetData(), 1);
                        Sol::FixStartLoad[del->delID] = -1;
                        if (cur < s) {
                            s = cur;
                            s.keyCustomers = keyCustomer;
                            found = true;
                            sortie = true;
                            break;
                        }
                    }
                }
            }
        }
    }
    s.keyCustomers = keyCustomer;
    if (sortie) {
//        cout << "RelocateStartLoad improves:" << s.GetLastCost() << endl;
    }
    return sortie;
}


bool RechercheLocale::LoadBackward(Sol &s) {
    cout << "LoadBackward" << endl;
    bool sortie = false;
    std::set<int> lateCustomers = s.lateCustomers;
    for (auto cId: lateCustomers) {
        Customer *c = s.GetCustomer(cId);
        Sol cur(s);
        cur.keyCustomers = {c->custID};
        cur.UnassignCustomer(c);
        const int ins = Parameters::LOAD_INSERTION;
        Parameters::LOAD_INSERTION = Parameters::DEPOTINSERTION::BACKWARD;
        CDPSolver::BuildOnSolution(cur, *cur.GetData(), 1);
        if (cur < s) {
            s = cur;
            sortie = true;
        }
        Parameters::LOAD_INSERTION = ins;
    }
    return sortie;
}

void RechercheLocale::RunAllFeasible(Sol &s) {
    cout << "RunAllFeasible" << endl;

    runtime = Parameters::GetElapsedTime();
    found = true;
    std::vector<int> seen(s.GetOrderCount(), -1);
    std::vector<int> orderList(s.GetOrderCount());
    std::iota(orderList.begin(), orderList.end(), 0);
    std::shuffle(orderList.begin(), orderList.end(), Parameters::RANDOM_GEN);
    const int BACKTRACK_DEPTH = Parameters::BACKTRACK_DEPTH;
    while (found) {
        if (Parameters::GetElapsedTime() - runtime > 1000) break;
        found = false;
        for (int i: orderList) {
            Order *o = s.GetOrder(i);
            if (s.orderLoads[o->orderID].size() == 1) continue;
            if (seen[o->orderID] != -1) continue;
            if (o->demand > s.GetData()->minDriverCap && o->demand <= s.GetData()->maxDriverCap) {
                if (UseSingleDriver(s, o)) {
                    continue;
                }
            }
            if (SwapLoad(s, o)) {
                found = true;
                seen[o->orderID] = o->orderID;
                continue;
            }
        }
    }
//    cout<<"End LS "<<s.GetLastCost()<<endl;
    Parameters::BACKTRACK_DEPTH = BACKTRACK_DEPTH;
}

void RechercheLocale::RemoveAndReschedule(Sol &s) {
    cout << "RemoveAndReschedule" << endl;
//    Parameters::ShowTime();
    bool sortie = false;
    found = true;
    runtime = Parameters::GetElapsedTime();
    keyCustomer = s.keyCustomers;
    std::vector<int> seen(s.GetCustomerCount(), -1);
    CDPSolver::disjointClients.resize(s.GetCustomerCount());
    while (found) {
        found = false;
        customerIdList = vector<int>(s.unscheduledCustomers.begin(), s.unscheduledCustomers.end());
        shuffle(customerIdList.begin(), customerIdList.end(), Parameters::RANDOM_GEN);
        for (auto id: customerIdList) {
            if (s.unscheduledCustomers.empty())
                break;
            if (Parameters::GetElapsedTime() - runtime > 1000) break;

            Customer *c1 = s.GetCustomer(id);
            if (LinkedClientSlot[c1->custID].empty())
                continue;
            if (seen[c1->custID] != -1) continue;
            if (s.isClientSatisfied(c1))
                continue;

            for (auto custId: LinkedClientSlot[c1->custID]) {
                Customer *c2 = s.GetCustomer(custId);
                if (!s.isClientSatisfied(c2))
                    continue;

                if (Relocate2(s, c1, c2)) {
                    sortie = true;
                    break;
                }
            }
            seen[c1->custID] = c1->custID;
        }
    }
    s.keyCustomers = keyCustomer;
//    Parameters::ShowTime();
    if (sortie) {
//        cout << "improves" << s.GetLastCost() << endl;
    }

}

bool RechercheLocale::RelocateDriver(Sol &s) {
    bool sortie = false;
    keyCustomer = s.keyCustomers;
    availableDriver = s.availableDrivers;
    cout << "RelocateDriver" << endl;

    found = true;
    runtime = Parameters::GetElapsedTime();
    while (found) {
        found = false;
        if (Parameters::GetElapsedTime() - runtime > 1000) break;
        for (int i = 0; i < s.GetDriverCount(); i++) {
            Driver *di = s.GetDriver(i);
//            double underwork = Sol::GetUnderWorkCost(s.shiftDuration[di->id]);
//            if (underwork < 0.1)continue;
            double overtime1 = Sol::GetOvertimeCost(s.shiftDuration[di->id]);
            if (overtime1 > 0.1)continue;
//        cout << *di << " "<<underwork<< " "<<s.shiftDuration[di->id]<<endl;
            for (int j = i + 1; j < s.GetDriverCount(); j++) {
                Driver *dj = s.GetDriver(j);
                double overtime = Sol::GetOvertimeCost(s.shiftDuration[dj->id]);
                if (overtime < 0.1)continue;
//            cout << "dj " << *dj << " "<< s.shiftDuration[dj->id]<< " "<<overtime<< endl;
                Node *prev = s.DriverNext[dj->StartNodeID];
                while (prev->type != Parameters::TypeNode::END_LINK) {

                    if (prev->type == Parameters::TypeNode::DELIVERY) {
                        Sol cur(s);
                        Delivery *del = cur.GetDelivery(prev);
                        cur.RemoveDelivery(del);
                        cur.keyCustomers = {del->custID};
                        cur.availableDrivers = {di->id};
                        CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
                        if (cur < s) {
                            found = true;
                            sortie = true;
                            cout<<" found "<<cur.GetLastCost()<<endl;
                            s = cur;
                            s.keyCustomers = keyCustomer;
                            s.availableDrivers = availableDriver;
                            break;
                        }
                    }
                    prev = s.DriverNext[prev->id];
                }
                if (found) {
                    break;
                }
            }
        }
    }
    return sortie;
}

bool RechercheLocale::UseSingleDriver(Sol &s, Order *o) {
    bool sortie = false;
    Delivery *di = s.GetDelivery(o, 0);
    Sol::FixLoad[di->delID] = o->demand;
    Sol cur = s;
    cur.UnassignCustomer(o->custID);
    cur.keyCustomers = s.unscheduledCustomers;
    for (auto c_ID: s.unscheduledCustomers) {
        cur.UnassignCustomer(c_ID);
    }
    cur.keyCustomers.insert(o->custID);
    CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
    Sol::FixLoad[di->delID] = -1;
    if (cur < s) {
        s = cur;
        sortie = true;
    }
    return sortie;
}

bool RechercheLocale::SwapLoad(Sol &s, Order *o) {
    bool sortie = false;
    Sol cur(s);
    keyCustomer = s.keyCustomers;
    std::vector<int> DeliveryList(s.GetDeliveryCount(o));
    int depth = 0;
    for (int i = 0; i < s.GetDeliveryCount(o); i++) {
        if (sortie) break;
        Delivery *di = s.GetDelivery(o, i);
        for (int j = i + 1; j < s.GetDeliveryCount(o); j++) {
            Delivery *dj = s.GetDelivery(o, j);
            if (cur.GetDriverAssignedTo(dj) == nullptr) break;
            if (cur.DeliveryLoad[di->delID] == cur.DeliveryLoad[dj->delID])
                continue;
            if (depth++ > 3) {
                depth = 0;
                continue;
            }
            Sol::FixLoad[di->delID] = cur.DeliveryLoad[dj->delID];
            Sol::FixLoad[dj->delID] = cur.DeliveryLoad[di->delID];
            Parameters::BACKTRACK_DEPTH = 2;
            cur.UnassignCustomer(o->custID);
            cur.keyCustomers = s.unscheduledCustomers;
            for (auto c_ID: s.unscheduledCustomers) {
                cur.UnassignCustomer(c_ID);
            }
            cur.keyCustomers.insert(o->custID);
            CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
            Sol::FixLoad[di->delID] = -1;
            Sol::FixLoad[dj->delID] = -1;
            if (cur < s) {
                s = cur;
//                cout << "Swap load same order " << s.GetLastCost() << endl;
                sortie = true;
                break;
            }
        }
    }
    s.keyCustomers = keyCustomer;
    return sortie;
}


bool RechercheLocale::Swap1(Sol &s, Customer *c1, Customer *c2) {
    if (c1->demand < c2->demand) return false;
    std::set<int> clients = s.satisfiedCustomers;
    clients.erase(c2->custID);
    clients.insert(c1->custID);

    std::set<int> v_intersection;
    std::set_intersection(clients.begin(), clients.end(),
                          CDPSolver::disjointClients[c1->custID].begin(), CDPSolver::disjointClients[c1->custID].end(),
                          std::inserter(v_intersection, v_intersection.end()));
    if (!v_intersection.empty())
        return false;
    if (CDPSolver::ComputeCost(s, clients) < bestCost.satisfiedCost)
        return false;
    Prompt::print({"Swap", to_string(c2->custID), "with", to_string(c1->custID)});
    Sol cur = s;
    cur.keyCustomers = clients;
    bool sortie = false;
    cur.UnassignCustomer(c2);
    CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
    if (cur < s) {
        if (cur.GetLastCost().satisfiedCost == s.GetLastCost().satisfiedCost) {
            if (cur.satisfiedCustomers != s.satisfiedCustomers) {
                return sortie;
            }
        }
        s = cur;
        s.heurName = "Swap Build";
        found = true;
        sortie = true;
//        cout << "new sol swap " << cur.CustomerString() << endl;
        if (cur.GetLastCost() < bestCost) {
            bestCost = s.GetLastCost();
        }
    }
    return sortie;
}

bool RechercheLocale::Swap2(Sol &s, Customer *c1, Customer *c2) {
    if (c1->demand < c2->demand) return false;
    std::set<int> clients = s.satisfiedCustomers;
    clients.erase(c2->custID);
    clients.insert(c1->custID);

    std::set<int> v_intersection;
    std::set_intersection(clients.begin(), clients.end(),
                          CDPSolver::disjointClients[c1->custID].begin(), CDPSolver::disjointClients[c1->custID].end(),
                          std::inserter(v_intersection, v_intersection.end()));
    if (!v_intersection.empty())
        return false;
    if (CDPSolver::ComputeCost(s, clients) < bestCost.satisfiedCost)
        return false;
    Prompt::print({"Swap", to_string(c2->custID), "with", to_string(c1->custID)});
    Sol cur = s;
    cur.keyCustomers = clients;
    bool sortie = false;
    cur.UnassignCustomer(c2);
    CDPSolver::SolveInstance(cur, *s.GetData(), 1);
    if (cur < s) {
        if (cur.GetLastCost().satisfiedCost == s.GetLastCost().satisfiedCost) {
            if (cur.satisfiedCustomers != s.satisfiedCustomers) {
                return sortie;
            }
        }
        s = cur;
        s.heurName = "Swap resolve";
        found = true;
        sortie = true;
//        cout << "new sol swap " << cur.CustomerString() << endl;
        if (cur.GetLastCost() < bestCost) {
            bestCost = s.GetLastCost();
        }
    }
    return sortie;
}

bool RechercheLocale::Relocate1(Sol &s, Customer *c1, Customer *c2) {
    std::set<int> v_intersection;
    std::set_intersection(s.satisfiedCustomers.begin(), s.satisfiedCustomers.end(),
                          CDPSolver::disjointClients[c1->custID].begin(), CDPSolver::disjointClients[c1->custID].end(),
                          std::inserter(v_intersection, v_intersection.end()));
    if (!v_intersection.empty())
        return false;
    set<int> clients;// = s.satisfiedCustomers;
    clients.insert(c1->custID);
    if (CDPSolver::ComputeCost(s, clients) < bestCost.satisfiedCost)
        return false;
    bool sortie = false;
//    cout << " try to relocate1 " << c1->custID << " near " << c2->custID << " " << s.CustomerString() << endl;
    Sol cur = s;
    cur.UnassignCustomer(c1);
    cur.keyCustomers = clients;
    CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
    if (cur < s) {
//        if (cur.GetLastCost().satisfiedCost == s.GetLastCost().satisfiedCost) {
//            if (cur.satisfiedCustomers != s.satisfiedCustomers) {
//                return sortie;
//            }
//        }
        s = cur;
        s.heurName = "Relocate build";
        found = true;
        sortie = true;
//        cout << "best sol relocate 1) " << cur.CustomerString() << endl;
        cout << "best sol relocate 1) " << endl;
        if (s.GetLastCost() < bestCost) {
            bestCost = s.GetLastCost();
        }
    }
    return sortie;
}

bool RechercheLocale::Relocate2(Sol &s, Customer *c1, Customer *c2) {

    set<int> clients = s.satisfiedCustomers;
    clients.insert(c1->custID);
    if (CDPSolver::ComputeCost(s, clients) < bestCost.satisfiedCost)
        return false;
    keyCustomer = s.keyCustomers;
    bool sortie = false;
//    cout << " try to relocate2  " << c1->custID << " near " << c2->custID << endl;
    Sol cur = s;
    cur.UnassignCustomer(c2);
    cur.keyCustomers = clients;
//    CDPSolver::SolveInstance(cur, *s.GetData(), 1);
    CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
    if (cur < s) {
        s = cur;
        s.keyCustomers = keyCustomer;
        s.heurName = "Relocate resolve";
        found = true;
        sortie = true;
//        cout << "best sol relocate 2) " << cur.CustomerString() << endl;
        bestCost = s.GetLastCost();
//        cout << "best sol relocate 2) " << bestCost << endl;
    }
    s.keyCustomers = keyCustomer;
    return sortie;
}

bool RechercheLocale::ShiftLoading(Sol &s) {
    bool sortie = false;

    for (auto cId: s.unscheduledCustomers) {
        Customer *c = s.GetCustomer(cId);
//        cout<<*c<<endl;
//        Prompt::print(s.depotLoadingIntervals[c->depotID]);
        set<int> UsedOrder;
        Order *cur_order = s.GetRandomOrder(c, UsedOrder);
        for (int j = 0; j < s.GetDeliveryCount(cur_order);) {
            Delivery *del = s.GetDelivery(cur_order, j);
            exit(1);
        }
        exit(1);
    }

    return sortie;
}
