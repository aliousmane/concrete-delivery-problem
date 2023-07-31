#include "RechercheLocale.h"


using namespace std;

void RechercheLocale::Run(Sol &s) {
    cout << "Start LS after " << s.heurName << endl;
    bestCost = s.GetLastCost();
    found = true;
    std::set<int> keyCustomer = s.keyCustomers;
    std::vector<int> seen(s.GetCustomerCount(), -1);
    CDPSolver::disjointClients.resize(s.GetCustomerCount());
    while (found) {
        found = false;
        customerIdList = vector<int>(s.unscheduledCustomers.begin(), s.unscheduledCustomers.end());
        shuffle(customerIdList.begin(), customerIdList.end(), Parameters::RANDOM_GEN);
        for (auto id: customerIdList) {
            Customer *c1 = s.GetCustomer(id);
            if (LinkedClientSlot[c1->custID].empty())
                continue;
            if (seen[c1->custID] != -1) continue;
            if (s.isClientSatisfied(c1))
                continue;
            TimeSlot Intc1 = TimeSlot(c1->early_tw, c1->late_tw, c1->custID);
            std::set<int> _set;
            for (auto custId: LinkedClientSlot[c1->custID]) {
                Customer *c2 = s.GetCustomer(custId);
                if (!s.isClientSatisfied(c2))
                    continue;
                TimeSlot Intc2 = TimeSlot(c2->early_tw, c2->late_tw, c2->custID);
                if (Intc1 == Intc2) {
                    _set.insert(c2->custID);
                }
                if (RelocateBuild(s, c1, c2)) {
                    break;
                }
//                if (RelocateReStart(s, c1, c2)) {
//                    break;
//                }
                if (SwapBuild(s, c1, c2)) {
                    break;
                }
                if (SwapReStart(s, c1, c2)) {
                    break;
                }
            }
            seen[c1->custID] = c1->custID;
        }
    }
//    cout<<"End LS\n";
//    Parameters::ShowTime();
    s.keyCustomers = keyCustomer;
}

bool RechercheLocale::SwapBuild(Sol &s, Customer *c1, Customer *c2) {
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
//    Prompt::print({"Swap", to_string(c2->custID),"with", to_string(c1->custID)});
    Sol cur = s;
    cur.keyCustomers = clients;
    bool sortie = false;
    cur.UnassignCustomer(c2);
    CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
    if (cur < s) {
        if (cur.GetLastCost().satisfiedCost == s.GetLastCost().satisfiedCost) {
            if (cur.satisfiedCustomers == s.satisfiedCustomers) {
                return false;
            }
        }
        s = cur;
        s.heurName = "Swap Build";
        found = true;
        sortie = true;
//        cout << "new sol swap build " << cur.GetLastCost().satisfiedCost << endl;
        bestCost = s.GetLastCost();
    }
    return sortie;
}

bool RechercheLocale::SwapReStart(Sol &s, Customer *c1, Customer *c2) {
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
//    Prompt::print({"Swap", to_string(c2->custID),"with", to_string(c1->custID)});
    Sol cur = s;
    cur.keyCustomers = clients;
    bool sortie = false;
    cur.UnassignCustomer(c2);
    CDPSolver::SolveInstance(cur, *s.GetData(), 1);
    if (cur < s) {
        if (cur.GetLastCost().satisfiedCost == s.GetLastCost().satisfiedCost) {
            if (cur.satisfiedCustomers == s.satisfiedCustomers) {
                return false;
            }
        }
        s = cur;
        s.heurName = "Swap resolve";
        found = true;
        sortie = true;
//        cout << "new sol swap resolve " << cur.CustomerString() << endl;
        bestCost = s.GetLastCost();

    }
    return sortie;
}


bool RechercheLocale::RelocateBuild(Sol &s, Customer *c1, Customer *c2) {
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
//    cout << " try to relocate " << c1->custID << " near " << c2->custID << " " << s.CustomerString() << endl;
    Sol cur = s;
    cur.keyCustomers = clients;
    CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
    if (cur < s) {
        if (cur.GetLastCost().satisfiedCost == s.GetLastCost().satisfiedCost) {
            if (cur.satisfiedCustomers == s.satisfiedCustomers) {
                return false;
            }
        }
        s = cur;
        s.heurName = "Relocate build";
        found = true;
        sortie = true;
//        cout << "best sol relocate Build) " << cur.GetLastCost().satisfiedCost << endl;
        bestCost = s.GetLastCost();
    }
    return sortie;
}

bool RechercheLocale::RelocateReStart(Sol &s, Customer *c1, Customer *c2) {
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
//    cout << " try to relocate " << c1->custID << " near " << c2->custID << " " << s.CustomerString() << endl;
    Sol cur = s;
    cur.keyCustomers = clients;
    CDPSolver::SolveInstance(cur, *s.GetData(), 1);
    if (cur < s) {
        if (cur.GetLastCost().satisfiedCost == s.GetLastCost().satisfiedCost) {
            if (cur.satisfiedCustomers == s.satisfiedCustomers) {
                return false;
            }
        }
        s = cur;
        s.heurName = "Relocate resolve";
        found = true;
        sortie = true;
//        cout << "best sol relocate resolve) " << cur.GetLastCost().satisfiedCost << endl;
        bestCost = s.GetLastCost();
    }
    return sortie;
}