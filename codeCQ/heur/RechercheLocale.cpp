#include "RechercheLocale.h"
#include "../Prompt.h"

using namespace std;

void RechercheLocale::Run(Sol &s) {
    cout << "Start LS after " << s.heurName << s.GetLastCost() << endl;
    Prompt::print(s.unscheduledCustomers);
    set<int> keyCustomer(s.keyCustomers);
    bestCost = s.GetLastCost();
    if (!s.unscheduledCustomers.empty()) {
//        ShiftLoading(s);
        UnscheduledFirst(s);
        RemoveAndReschedule(s);
    }
//    if (s.unscheduledCustomers.empty())
    {
        RunAllFeasible(s);
    }
//    RelocateDriver(s);
//    return;
    LoadBackward(s);
    cout << "End LS" << s.GetLastCost() << endl;
    s.keyCustomers = keyCustomer;
}

bool RechercheLocale::UnscheduledFirst(Sol &s) {
    bool sortie = false;
    Sol cur(s.GetData());
    cout<<"UnscheduledFirst"<<endl;
    cur.keyCustomers = s.unscheduledCustomers;
    CDPSolver::SolveInstance(cur, *cur.GetData(), 1);
    cur.keyCustomers = s.satisfiedCustomers;
    CDPSolver::BuildOnSolution(cur, *cur.GetData(), 1);
    if (cur < s) {
        s = cur;
        sortie = true;
    }
    return sortie;
}

bool RechercheLocale::LoadBackward(Sol &s) {
    cout<<"LoadBackward"<<endl;

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
    cout<<"RunAllFeasible"<<endl;

    runtime = Parameters::GetElapsedTime();
    found = true;
    std::vector<int> seen(s.GetOrderCount(), -1);
    std::vector<int> orderList(s.GetOrderCount());
    std::iota(orderList.begin(), orderList.end(), 0);
    std::shuffle(orderList.begin(), orderList.end(), Parameters::RANDOM_GEN);
    while (found) {
        if (Parameters::GetElapsedTime() - runtime > 30) break;
        found = false;
        for (int i: orderList) {
            Order *o = s.GetOrder(i);
            if (s.orderLoads[o->orderID].size() == 1) continue;
            if (seen[o->orderID] != -1) continue;
//            cout << o->orderID << "-" << o->custID << " D:" << o->demand << endl;
            if (o->demand > s.GetData()->minDriverCap && o->demand <= s.GetData()->maxDriverCap) {
                if (UseSingleDriver(s, o)) {
                    continue;
                }
            }
            if (SwapLoad(s, o)) {
                found = true;
                seen[o->orderID] = o->orderID;
                continue;
//                break;
            }
        }
//        std::shuffle(orderList.begin(),orderList.end(),Parameters::RANDOM_GEN);
    }
//    cout<<"End LS "<<s.GetLastCost()<<endl;
    Parameters::BACKTRACK_DEPTH = 20;
}

void RechercheLocale::RemoveAndReschedule(Sol &s) {
    cout<<"RemoveAndReschedule"<<endl;

    found = true;
    runtime = Parameters::GetElapsedTime();
    std::set<int> keyCustomer = s.keyCustomers;
    std::vector<int> seen(s.GetCustomerCount(), -1);
    CDPSolver::disjointClients.resize(s.GetCustomerCount());
    while (found) {
        found = false;
        if (Parameters::GetElapsedTime() - runtime > 30) break;
        customerIdList = vector<int>(s.unscheduledCustomers.begin(), s.unscheduledCustomers.end());
        shuffle(customerIdList.begin(), customerIdList.end(), Parameters::RANDOM_GEN);
        for (auto id: customerIdList) {
            if (s.unscheduledCustomers.empty())
                break;

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
                    break;
                }
            }
            seen[c1->custID] = c1->custID;
        }
    }
    s.keyCustomers = keyCustomer;

}

bool RechercheLocale::RelocateDriver(Sol &s) {

    for (int i = 0; i < s.GetDriverCount(); i++) {
        Driver *di = s.GetDriver(i);
        if (Sol::GetUnderWorkCost(s.shiftDuration[di->id]) == 0)
            continue;
        cout << *di << endl;
        for (int j = i + 1; j < s.GetDriverCount(); j++) {
            Driver *dj = s.GetDriver(j);
            if (Sol::GetOvertimeCost(s.shiftDuration[dj->id]) > 0)
                continue;
            cout << "dj " << *dj << endl;
            // Relocate node of di inside node of dj
        }
    }

    exit(1);
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
//    CDPSolver::SolveInstance(cur, *s.GetData(), 1);
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
    set<int> keyCustomer = s.keyCustomers;
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
//            cout << "Deliveries " << di->id << "->" << dj->id << endl;
            Sol::FixLoad[di->delID] = cur.DeliveryLoad[dj->delID];
            Sol::FixLoad[dj->delID] = cur.DeliveryLoad[di->delID];
            Parameters::BACKTRACK_DEPTH = 2;
//            cur.ShowSchedule(o);
//            cout<<"------\n";
            cur.UnassignCustomer(o->custID);
            cur.keyCustomers = s.unscheduledCustomers;
            for (auto c_ID: s.unscheduledCustomers) {
                cur.UnassignCustomer(c_ID);
            }
            cur.keyCustomers.insert(o->custID);
//            CDPSolver::SolveInstance(cur, *s.GetData(), 1);
            CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);

//            cur.ShowSchedule(o);
//            cout<<" After ------\n";

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
    std::set<int> v_intersection;
    std::set_intersection(s.satisfiedCustomers.begin(), s.satisfiedCustomers.end(),
                          CDPSolver::disjointClients[c1->custID].begin(), CDPSolver::disjointClients[c1->custID].end(),
                          std::inserter(v_intersection, v_intersection.end()));
    if (!v_intersection.empty())
        return false;
    set<int> clients = s.satisfiedCustomers;
    clients.insert(c1->custID);
    if (CDPSolver::ComputeCost(s, clients) < bestCost.satisfiedCost)
        return false;
    bool sortie = false;
//    cout << " try to relocate2  " << c1->custID << " near " << c2->custID << " " << s.CustomerString() << endl;
    Sol cur = s;
    cur.UnassignCustomer(c1);
    cur.keyCustomers = clients;
    CDPSolver::SolveInstance(cur, *s.GetData(), 1);
    if (cur < s) {
        s = cur;
        s.heurName = "Relocate resolve";
        found = true;
        sortie = true;
//        cout << "best sol relocate 2) " << cur.CustomerString() << endl;
//        if (s.GetLastCost() < bestCost)
        {
            bestCost = s.GetLastCost();
        }
//        cout << "best sol relocate 2) " << bestCost << endl;
    }
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
