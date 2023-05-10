
#include "Solver.h"
#include "CDPSolver.h"
#include <istream>
#include "CustInsertion.h"
#include "CustInsertionOperator.h"

#include "InsRmvMethodFast.h"
#include "InsRmvBuilder1.h"
#include "InsRmvBuilder2.h"
#include "InsRmvBuilder3.h"

using namespace std;

void Solver::run() {
    CDPSolver::disjointClients.resize(_data->GetCustomerCount(), set < int > ());
    CDPSolver::fillOperatorList(*_data);
    std::set<int> feasibleClients;

    feasibleClients = CDPSolver::EliminateCustomer(*_data, 1);
    Sol s1;
    run2(s1,feasibleClients);
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
    CDPSolver::findCorrelation(dat, listInt, linkedClientSlot, linkedClientDemand, linkedClients);
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
    SolveGrasp(s, dat, linkedClientSlot);
    s.ShowCustomer();
    CDPSolver::deleteOperator();

}

void Solver::SolveGrasp(Sol &s, Data &dat, vector<set<int>> const &linkedClientSlot) {

    Sol::FailureCause.resize(dat.GetNodeCount(), Parameters::FAILURECAUSE::NONE);
    Sol::FailureCount.resize(dat.GetNodeCount(), 0);
    Sol::minDelay.resize(dat.GetNodeCount(), 0);
    TimeSlot::myData = dat;
    InsRmvMethodFast insrmv(dat);
    InsRmvBuilder1 builder1(dat);
    InsRmvBuilder2 builder2(dat);
    InsRmvBuilder3 builder3(dat);
    CustInsertion custIns1(dat, builder1);
    CustInsertion custIns2(dat, builder2);
    CustInsertion custIns3(dat, builder3);

    vector<InsertOperator<Customer, Driver> *> grasp_heuristics;
    vector<CustInsertionOperator> custInsertionOp;

    GRASP<Customer, Driver> grasp(&dat);
    vector<pair<int, string>> custInfo = {
            {0, "Cust Sort Early TW"},
            {1, "Cust Sort Greater D"},
            {6, "Cust Random"},
            {7, "Cust Sort Kinable"}
    };

    custInsertionOp.reserve(3 * custInfo.size());
    for (const auto &val: custInfo) {
//        custInsertionOp.emplace_back(&custIns1,val.first,"Builder1 "+ val.second);
//        custInsertionOp.emplace_back(&custIns2,val.first,"Builder 2 "+ val.second);
        custInsertionOp.emplace_back(&custIns3, val.first, "Builder 3 " + val.second);
//        cout<<&temp3<<endl;
    }
    CustInsertionOperator c0(&custIns3, custInfo[0].first, "Builder 3 " + custInfo[0].second);
    CustInsertionOperator c1(&custIns3, custInfo[1].first, "Builder 3 " + custInfo[1].second);
    CustInsertionOperator c2(&custIns3, custInfo[2].first, "Builder 3 " + custInfo[2].second);
    CustInsertionOperator c3(&custIns3, custInfo[3].first, "Builder 3 " + custInfo[3].second);
    grasp.AddInsertOperatorVrp(&c0);
    grasp.AddInsertOperatorVrp(&c1);
    grasp.AddInsertOperatorVrp(&c2);
    grasp.AddInsertOperatorVrp(&c3);
    grasp.verbose = true;
    grasp.SetIterationCount(500);
    RechercheLocale loc_search(s.keyCustomers);
    loc_search.LinkedClientSlot = linkedClientSlot;
    grasp.Optimize(s, nullptr, nullptr, &loc_search, true);
}

/**
 * Essayer de trouver le même schedule avec un nombre inférieur de chauffeurs
 * Et ajouter d'autres clients à ce schedule.
 * @param feasible
 */
void Solver::run2(Sol &s, std::set<int> feasible) {
    // Essaie de scheduler tous les clients dans

    vector<int> driverCap;
    for (int k = 0; k < _data->GetDriverCount(); k++) {
        driverCap.push_back(_data->GetDriver(k)->capacity);
    }
    set<vector<int>> combi_driver;
    CDPSolver::find_all_combinations(driverCap, combi_driver, std::min((int)driverCap.size() - 1,5), driverCap.size() - 1);

    // Reduire le nombre de chauffeurs pour voir si la solution est toujours réalisable
    int iter=0;
    while(iter++<5){
        Sol cur(_data);
        cur.keyCustomers = feasible;
        CDPSolver::SolveInstance(cur, *_data, 1);
        if(cur.satisfiedCustomers == feasible)
        {
            s=cur;
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
                } else {
                    cur2.availableDrivers.clear();
                    cur2.keyCustomers.clear();
                    CDPSolver::BuildOnSolution(cur2, *_data, 5);
                    cout << "2) " << cur2.CustomerString() << endl;
                    if (cur2 < cur) {
                        cur = cur2;
                        found = true;
                        cout << "found new best 2" << endl;
//                        break;
                    }
                    if (cur2.hasScheduled(cur.satisfiedCustomers) and
                        cur2.GetLastCost().driverUsed < cur.GetLastCost().driverUsed) {
                        cur = cur2;
                        cout << "found new best 3" << endl;
                        found = true;
//                        break;
                    }
                }
            }
            if(cur<s)
                s=cur;
        }
        s.ShowCustomer();
        cout << s.GetCost() << endl;

    }

}