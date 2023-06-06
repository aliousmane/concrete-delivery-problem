
#include "Solver.h"
#include "CDPSolver.h"
#include <istream>
#include "CustInsertion.h"
#include "CustInsertion2.h"
#include "InsRmvMethodFast.h"
#include "InsRmvBuilder1.h"
#include "InsRmvBuilder2.h"
#include "InsRmvBuilder3.h"

using namespace std;

void Solver::run() {
    feasibleClients = CDPSolver::EliminateCustomer(*_data, 1);
    vector<TimeSlot> listInt;
    for (int i: feasibleClients) {
        Customer *c = _data->GetCustomer(i);
        listInt.emplace_back(c->early_tw, c->late_tw, c->custID);
    }
    vector<set<int>> linkedClientSlot;
    vector<set<int>> linkedClientDemand;
    vector<set<int>> linkedClients;
    vector<set<int>> linkedClientsInf;
    vector<set<int>> linkedClientSup;
    Data dat = *_data;
    CDPSolver::findCorrelation(dat, listInt, linkedClientSlot, linkedClientDemand, linkedClients, linkedClientsInf,
                               linkedClientSup);
    {
//        Sol s(&dat);
//        s.keyCustomers = feasibleClients;
//        s.keyCustomers = { 0,1,5,9,11,13,14,16,19,};
//        Parameters::SORT_TYPE=Parameters::SORT::ONE;
//        CDPSolver::SolveInstance(s, dat, 10);
//        s.ShowSchedule();
//        s.ShowCustomer();
//
//        exit(1);
//        s.keyCustomers = {7};

//        CDPSolver::BuildOnSolution(s, dat, 10);

//        s.ShowCustomer();
//        exit(1);
    }
//    Test0();exit(1);
    Sol s(&dat);
    s.keyCustomers = feasibleClients;
//    s.keyCustomers= {2,3,7,10,14};
//    Parameters::SORT_TYPE=Parameters::SORT::FIVE;
//    CDPSolver::SolveInstance(s,dat,10);
//    exit(1);
    SolveGrasp(s, dat, linkedClientSlot, 10);
    SaveResults(s);
//    s.ShowSchedule();
//    s.ShowCustomer();
}
void Solver::SolveGrasp(Sol &s, Data &dat, vector<set<int>> const &linkedClientSlot, int iter) {

    CDPSolver::nbSatisfied.resize(dat.GetCustomerCount());

    InsRmvBuilder3 builder3(dat);
    CustInsertion custIns3(dat, builder3);
    CustInsertion2 custIns23(dat, builder3);
    PriorityQueueInsertion prioIns3(dat, builder3);
    DriverInsertion driverIns3(dat, builder3);

    vector<AllInsertionOperator> grasp_heuristics;

    GRASP<Customer, Driver> grasp(&dat);
    vector<pair<int, string>> custInfo = {
            {0, "Cust Early TW"},
            {1, "Cust Greater D"},
            {2, "Cust Late TW "},
            {3, "Cust Min Width TW"},
            {4, "Cust Random"},
            {5, "Cust Kinable"},
    };
    vector<pair<int, string>> priorityInfo = {
            {0, "PrioriSort I Early TW"},
            {1, "PrioriSort I Late TW"},
            {2, "PrioriSort D Early TW"},
            {3, "PrioriSort D Late TW"},
            {4, "PrioriSort D Demand"},
            {5, "PrioriSort I Demand"},
//            {6,"PrioriSort I TW width"},
//            {7,"PrioriSort D TW width"},
    };

    vector<pair<int, string>> driverInfo = {
            {0, "Driver I Cap"},
            {1, "Driver D Cap"},
            {2, "Driver Random Cap "},
    };

    AllInsertionOperator c0(&custIns3, custInfo[0].first, "Builder 3 " + custInfo[0].second);
    AllInsertionOperator c1(&custIns3, custInfo[1].first, "Builder 3 " + custInfo[1].second);
    AllInsertionOperator c2(&custIns3, custInfo[2].first, "Builder 3 " + custInfo[2].second);
    AllInsertionOperator c3(&custIns3, custInfo[3].first, "Builder 3 " + custInfo[3].second);
    AllInsertionOperator c4(&custIns3, custInfo[4].first, "Builder 3 " + custInfo[4].second);
    AllInsertionOperator c5(&custIns3, custInfo[5].first, "Builder 3 " + custInfo[5].second);

    AllInsertionOperator p0(&prioIns3, priorityInfo[0].first, "Builder 3 " + priorityInfo[0].second);
    AllInsertionOperator p1(&prioIns3, priorityInfo[1].first, "Builder 3 " + priorityInfo[1].second);
    AllInsertionOperator p2(&prioIns3, priorityInfo[2].first, "Builder 3 " + priorityInfo[2].second);
    AllInsertionOperator p3(&prioIns3, priorityInfo[3].first, "Builder 3 " + priorityInfo[3].second);
    AllInsertionOperator p4(&prioIns3, priorityInfo[4].first, "Builder 3 " + priorityInfo[4].second);
    AllInsertionOperator p5(&prioIns3, priorityInfo[5].first, "Builder 3 " + priorityInfo[5].second);

    AllInsertionOperator d0(&driverIns3, driverInfo[0].first, "Builder 3 " + driverInfo[0].second);
    AllInsertionOperator d1(&driverIns3, driverInfo[1].first, "Builder 3 " + driverInfo[1].second);
    AllInsertionOperator d2(&driverIns3, driverInfo[2].first, "Builder 3 " + driverInfo[2].second);


    grasp.AddInsertOperatorVrp(&c0);
    grasp.AddInsertOperatorVrp(&c1);
    grasp.AddInsertOperatorVrp(&c2);
//    grasp.AddInsertOperatorVrp(&c3);
    grasp.AddInsertOperatorVrp(&c4);
    grasp.AddInsertOperatorVrp(&c5);
//
//
//    grasp.AddInsertOperatorVrp(&p0);
//    grasp.AddInsertOperatorVrp(&p1);
//    grasp.AddInsertOperatorVrp(&p2);
//    grasp.AddInsertOperatorVrp(&p3);
//    grasp.AddInsertOperatorVrp(&p4);
//    grasp.AddInsertOperatorVrp(&p5);

//    grasp.AddInsertOperatorVrp(&d0);
//    grasp.AddInsertOperatorVrp(&d1);
//    grasp.AddInsertOperatorVrp(&d2);

    grasp.verbose = true;
    grasp.SetIterationCount(iter);
    RechercheLocale loc_search(s.keyCustomers);
    loc_search.LinkedClientSlot = linkedClientSlot;
    grasp.Optimize(s, nullptr, nullptr, &loc_search, true);
//    grasp.Optimize(s, nullptr, nullptr, nullptr, true);
}

void Solver::SaveResults(Sol &s) {
    printf("Instance:%s\n", _data->instance_name.c_str());

    printf("Nombre clients %d \t", _data->nbCustomers);
    printf("Nombre ordre %d \t", _data->nbOrders);
    printf("Total Cost:%.2lf \t", s.GetLastCost().satisfiedCost);

    if (_data->result_file.c_str() != nullptr) {
        FILE *f = fopen(_data->result_file.c_str(), "a");
        if (f != nullptr) {
            time_t now = time(nullptr);
            tm *ltm = localtime(&now);
            fprintf(f, "%d-", 1900 + ltm->tm_year);
            fprintf(f, "%d-", 1 + ltm->tm_mon);
            fprintf(f, "%d;", ltm->tm_mday);
            fprintf(f, "%d:%d:%d;", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);
            fprintf(f, "%s;", _data->problem_name.c_str());
            fprintf(f, "%s;", _data->instance_name.c_str());
            fprintf(f, "%2.0lf;", s.GetLastCost().satisfiedCost);
            fprintf(f, "%2.2lf;", s.GetLastCost().travelCost);
            fprintf(f, "%d;", s.GetLastCost().driverUsed);
            fprintf(f, "%ld\n", Parameters::GetElapsedTime());
            fclose(f);
        }
    }
}

void Solver::Test0() {

    Parameters::SORT_TYPE = Parameters::SORT::THREE;
    feasibleClients = CDPSolver::EliminateCustomer(*_data, 1);

    InsRmvMethodFast insrmv(*_data);
    InsRmvBuilder3 builder3(*_data);
    CustInsertion custIns(*_data, builder3);
    CustInsertion2 custIns23(*_data, builder3);
    {
        Parameters::DRIVER_USE = Parameters::MINIMIZEDRIVER::CLIENT;

        Sol s(_data);
        for (auto id: feasibleClients) {
            Customer *c = _data->GetCustomer(id);
            std::vector<Customer *> list{c};
            s.keyCustomers = {c->custID};
            custIns23.Insert(s);
            if (s.isSatisfied(c)) {
                s.ShowCustomer();
                s.keyCustomers.clear();
            } else {
                continue;
                int iter = 0;
                while (iter++ < 5) {
                    Sol cur = s;
                    for (auto conflict_id: Sol::CustomerConflict[c->custID]) {
                        cur.UnassignCustomer(conflict_id);
                        cur.keyCustomers.insert(conflict_id);
                    }
                    Parameters::SHOW = false;
                    CDPSolver::BuildOnSolution(cur, *_data, 1);
                    s.ShowCustomer();
                    if (cur.GetLastCost().satisfiedCost < s.GetLastCost().satisfiedCost) {
                        s = cur;
                    }
                    if (s.hasScheduled(s.keyCustomers)) break;
                }
                if (s.isSatisfied(c)) {
                    s.ShowCustomer();
                }
                Parameters::SHOW = false;
            }

        }
    }
    exit(1);
    Sol s1(_data);


    {
//        Résouds B_8_20_4
//        Parameters::SORT_TYPE = Parameters::SORT::THREE;
//        s1.keyCustomers={1,2,10,11,17,18,19,12};
//        CDPSolver::SolveInstance(s1, *_data, 100);
//        s1.ShowCustomer();
//        s1.keyCustomers.clear();
//        CDPSolver::BuildOnSolution(s1, *_data, 100);
//        //   s1.exportCSVFormat(_data->sol_output);
//        s1.exportCSVFormat("s0.csv");
//        //    s1.ShowDrivers();
//        s1.ShowCustomer();
//        //    Test(s1);
//        exit(1);
//            run2(s1,feasibleClients);
    }
    {
//        Parameters::SORT_TYPE = Parameters::SORT::ONE;
//        Parameters::SORT_TYPE = Parameters::SORT::TWO;
//        Parameters::SORT_TYPE = Parameters::SORT::THREE;
        Parameters::SORT_TYPE = Parameters::SORT::FOUR;
//        Parameters::PENALTY_COST = true;
        Parameters::DRIVER_USE = Parameters::MINIMIZEDRIVER::HYBRID;
//        Parameters::DRIVER_USE=Parameters::MINIMIZEDRIVER::SOLUTION;
        s1.keyCustomers = feasibleClients;
//        s1.keyCustomers={1};
//s1.availableDrivers={0,5,6,7,1};
        CDPSolver::SolveInstance(s1, *_data, 1);
//        s1.keyCustomers.clear();
//        CDPSolver::BuildOnSolution(s1, *_data, 1);

        //   s1.exportCSVFormat(_data->sol_output);
        s1.exportCSVFormat("s0.csv");
//        s1.ShowDrivers();
        s1.ShowCustomer();
        cout << s1.GetLastCost() << endl;
//        s1.ShowSchedule();
    }
    Parameters::ShowTime();
    exit(1);
    vector<TimeSlot> listInt;
    for (int i: feasibleClients) {
        Customer *c = _data->GetCustomer(i);
        listInt.emplace_back(c->early_tw, c->late_tw, c->custID);
    }
    Data dat = *_data;
    vector<set<int>> linkedClientSlot;
    vector<set<int>> linkedClientDemand;
    vector<set<int>> linkedClients;
    vector<set<int>> linkedClientsInf;
    vector<set<int>> linkedClientSup;
    CDPSolver::findCorrelation(dat, listInt, linkedClientSlot, linkedClientDemand, linkedClients, linkedClientsInf,
                               linkedClientSup);

    bool sortie = true;
    auto it = s1.unscheduledCustomers.begin();

    {
        Customer *c = _data->GetCustomer(*it);
        cout << *c << endl;
        Prompt::print(linkedClientsInf[c->constID]);
        Prompt::print(linkedClientSup[c->constID]);
        Prompt::print(linkedClientSlot[c->constID]);
        for (int k = 0; k < _data->GetDriverCount(); k++) {
            Driver *d = _data->GetDriver(k);
            cout << *d << endl;
            s1.ShowSlot(d);
            for (auto slot: s1.driverWorkingIntervals[d->id]) {
                auto del = dynamic_cast<Delivery *> (s1.GetNode(slot.nodeID));
                auto c_slot = _data->GetCustomer(del->custID);
                auto d_slot = _data->GetDepot(del->depotID);
                bool solve = false;
                if (TimeSlot(c->early_tw, c->late_tw) == slot) {
                    cout << slot << " == " << del->custID << endl;
                    solve = true;
                } else if (TimeSlot(slot.upper + s1.Travel(c_slot, c)) == TimeSlot(c->early_tw)) {
//                        cout<<slot<< " <= "<<del->custID<< endl;
                    solve = true;
                } else if (slot.lower - s1.Travel(c->distID, d_slot->distID) >= 0)
                    if (TimeSlot(slot.lower - s1.Travel(c->distID, d_slot->distID)) ==
                        TimeSlot(c->early_tw, c->late_tw)) {
                        solve = true;
                        //cout<<slot<< " >= "<<del->custID<< " FS "<<slot.lower - s1.Travel(c->distID,d_slot->distID)<<endl;
                    }

            }

        }
        for (auto id: linkedClientSlot[c->constID]) {
            Customer *c1 = _data->GetCustomer(id);

            auto *first = dynamic_cast<Delivery *>(s1.CustomerNext[c1->StartNodeID]);
            int early = c1->early_tw;
            bool sortie = false;
            while (!sortie) {
                auto *last = dynamic_cast<Delivery *>(s1.CustomerPrev[c1->EndNodeID]);
                if (s1.EndServiceTime[last->id] == s1.LateTW(last))
                    break;
                c1->Show();
                s1.ShowSchedule(first);
                s1.ShowSchedule(last);
                int gap = s1.LateTW(last) - s1.EndServiceTime[last->id];
                c1->early_tw = s1.StartServiceTime[first->id] + gap;
                Sol cur2 = s1;
                cur2.UnassignCustomer(c1);
                CDPSolver::BuildOnSolution(cur2, *_data, 1);
                if (cur2.isSatisfied(c1)) {
                    s1 = cur2;
                    cur2.ShowSchedule(c1);
                    CDPSolver::BuildOnSolution(s1, *_data, 1);
                    if (s1.isSatisfied(c)) {
                        s1.ShowSchedule(c);
                    }
                    sortie = false;
                } else {
                    sortie = true;
                }

            }
            c1->early_tw = early;
        }

        exit(1);
    }
    Customer *c0 = _data->GetCustomer(*it);
    set<int> solvedfirst = linkedClientSlot[c0->constID];
    solvedfirst.insert(c0->custID);
    for (int iter = 0; iter < 20; iter++) {
        cout << *c0 << endl;
        Sol cur2(_data);
        cur2.keyCustomers = solvedfirst;
//        cur2.keyCustomers.insert(c0->custID);
        CDPSolver::SolveInstance(cur2, *_data, 100);
        cur2.ShowCustomer();
        cur2.keyCustomers.clear();
        CDPSolver::BuildOnSolution(cur2, *_data, 100);
        cur2.ShowCustomer();
        if (!cur2.unscheduledCustomers.empty()) {
            solvedfirst.insert(cur2.unscheduledCustomers.begin(), cur2.unscheduledCustomers.end());
            c0 = _data->GetCustomer(*cur2.unscheduledCustomers.begin());
        } else {
            break;
        }
    }
    exit(1);

    for (auto custId: s1.unscheduledCustomers) {
        Customer *c = _data->GetCustomer(custId);
        cout << *c << endl;
        Prompt::print(linkedClientsInf[c->constID]);
        Prompt::print(linkedClientSup[c->constID]);
        Prompt::print(linkedClientSlot[c->constID]);
        Sol cur2(_data);
        cur2.keyCustomers = linkedClientSlot[c->constID];
        cur2.keyCustomers.insert(c->id);
        CDPSolver::SolveInstance(cur2, *_data, 10);
        cur2.ShowCustomer();
        cur2.keyCustomers.clear();
        CDPSolver::BuildOnSolution(cur2, *_data, 100);
        cur2.ShowCustomer();
        exit(1);
        set<int> driverUsed;
        set<int> Caps;
        int sum_demand = 0;
        {
//            Test(s1,c,linkedClientsInf);
//            Test(s1,c,linkedClientSup);
            Test(s1, c, linkedClientSlot);

            bool sortie = true;
            while (sortie) {
                sortie = false;
                bool find_new = false;
                for (int k = 0; k < _data->GetDriverCount(); k++) {
                    Driver *d = _data->GetDriver(k);
                    cout << *d << endl;
                    s1.ShowSlot(d);
                    for (const auto &slot: s1.driverWorkingIntervals[d->id]) {
                        auto del = dynamic_cast<Delivery *> (s1.GetNode(slot.nodeID));
                        auto c_slot = _data->GetCustomer(del->custID);
                        auto d_slot = _data->GetDepot(del->depotID);
                        bool solve = false;
                        if (TimeSlot(c->early_tw, c->late_tw) == slot) {
                            cout << slot << " == " << del->custID << endl;
                            solve = true;
                        } else if (TimeSlot(slot.upper + s1.Travel(c_slot, c)) == TimeSlot(c->early_tw)) {
//                        cout<<slot<< " <= "<<del->custID<< endl;
                            solve = true;
                        } else if (slot.lower - s1.Travel(c->distID, d_slot->distID) >= 0)
                            if (TimeSlot(slot.lower - s1.Travel(c->distID, d_slot->distID)) ==
                                TimeSlot(c->early_tw, c->late_tw)) {
                                solve = true;
                                //cout<<slot<< " >= "<<del->custID<< " FS "<<slot.lower - s1.Travel(c->distID,d_slot->distID)<<endl;
                            }
                        if (solve) {
//                            find_new = TabuDriverNode(s1, c_slot, del, s1.GetDriverAssignedTo(del));
                            find_new = TabuDriverSolution(s1, c_slot, s1.GetDriverAssignedTo(del));
                        }
                        if (find_new) {
//                            Test(s1,c,linkedClientsInf);
//                            Test(s1,c,linkedClientSup);
                            break;
                        }
                    }
                    if (find_new) {
                        break;
                    } else {
                        sortie = true;
                    }

                }
            }
            for (int k = 0; k < _data->GetDriverCount(); k++) {
                Driver *d = _data->GetDriver(k);
                cout << *d << endl;
                s1.ShowSlot(d);
            }
            exit(1);
            Parameters::SORT_TYPE = Parameters::SORT::TWO;
            Test(s1, c, linkedClientsInf);
            Test(s1, c, linkedClientSup);
            s1.exportCSVFormat("s2.csv");
            for (int k = 0; k < _data->GetDriverCount(); k++) {
                Driver *d = _data->GetDriver(k);
                for (auto slot: s1.driverWorkingIntervals[d->id]) {
                    auto del = dynamic_cast<Delivery *> (s1.GetNode(slot.nodeID));
                    auto c_slot = _data->GetCustomer(del->custID);
                    if (TimeSlot(c->early_tw) == slot) {
                        cout << slot << " == " << del->custID << endl;
                    }
                    if (TimeSlot(slot.upper + s1.Travel(c_slot, c)) == TimeSlot(c->early_tw)) {
                        cout << slot << " <= " << del->custID << endl;
                    }
                }
            }
            exit(1);
        }
        Sol cur = s1;
        for (auto id: linkedClientsInf[c->constID]) {
            Customer *c1 = _data->GetCustomer(id);
            driverUsed.insert(s1.clientDriverUsed[c1->custID].begin(), s1.clientDriverUsed[c1->custID].end());
            sum_demand += c1->demand;
            cur.UnassignCustomer(c1);
        }
        for (auto id: linkedClientSlot[c->constID]) {
            Customer *c1 = _data->GetCustomer(id);
            Delivery *first = dynamic_cast<Delivery *>(s1.CustomerNext[c1->StartNodeID]);
            Delivery *last = dynamic_cast<Delivery *>(s1.CustomerPrev[c1->EndNodeID]);
            if (s1.EndServiceTime[last->id] == s1.LateTW(last))
                continue;
            c1->Show();
            s1.ShowSchedule(first);
            s1.ShowSchedule(last);
            int gap = s1.LateTW(last) - s1.EndServiceTime[last->id];
            int early = c1->early_tw;
            c1->early_tw = s1.StartServiceTime[first->id] + gap;
            Sol cur2 = s1;
            cur2.UnassignCustomer(c1);
            CDPSolver::BuildOnSolution(cur2, *_data, 1);
            if (cur2.isSatisfied(c1)) {
//                s1=cur2;
                cur2.ShowSchedule(c1);
                CDPSolver::BuildOnSolution(s1, *_data, 1);
                if (s1.isSatisfied(c)) {
                    s1.ShowSchedule(c);
                }
            }
            c1->early_tw = early;
        }
        CDPSolver::BuildOnSolution(s1, *_data, 1);
        if (s1.isSatisfied(c)) {
            s1.ShowSchedule(c);
        }
        exit(1);
        vector<int> vec;
        for (auto id: driverUsed) {
            vec.push_back(_data->GetDriver(id)->capacity);
        }
        set<int> driversId = _data->GetDriversId();
        for (auto id: driverUsed) {
            driversId.erase(id);
        }
        s1.exportCSVFormat("s1.csv");
        Prompt::print(driverUsed);
        cur.availableDrivers = driversId;
        Prompt::print(cur.availableDrivers);
        cur.keyCustomers = {c->custID};
        Parameters::SORT_TYPE = Parameters::SORT::ONE;
        Parameters::SHOW = true;

        CDPSolver::BuildOnSolution(cur, *_data, 1);
        cur.ShowCustomer();
        cur.availableDrivers = driverUsed;
        cur.keyCustomers = {c->custID};
        Parameters::SORT_TYPE = Parameters::SORT::ONE;
        Prompt::print(cur.availableDrivers);

        CDPSolver::BuildOnSolution(cur, *_data, 1);
        cur.ShowCustomer();


        cur.exportCSVFormat("cur.csv");
//        auto array = Combinatorial::findCombinationsWithLimit(vec,sum_demand);
        driverUsed.clear();
        for (auto id: linkedClientsInf[c->constID]) {
            Customer *c1 = _data->GetCustomer(id);
            driverUsed.insert(cur.clientDriverUsed[c1->custID].begin(), cur.clientDriverUsed[c1->custID].end());
        }
        Prompt::print(driverUsed);
        exit(1);

        for (auto id: linkedClientsInf[c->constID]) {
            Customer *c1 = _data->GetCustomer(id);
            if (!s1.isClientSatisfied(c1)) continue;
            // try to insert c after c1
            // Regarder si je peux placer le premier node de c après un des nodes de c1
            int nbMin = ceil(double(c1->demand) / s1.GetData()->maxDriverCap);
            if (s1.OrderVisitCount[c1->custID] == nbMin) {
                // schedule *optimal*
                continue;
            }
            // essayer de reduire le nombre de chauffeurs utilisé pour c1

            InsRmvBuilder3 builder3(*s1.GetData());

            for (auto deli: s1.GetDeliveries(c1)) {
                cout << *deli << endl;
                int start = s1.StartServiceTime[deli->id];
                cout << "Debut " << start << endl;
                // essayer d'utiliser un chauffeur de plus grande capacité
                Driver *d = s1.GetDriverAssignedTo(deli);
                if (d == nullptr) break;
                Move<Delivery, Driver, MoveVrp> best;

                for (int k = 0; k < s1.GetDriverCount(); k++) {
                    Driver *d_k = s1.GetDriver(k);
                    if (d_k->capacity <= d->capacity)
                        continue;

                    for (auto val: s1.driverWorkingIntervals[d_k->id]) {

                        if (TimeSlot(start) == val) {
                            cout << val << "--";
                            Delivery *val_del = dynamic_cast<Delivery *>(s1.GetNode(val.n.id));
                            cout << *val_del << endl;
                            Customer *c2 = s1.GetCustomer(val_del->custID);
                            cout << *c2 << endl;
                            s1.ShowSchedule(c1);
                            s1.ShowSchedule(c2);

                            auto c1_drivers = s1.clientDriverUsed[c1->custID];
                            auto c2_drivers = s1.clientDriverUsed[c2->custID];
//                          Prompt::print(c2_drivers);
//                            c1_drivers= s1.driverUsed;
                            c1_drivers.insert(d_k->id);
                            c1_drivers.erase(d->id);
//                            c2_drivers=s1.driverUsed;
                            c2_drivers.insert(d->id);
                            c2_drivers.erase(d_k->id);
                            Sol cur = s1;

                            cur.UnassignCustomer(c1);
                            cur.UnassignCustomer(c2);
                            cur.keyCustomers = {c1->custID};
                            cur.availableDrivers = c1_drivers;
//                            Prompt::print(c1_drivers);
                            CDPSolver::BuildOnSolution(cur, *cur.GetData(), 1);
//                            cur.ShowSchedule(c1);

                            cur.keyCustomers = {c2->custID};
                            cur.availableDrivers = c2_drivers;
//                            Prompt::print(c2_drivers);
                            CDPSolver::BuildOnSolution(cur, *cur.GetData(), 1);
//                            cur.ShowSchedule(c2);
                            cur.ShowCustomer();
                        }
                    }
                    cout << endl;
                    continue;
                    s1.ShowSlot(d_k);

                    Move<Delivery, Driver, MoveVrp> m;
                    Sol cur = s1;
                    cur.UnassignCustomer(c1);
                    builder3.GetBestInsertion(cur, {deli->delID}, {d_k}, m);
                    if (m.IsFeasible) {
                        best = m;
                        break;
                    } else {
                        // swap both drivers.
                    }
                }
                if (best.IsFeasible) {
                    exit(1);
                }
                exit(1);
            }
        }
    }
    exit(1);


    Prompt::print(linkedClientSlot[2]);
    Prompt::print(linkedClientsInf[2]);
    Sol::minDelay[118] = 19;
    Sol::minDelay[126] = 25;
    Sol::minDelay[122] = 15;
    Sol::minDelay[131] = 30;
//    s1.UnassignCustomer(s1.GetCustomer(13));
//    s1.UnassignCustomer(s1.GetCustomer(15));
//    s1.UnassignCustomer(s1.GetCustomer(14));
//    s1.UnassignCustomer(s1.GetCustomer(16));
    // dist de 12 à 1: 116-> 152,  152-89
//    CDPSolver::SolveInstance(s1,*_data,1);
//    s1.ShowSchedule();
    s1.ShowDrivers();
//    s1.ShowCustomer();

    exit(1);
    Prompt::print(feasibleClients);
    CDPSolver::findDisjointSet(*_data, linkedClients);
    Sol s(&dat);
    s.keyCustomers = feasibleClients;
//    s.keyCustomers={10,18,19};
    s.availableDrivers.clear();
    for (int k = 0; k < _data->GetDriverCount(); k++) {
        Driver *d = _data->GetDriver(k);
//        s.availableDrivers.insert(k);
//        s.availableDrivers={0,5,6,7};
        cout << "Nbre drivers " << s.availableDrivers.size() << endl;
        CDPSolver::SolveInstance(s, dat, 1);
//        s.ShowCustomer();
        if (s.hasScheduled(s.keyCustomers)) {
//            s.ShowCustomer();
//          s.ShowSchedule();
            break;
        }
//        exit(1);
        break;
    }
    s.ShowSchedule();
    s.ShowCustomer();
    exit(1);
    // TODO: Est-ce que je peux faire une recherche locale pour réduire le nombre de chauffeurs utilisés?
    // Pour chaque client visité, je verifie si on peut reduire le nombre de chauffeurs qui peut le visiter
    // Si c'est possible, je fais des relocate et move pour deplacer les noeuds dans les routes des chauffeurs.
    s.availableDrivers.clear();
    s.keyCustomers.clear();
    CDPSolver::BuildOnSolution(s, dat, 10);
    s.ShowCustomer();

    exit(1);
    s.ShowCustomer();
    cout << s.GetCost() << endl;
    exit(1);
    for (auto cap: _data->driverCapacities) {
        cout << "Driver of cap " << cap << endl;
        for (int k = 0; k < _data->GetDriverCount(); k++) {
            Driver *d = _data->GetDriver(k);
            if (d->capacity != cap) continue;
            s.availableDrivers = {k};
            CDPSolver::BuildOnSolution(s, dat, 20);
//            CDPSolver::SolveInstance(s,dat,20);
//          s.ShowSchedule();
            s.ShowCustomer();
//        exit(1);
            break;
        }
    }

//    s.ShowCustomer();
    exit(1);
    SolveGrasp(s, dat, linkedClientSlot, 1);
    s.ShowCustomer();
    CDPSolver::deleteOperator();

}

bool Solver::TabuDriverNode(Sol &s1, Customer *c, Delivery *del, Driver *d) {
    Sol::TabuFleet[del->id].clear();
    Sol::TabuFleet[del->id].insert(d->id);
    Sol cur2 = s1;
    cur2.UnassignCustomer(c);
    CDPSolver::BuildOnSolution(cur2, *_data, 1);
    Sol::TabuFleet[del->id].clear();
    bool feasible = cur2.isSatisfied(c);
    if (feasible) {
        cur2.ShowSchedule(c);
        cur2.ShowSlot(d);
        s1 = cur2;
    }
    return feasible;
}

bool Solver::TabuDriverSolution(Sol &s1, Customer *c, Driver *d) {
    Sol cur2 = s1;
    cur2.availableDrivers = _data->GetDriversId();
    cur2.availableDrivers.erase(d->id);
    cur2.UnassignCustomer(c);
    CDPSolver::BuildOnSolution(cur2, *_data, 1);
    bool feasible = cur2.isSatisfied(c);
    if (feasible) {
        cur2.ShowSchedule(c);
        cur2.ShowSlot(d);
        s1 = cur2;
    }
    return feasible;
}

void Solver::Test() {
    Sol s(_data);
    InsRmvMethodFast insrmv(*_data);
    InsRmvBuilder3 builder3(*_data);
    CustInsertion custIns(*_data, builder3);
    Sol::FailureCause.resize(_data->GetNodeCount(), Parameters::FAILURECAUSE::NONE);
    Sol::FailureCount.resize(_data->GetNodeCount(), 0);
    Sol::StartBefore.resize(_data->GetNodeCount(), 0);
    Sol::pullVisit.resize(_data->GetNodeCount(), 0);
    Sol::pushVisit.resize(_data->GetNodeCount(), 0);

    std::vector<int> all_drivers_cap;
    for (int k = 0; k < _data->GetDriverCount(); k++) {
        all_drivers_cap.push_back(_data->GetDriver(k)->capacity);
    }

    cout << "learn Parameters\n";
    for (int i = 0; i < _data->GetCustomerCount(); i++) {
        Customer *c = _data->GetCustomer(i);
        cout << *c << endl;
        std::vector<int> vec_cap(_data->driverCapacities.begin(), _data->driverCapacities.end());

        std::set < std::tuple<int, std::vector<int>> > combinations =
                Combinatorial::findCombinationsWithLimit(vec_cap, all_drivers_cap,
                                                         c->demand, _data->Travel(c, c));
        set<int> sumService;
        for (auto [val, vec]: combinations) {
            sumService.insert(val);
//            Prompt::print(val);
        }
//        Prompt::print(sumService);
        const int early = c->early_tw;
        for (auto sumService: sumService) {
            c->early_tw = c->late_tw - sumService;
            CDPSolver::LearnParameters(s, {c->custID});
        }
        c->early_tw = early;
    }
    for (auto [key, temps]: Sol::nodeMaxStartService) {
        int delId = std::get<0>(key);
        cout << delId << "-" << std::get<1>(key) << ":" << temps << "|";
    }
}

void Solver::Test(Sol &s) {
    vector<Customer *> listCust = _data->GetCustomers();

    SortNode<Customer, Driver>::radixSortEarlyTW(listCust, s.GetData()->maxEarlyTW);

    for (auto c: listCust) {
        if (s.isSatisfied(c)) {
            int nbMin = ceil(double(c->demand) / s.GetData()->maxDriverCap);
            if (s.OrderVisitCount[c->custID] == nbMin) {
                continue;
            }
            Delivery *del = s.GetDeliveries(c)[0];
            Driver *d = s.GetDriverAssignedTo(del);
            s.ShowSchedule(c);
            for (int k = 0; k < s.GetDriverCount(); k++) {
                Driver *dk = s.GetDriver(k);
                if (dk->capacity <= d->capacity)
                    continue;
//                s.ShowSlot(dk);
                set<int> driverUsed = s.clientDriverUsed[c->custID];
                driverUsed.erase(d->id);
                driverUsed.insert(dk->id);
                Sol cur = s;
                cur.UnassignCustomer(c);
                cur.availableDrivers = driverUsed;
                cur.keyCustomers = {c->custID};
                CDPSolver::BuildOnSolution(cur, *s.GetData(), 1);
                if (cur.isSatisfied(c)) {
//                    s = cur;
                    cur.ShowSchedule(c);
//                    break;
                }
            }
        } else {
            cout << c->custID << " is not scheduled\n";
        }
    }
    s.ShowCustomer();
    exit(1);
}

void Solver::TestRecursive(Sol &s, Delivery *del, int cap) {
    // trouver chauffeur de capacité cap
    vector<Driver *> listDrivers;
    for (int k = 0; k < _data->GetDriverCount(); k++) {
        if (_data->GetDriver(k)->capacity == cap) {
            listDrivers.push_back(_data->GetDriver(k));
        }
    }
    cout << *del << endl;
    Order *cur_order = _data->GetOrder(del->orderID);
    InsRmvBuilder3 builder3(*_data);
    Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
    for (auto d: listDrivers) {
        cout << " try to insert with " << d->id << endl;
        Sol cur = s;
        Driver *used_d = cur.GetDriverAssignedTo(del);
        if (used_d != nullptr) {
            if (used_d->capacity == d->capacity)
                break;
        }
        Move<Delivery, Driver, MoveVrp> m;
        builder3.GetBestInsertion(cur, {del->delID}, {d}, m);
        if (m.IsFeasible) {
            cout << "Insert " << del->id << " with " << d->id << endl;
            builder3.ApplyInsertMove(cur, m);
            cur.Update(m.move.depot, m.move.dock, m.n);
            if (cur.isSatisfied(cur_order)) {
                s = cur;
                break;
            } else {
                Delivery *next = cur.GetNextIdleDelivery(cur_order);
                if (next == nullptr) {
                    continue;
                } else {
                    TestRecursive(cur, next, cap);
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
                    int cur_cap = cur.GetDriverAssignedTo(prec_del)->capacity;
                    cur.UnassignDelivery({prec_del});
                    prec_del->isdelayed = true;
                    TestRecursive(cur, prec_del, cur_cap);
                }
            }
            continue;
        }
    }
}

void Solver::TestRecursive2(Sol &s, Delivery *del, std::unordered_map<std::string, Sol, MyHashFunction> *umap) {
//    cout << *del << endl;
    Order *cur_order = _data->GetOrder(del->orderID);
    InsRmvBuilder3 builder3(*_data);
    for (auto cap2: _data->driverCapacities) {

        for (int k = 0; k < _data->GetDriverCount(); k++) {
            if (_data->GetDriver(k)->capacity != cap2) {
                continue;
            }
            Driver *d = _data->GetDriver(k);
//            cout << " try to insert with " << d->id << endl;
            Sol cur = s;
            Driver *used_d = cur.GetDriverAssignedTo(del);
            if (used_d != nullptr) {
                if (used_d->capacity == d->capacity)
                    break;
            }
            Move<Delivery, Driver, MoveVrp> m;
            builder3.GetBestInsertion(cur, {del->delID}, {d}, m);
            if (m.IsFeasible) {
//                cout << "Insert " << del->id << " with " << d->id << endl;
                builder3.ApplyInsertMove(cur, m);
                cur.Update(m.move.depot, m.move.dock, m.n);
                if (cur.isSatisfied(cur_order)) {
                    if (umap != nullptr) {
//                        cur.Update();
//                        cout<<cur.toString()<<endl;
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
                        TestRecursive2(cur, next, umap);
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
                        TestRecursive2(cur, prec_del, umap);
                    }
                }
                continue;
            }
        }
    }
}

void Solver::Test(Sol &s1, Customer *c, std::vector<std::set<int>> const &linkedClient) {

    auto it_first = linkedClient[c->constID].begin();
    bool sortie = true;
    while (sortie) {
        Customer *c1 = _data->GetCustomer(*it_first);
        if (!s1.isSatisfied(c1)) {
            auto it = std::next(it_first);
            if (it == linkedClient[c->constID].end()) {
                break;
            }
            it_first = it;
            continue;
        }
        Delivery *first = dynamic_cast<Delivery *>(s1.CustomerNext[c1->StartNodeID]);
        Delivery *last = dynamic_cast<Delivery *>(s1.CustomerPrev[c1->EndNodeID]);
        if (s1.EndServiceTime[last->id] == s1.LateTW(last)) {
            auto it = std::next(it_first);
            if (it == linkedClient[c->constID].end()) {
                break;
            }
            it_first = it;
            continue;
        }
//        c1->Show();
//        s1.ShowSchedule(first);
//        s1.ShowSchedule(last);

        int gap1 = s1.StartServiceTime[first->id] - s1.EarlyTW(first);
        int gap2 = s1.LateTW(last) - s1.EndServiceTime[last->id];

        pair<int, int> Tw = make_pair(c1->early_tw, c1->late_tw);

        bool solve = true;
        if (c1->late_tw <= c->early_tw) {
            if (gap1 == 0) {
                solve = false;
            } else
                c1->late_tw = c1->late_tw - gap1;
        } else if (c1->early_tw >= c->late_tw) {
            if (gap2 == 0) {
                solve = false;
            } else
                c1->early_tw = s1.StartServiceTime[first->id] + gap2;
        } else if (TimeSlot(c1->early_tw, c1->late_tw) == TimeSlot(c->early_tw, c->late_tw)) {
            solve = false;
            if (c->late_tw < c1->late_tw) {
                if (gap2 != 0) {
                    c1->early_tw = s1.StartServiceTime[first->id] + gap2;
                    solve = true;
                }
            }
            if (c1->early_tw < c->early_tw) {
                if (!solve && gap1 == 0)
                    solve = false;
                else {
                    c1->late_tw = c1->late_tw - gap1;
                    solve = true;
                }
            }
        }
        if (solve) {
            Sol cur2 = s1;
            cur2.UnassignCustomer(c1);
            CDPSolver::BuildOnSolution(cur2, *_data, 1);
            c1->early_tw = Tw.first;
            c1->late_tw = Tw.second;
            if (cur2.isSatisfied(c1)) {
                s1 = cur2;
                cur2.ShowSchedule(c1);
                sortie = true;
            }
        }
        auto it = std::next(it_first);
        if (it == linkedClient[c->constID].end()) {
            sortie = false;
        }
        it_first = it;
    }
    CDPSolver::BuildOnSolution(s1, *_data, 1);
    if (s1.isSatisfied(c)) {
        s1.ShowSchedule(c);
        exit(1);
    }
    s1.Update();
}

/**
 * Essayer de trouver le même schedule avec un nombre inférieur de chauffeurs
 * Et ajouter d'autres clients à ce schedule.
 * @param feasible
 */
void Solver::run2(Sol &s, const std::set<int> &feasible) {
    // Essaie de scheduler tous les clients dans

    vector<int> driverCap(_data->GetDriverCount());
    for (int k = 0; k < _data->GetDriverCount(); k++) {
        driverCap[k] = _data->GetDriver(k)->capacity;
    }
    set<vector<int>> combi_driver;
    Combinatorial::find_all_combinations(driverCap, combi_driver, std::min((int) driverCap.size() - 1, 5),
                                         driverCap.size() - 1);

    // Reduire le nombre de chauffeurs pour voir si la solution est toujours réalisable
    int iter = 0;
    while (iter++ < 5) {
        Sol cur(_data);
        cur.keyCustomers = feasible;
        CDPSolver::SolveInstance(cur, *_data, 1);
        if (cur.satisfiedCustomers == feasible) {
            s = cur;
            return;
        }
        cur.ShowCustomer();
        cout << cur.GetCost() << endl;
        Prompt::print(cur.driverUsed);
        bool found = true;
        while (found) {
            found = false;
            for (auto list_d: combi_driver) {
                Sol cur2(_data);
                cur2.keyCustomers = cur.satisfiedCustomers;
                cur2.availableDrivers = set(list_d.begin(), list_d.end());

                Prompt::print(cur2.availableDrivers);
                CDPSolver::SolveInstance(cur2, *_data, 10);

                cout << cur2.CustomerString() << endl;
                if (cur2.hasScheduled(cur.satisfiedCustomers)) {
                    cur = cur2;
                    cout << "found min driver " << endl;
                    cur2.availableDrivers.clear();
                    cur2.keyCustomers.clear();
                    CDPSolver::BuildOnSolution(cur2, *_data, 5);
                    if (cur2.hasScheduled(cur.satisfiedCustomers)) {
                        cur = cur2;
                        cout << "found new best 1" << endl;
                        cout << cur2.CustomerString() << endl;
                        found = true;
//                        break;
                    }
                }
            }
            if (cur < s)
                s = cur;
        }
        s.ShowCustomer();
        cout << s.GetCost() << endl;

    }

}