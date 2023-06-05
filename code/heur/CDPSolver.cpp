
#include "CDPSolver.h"
#include "CustInsertion.h"
#include "CustInsertion2.h"
#include "CustInsertionBacktrack.h"
#include "AllInsertionOperator.h"
#include "DriverInsertion.h"
#include "InsRmvMethodFast.h"
#include "InsRmvBuilder1.h"
#include "InsRmvBuilder2.h"
#include "InsRmvBuilder3.h"
#include <istream>


using namespace std;
std::vector<std::set<int>> CDPSolver::disjointClients = vector<set<int>>();
std::vector<InsertOperator<Node, Driver> *> CDPSolver::listOperators =
        std::vector<InsertOperator<Node, Driver> *>();
vector<int> CDPSolver::nbSatisfied = vector<int>();

void CDPSolver::run() {}

std::set<int> CDPSolver::EliminateCustomer(Data &data, const int iter) {
    std::set<int> feasibleClients;
    for (int i = 0; i < data.GetCustomerCount(); i++) {
        std::vector<int> temp{i};
        Data dat = data.copyCustomersData(temp);
        Sol s(&dat);
        s.keyCustomers = {0};
        InsRmvBuilder3 builder3(dat);
        CustInsertionBacktrack custIns_BT3(dat, builder3);
        custIns_BT3.Insert(s);

//        CDPSolver::SolveInstance(s, dat, iter);
        if (s.isFeasible) {
            cout << i << "-" << std::flush;
            feasibleClients.insert(i);
//             s.ShowSchedule();
//             cout<<s.GetCost()<<endl;

        } else {
            cout << "[" << i << "]-" << std::flush;
        }
    }
    cout << endl;
    return feasibleClients;
}

void CDPSolver::SolveCDP(Sol &s, Data &dat, int iter, bool restart) {

    InsRmvBuilder1 builder1(dat);
    InsRmvBuilder2 builder2(dat);
    InsRmvBuilder3 builder3(dat);
    CustInsertion custIns1(dat, builder1);
    CustInsertion custIns2(dat, builder2);
    CustInsertion custIns3(dat, builder3);
    CustInsertion2 custIns23(dat, builder3);
    CustInsertionBacktrack custIns_BT3(dat, builder3);
    PriorityQueueInsertion prioIns3(dat, builder3);
    DriverInsertion driverIns3(dat, builder3);

    vector<pair<int, string>> custInfo = {
            {0, "Cust Sort Early TW"},
            {1, "Cust Sort Greater D"},
            {2, "Cust Sort Late TW "},
            {3, "Cust Sort Min Width TW"},
            {4, "Cust Random"},
//            {5, "Cust Sort Kinable"},
//            {-1, "Cust customized Sort"}
    };
    vector<pair<int, string>> priorityInfo = {
            {0, "PrioriSort I Early TW"},
            {1, "PrioriSort I Late TW"},
            {2, "PrioriSort D Early TW"},
            {4, "PrioriSort D Demand"},
            {5, "PrioriSort I Demand"},
//            {6, "PrioriSort I TW width"},
//            {7, "PrioriSort D TW width"},
    };
    vector<pair<int, string>> driverInfo = {
            {0, "Driver I Cap"},
            {1, "Driver D Cap"},
            {2, "Driver Random Cap "},
    };

    vector<AllInsertionOperator> AllInsertionOp;
    AllInsertionOp.reserve(3 * (custInfo.size() + priorityInfo.size()+ driverInfo.size()));
    for (const auto &val: custInfo) {
        AllInsertionOp.emplace_back(&custIns3, val.first, "Builder 3 " + val.second);
    }
    for (const auto &val: priorityInfo) {
//        AllInsertionOp.emplace_back(&prioIns3, val.first, "Builder 3 " + val.second);
    }
    for (const auto &val: driverInfo) {
//            AllInsertionOp.emplace_back(&driverIns3, val.first, "Builder 3 " + val.second);
    }
    Sol best = s;
    for (int i = 0; i < iter; i++) {
        for (auto heur: AllInsertionOp) {
            Sol cur(&dat,s.keyCustomers);
            if (!restart) {
                cur = s;
            }
            cur.availableDrivers = s.availableDrivers;
//            Prompt::print({"Iter", to_string(i), heur.name});
            heur.Insert(cur);
            if (cur < best) {
//                cout << "current best cost " << cur.GetCost().satisfiedCost << " with " << heur.name << endl;
                best = cur;
                if (s.hasScheduled(s.keyCustomers))
                    break;
            }
        }
    }
    s = best;
}

double CDPSolver::ComputeCost(Sol &s, const std::set<int>& setClients) {
    double sum = 0;
    for (auto i: setClients) {
        sum += s.GetCustomer(i)->demand;
    }
    return sum;
}

void CDPSolver::SolveInstance(Sol &s, Data &dat, int iter) {
    CDPSolver::SolveCDP(s, dat, iter, true);
}

void CDPSolver::BuildOnSolution(Sol &s, Data &dat, int iter) {
    CDPSolver::SolveCDP(s, dat, iter, false);
}

void CDPSolver::find_all_routes(Sol &s, Customer *c, std::unordered_map<std::string, Sol, MyHashFunction> *umap) {

    InsRmvBuilder3 builder3(*s.GetData());

    Sol::FailureCause.resize(s.GetNodeCount(), Parameters::FAILURECAUSE::NONE);
    Sol::FailureCount.resize(s.GetNodeCount(), 0);
    Sol::StartBefore.resize(s.GetNodeCount(), 0);
    Sol::pullVisit.resize(s.GetNodeCount(), 0);
    Sol::pushVisit.resize(s.GetNodeCount(), 0);
    Delivery *del = s.GetDeliveries(c)[0];
    Sol cur(s.GetData());
    RecursiveSols(cur, del, umap);
    set<int> sumServiceTime;
    if (umap != nullptr) {
//        cout<<"solution count "<<umap->size()<<endl;
        for (const auto &val: *umap) {
            Sol cur1 = val.second;
//            cout<<val.first<<endl;
            //        cur.ShowSchedule();
            sumServiceTime.insert(cur1.sumServiceTime[c->custID]);
        }
    }

}


void CDPSolver::RecursiveSols(Sol &s, Delivery *del, std::unordered_map<std::string, Sol, MyHashFunction> *umap) {
    Order *cur_order = s.GetOrder(del->orderID);
    InsRmvBuilder3 builder3(*s.GetData());
    for (auto cap2: s.GetData()->driverCapacities) {
        for (int k = 0; k < s.GetDriverCount(); k++) {
            if (s.GetDriver(k)->capacity != cap2) {
                continue;
            }
            Driver *d = s.GetDriver(k);
            Sol cur = s;
            Driver *used_d = cur.GetDriverAssignedTo(del);
            if (used_d != nullptr) {
                if (used_d->capacity == d->capacity)
                    break;
            }
            Move<Delivery, Driver, MoveVrp> m;
            builder3.GetBestInsertion(cur, {del->delID}, {d}, m);
            if (m.IsFeasible) {
                builder3.ApplyInsertMove(cur, m);
                cur.Update(m.move.depot, m.move.dock, m.n);
                if (cur.isSatisfied(cur_order)) {
                    if (umap != nullptr) {
                        if (umap->find(cur.toString()) == umap->end()) {
                            (*umap)[cur.toString()] = cur;
                        }
                    }
                    break;
                } else {
                    Delivery *next = cur.GetNextIdleDelivery(cur_order);
                    if (next == nullptr) {
                        continue;
                    } else {
                        RecursiveSols(cur, next, umap);
                    }
                }
            } else {
                if (del->rank > 0) {
                    Delivery *prec_del = cur.GetDelivery(cur_order, del->rank - 1);
                    if (Sol::FailureCount[del->id] < 3 &&
                        Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::DELAY) {
                        Sol::minDelay[prec_del->id] = Sol::pushVisit[del->id];
                        Sol::minDelay[del->id] = 0;
                        Sol::pushVisit[del->id] = 0;
                        Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                        Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
                        cur.UnassignDelivery({prec_del});
                        prec_del->isdelayed = true;
                        RecursiveSols(cur, prec_del, umap);
                    }
                }
                continue;
            }
        }
    }
}


void CDPSolver::LearnParameters(Sol &s, std::set<int> const &customer) {
    for (auto i: customer) {
        Customer *c = s.GetCustomer(i);
        unordered_map<string, Sol, MyHashFunction> myMap;
        Sol cur1(s.GetData());
        find_all_routes(cur1, c, &myMap);
        int count = 0;
        for (const auto &val: myMap) {
            Sol cur = val.second;
            cout << count++ << "-" << val.first << endl;
            Node *prev = cur.CustomerNext[c->StartNodeID];
            while (prev->type != Parameters::END_LINK) {
                Driver *d = cur.GetDriverAssignedTo(prev);
                if (prev->type == Parameters::DELIVERY) {
                    auto tup = make_tuple(prev->id, d->capacity);

                    Sol::nodeMaxStartService[tup] = std::max(Sol::nodeMaxStartService[tup],
                                                             (int) cur.StartServiceTime[prev->id]);
                }
                Node *next = cur.CustomerNext[prev->id];
                prev = next;
            }
        }
    }
}


void CDPSolver::findCorrelation(Data &dat, std::vector<TimeSlot> const &listInt,
                                std::vector<std::set<int>> &linkedClientSlot,
                                std::vector<std::set<int>> &linkedClientDemand,
                                std::vector<std::set<int>> &linkedClients,
                                std::vector<std::set<int>> &linkedClientsInf,
                                std::vector<std::set<int>> &linkedClientSup
) {
    linkedClientSlot.clear();
    linkedClientDemand.clear();
    linkedClients.clear();
    linkedClientsInf.clear();
    linkedClientSup.clear();
    linkedClientSlot.resize(dat.GetCustomerCount());
    linkedClientDemand.resize(dat.GetCustomerCount());
    linkedClients.resize(dat.GetCustomerCount());
    linkedClientsInf.resize(dat.GetCustomerCount());
    linkedClientSup.resize(dat.GetCustomerCount());
    for (int i = 0; i < listInt.size(); i++) {
        auto intv1 = listInt[i];
        Customer *c1 = dat.GetCustomer(intv1.nodeID);
        Depot *dep1 = dat.GetDepot(c1->depotID);
        for (int j = 0; j < listInt.size(); j++) {
            if (i == j)
                continue;
            auto intv2 = listInt[j];
            Customer *c2 = dat.GetCustomer(intv2.nodeID);
            Depot *dep2 = dat.GetDepot(c2->depotID);
            if (intv1 == intv2) {
                linkedClientSlot[intv1.nodeID].insert(intv2.nodeID);
                linkedClients[intv1.nodeID].insert(intv2.nodeID);
            } else if (intv1 < intv2) {
                if (intv1.upper + dat.Travel(c1, c2) >= intv2.lower) {
                    linkedClients[intv1.nodeID].insert(intv2.nodeID);
                }

                linkedClientSup[intv1.nodeID].insert(intv2.nodeID);
            } else if (intv1 > intv2) {
                if (intv2.upper + dat.Travel(c2, c1) >= intv1.lower) {
                    linkedClients[intv1.nodeID].insert(intv2.nodeID);
                }

                linkedClientsInf[intv1.nodeID].insert(intv2.nodeID);
            }

            if (c1->demand == c2->demand) {
                linkedClientDemand[intv1.nodeID].insert(intv2.nodeID);
                linkedClients[intv1.nodeID].insert(intv2.nodeID);
            }
        }
    }
}

void CDPSolver::findDisjointSet(Data &dat, std::vector<std::set<int>> const &linkedClients) {
    map<set<int>, bool> seen_pairs;

    for (int i = 0; i < linkedClients.size(); i++) {
        auto _set = linkedClients[i];
        if (_set.empty())
            continue;
        for (auto val: _set) {
            set<int> my_set{i, val};
            if (seen_pairs.find(my_set) != seen_pairs.end()) {
                continue;
            } else {
                seen_pairs[my_set] = true;
//                Prompt::print(my_set);
                Data dat1 = dat.copyCustomersData(my_set);
                Sol cur(&dat1);
                SolveInstance(cur, dat1, 5);
//                cur.ShowCustomer();
                if (!cur.hasScheduled(my_set)) {
                    CDPSolver::disjointClients[i].insert(val);
                    CDPSolver::disjointClients[val].insert(i);

                }
            }
        }
    }
}

void CDPSolver::PathRelinking(Sol &new_cur, std::vector<Customer *> &list_cust,
                              std::vector<BestSolutionList<Customer, Driver>> &vectSolutions,
                              Cost &bestCout, Sol &best) {
//    cout << "begin path relinking\n";

    shuffle(list_cust.begin(), list_cust.end(), Parameters::RANDOM_GEN);
//    Prompt::print(best.unscheduledCustomers);
    new_cur.heurName="Path relinking";
    for (Customer *c: list_cust) {

        if (CDPSolver::nbSatisfied[c->custID] == 0) continue;

//            cout<<*c<<endl;
        std::vector<Sol *> v;
        vectSolutions[c->custID].GetSolutions(v);
        std::vector<Order *> orders = new_cur.GetOrders(c);
        shuffle(v.begin(), v.end(), Parameters::RANDOM_GEN);
        int count = 0;
        bool isfeasible = true;
        for (auto cur: v) {
//            cout << count << " " << cur->toString(c) << endl;
//                exit(1);
//                cur->ShowSchedule(c);
            std::vector<Delivery *> deliveries;
            int cust_dmd=0;
            for (auto o1: orders) {
                if(cust_dmd >= c->demand )
                    break;
                int order_dmd=0;
                for (int i = 0; i < cur->GetDeliveryCount(o1); i++) {
                    if(order_dmd >= o1->demand )
                        break;
                    Delivery *del = cur->GetDelivery(o1, i);
                    if (cur->GetDriverAssignedTo(del) == nullptr)
                        break;
                    if (new_cur.GetDriverAssignedTo(del) != nullptr)
                        continue;

                    isfeasible = InsertDel(new_cur, cur, del);

                    if (not isfeasible)
                        break;

                    order_dmd+=new_cur.DeliveryLoad[del->delID];
                    cust_dmd+=new_cur.DeliveryLoad[del->delID];
                }
                if (not isfeasible)
                    break;
            }
            count++;
            if (not isfeasible){
                new_cur.UnassignCustomer(c);
                continue;
            }
            repairSolution(new_cur);
//                 printf("%d realisable\n", count - 1);
//                 cout << "Insert " << c->custID << " in new_cost\n";
            break;
        }
        if (not isfeasible) {

//          cout << "Can't insert " << c->custID << " in new_cost\n";
        }

    }
    repairSolution(new_cur);
}


bool CDPSolver::InsertDel(Sol &sol_cur, Sol *cur, Delivery *del) {
    Dock *dock = cur->GetDock(del->dockID);
    Depot *dep = cur->GetDepotAssignedTo(dock);
    Driver *d = cur->GetDriverAssignedTo(del);
    assert(d == cur->GetDriverAssignedTo(dock));
    // si chauffeur occupé, arrêter et supprimer les autres noeuds
    TimeSlot intv = cur->nodeServiceIntervals[dock->id];
    if (sol_cur.depotLoadingIntervals[dep->depotID].find(intv) !=
        sol_cur.depotLoadingIntervals[dep->depotID].end()) {
        // cout << intv << " depot occupied\n";
        // return false;
    }
    intv = cur->nodeServiceIntervals[del->id];
    auto *prec_del = dynamic_cast<Delivery *>( sol_cur.CustomerPrev[del->id]);
    if (prec_del != nullptr) {
        Dock *prec_dock = sol_cur.GetDock(prec_del->dockID);
        if (intv.lower < sol_cur.nodeServiceIntervals[prec_dock->id].lower)
            return false;
        if (sol_cur.nodeServiceIntervals[prec_del->id].lower > intv.lower)
            return false;
    }

    if (sol_cur.driverWorkingIntervals[d->id].find(intv) !=
        sol_cur.driverWorkingIntervals[d->id].end()) {
        return false;
    }
    auto it = sol_cur.driverWorkingIntervals[d->id].lower_bound(intv);

    if (it == sol_cur.driverWorkingIntervals[d->id].end() and
        not sol_cur.driverWorkingIntervals[d->id].empty()) {
        it--;
    }

    if (it != sol_cur.driverWorkingIntervals[d->id].end()) {
        auto *del_it = dynamic_cast<Delivery *>( sol_cur.GetNode(it->nodeID));
        Dock *dock_it = sol_cur.GetDock(del_it->dockID);
        if (*it < intv) {
            // finir it->upper avant de commencer au dock à intv.lower
            if (it->upper + sol_cur.Travel(del_it, dock) > intv.lower) {
                return false;
            }
            auto it1 = std::next(it);
            if (it1 != sol_cur.driverWorkingIntervals[d->id].end()) {
                auto *del_it1 = dynamic_cast<Delivery *>( sol_cur.GetNode(it1->nodeID));
                Dock *dock_it2 = sol_cur.GetDock(del_it1->dockID);
                if (intv.upper + sol_cur.Travel(del, dock_it2) > it1->lower) {
                    return false;
                }
            }
        } else if (*it > intv) {
            if (intv.upper + sol_cur.Travel(del, dock_it) > it->lower) {
                return false;
            }
            if (it != sol_cur.driverWorkingIntervals[d->id].begin()) {
                auto it2 = std::prev(it);
                if (it2 != it) {
                    if (it2 != sol_cur.driverWorkingIntervals[d->id].end()) {
                        auto *temp_del = dynamic_cast<Delivery *>( sol_cur.GetNode(it2->nodeID));
                        if (it2->upper + sol_cur.Travel(temp_del, dock) > intv.lower) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    sol_cur.DriverAssignTo[dock->id] = d;
    sol_cur.DeliveryLoad[del->delID] = cur->DeliveryLoad[del->delID];
    sol_cur.InsertAfter(del, dock, d);
    sol_cur.AssignDeliveryToCustomer(del);

    sol_cur.UpdateDepotLoadingSet(dep, dock, cur->nodeServiceIntervals[dock->id]);
//    sol_cur.ShowSlot(dep);
    sol_cur.driverWorkingIntervals[d->id].insert(
            cur->nodeServiceIntervals[del->id]);
    sol_cur.ArrivalTime[dock->id] = cur->ArrivalTime[dock->id];
    sol_cur.ArrivalTime[del->id] = cur->ArrivalTime[del->id];

    return true;
}


void CDPSolver::repairSolution(Sol &new_cur) {
//    new_cur.ShowDepotSlots();
//    new_cur.ShowDriverSlots();
    new_cur.BuildFromDepotSetIntervall();
    new_cur.BuildFromDriverSetIntervall();
//    new_cur.ShowDrivers();
    new_cur.Update();
}

void CDPSolver::repairSchedule(Sol &sol) {
//TODO    if (_insrmv != nullptr)
//    {
//        _insrmv->repairSolution(sol);
//    }
}






