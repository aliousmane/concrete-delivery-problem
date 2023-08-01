
#include "Solver.h"
#include "CDPSolver.h"
#include <istream>
#include "CustInsertion.h"
#include "InsRmvMethodFast.h"
#include "InsRmvBuilder1.h"
#include "InsRmvBuilder3.h"

using namespace std;

void Solver::run() {

    vector<TimeSlot> listInt;

    vector<set<int>> linkedClientSlot;
    vector<set<int>> linkedClientDemand;
    vector<set<int>> linkedClients;
    vector<set<int>> linkedClientsInf;
    vector<set<int>> linkedClientSup;
    Data dat = *_data;

    feasibleClients.clear();
    for (int i = 0; i < dat.GetCustomerCount(); i++) {
        Customer *c = dat.GetCustomer(i);
        listInt.emplace_back(c->early_tw, c->early_tw + ceil(Data::UnloadingTime(c->demand, c->demand)) + 120, c->custID);
        feasibleClients.insert(c->custID);
    }

    CDPSolver::findCorrelation(dat, listInt, linkedClientSlot, linkedClientDemand, linkedClients, linkedClientsInf,
                               linkedClientSup);

    Sol s(&dat);
    s.keyCustomers = feasibleClients;
    SolveGrasp(s, dat, linkedClientSlot, Parameters::ITERATION);
    SaveResults(s);
    s.exportCSVFormat("s1.csv");
    cout << s.GetLastCost() << endl;
    Prompt::print(s.unscheduledCustomers);
}

void Solver::SolveGrasp(Sol &s, Data &dat, vector<set<int>> const &linkedClientSlot, int iter) {

    CDPSolver::nbSatisfied.resize(dat.GetCustomerCount());

    InsRmvBuilder3 builder3(dat);
    InsRmvBuilder1 builder1(dat);
    CustInsertion custIns1(dat, builder1);
    CustInsertion custIns3(dat, builder3);
    PriorityQueueInsertion prioIns3(dat, builder3);
    PriorityQueueInsertion prioIns1(dat, builder1);
    DriverInsertion driverIns3(dat, builder3);

    vector<AllInsertionOperator> grasp_heuristics;

    GRASP<Customer, Driver> grasp(&dat);
    vector<pair<int, string>> custInfo = {
            {0,  "Cust Early TW"},
            {1,  "Cust Greater D"},
            {2,  "Cust Late TW "},
            {3,  "Cust Min Width TW"},
            {4,  "Cust Random"},
            {5,  "Cust Kinable"},
            {-1, "Cust Fixed"},
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

    AllInsertionOperator c0_1(&custIns1, custInfo[0].first, "Builder 1 " + custInfo[0].second);
    AllInsertionOperator c0_3(&custIns3, custInfo[0].first, "Builder 3 " + custInfo[0].second);
    AllInsertionOperator c1_1(&custIns1, custInfo[1].first, "Builder 1 " + custInfo[1].second);
    AllInsertionOperator c1_3(&custIns3, custInfo[1].first, "Builder 3 " + custInfo[1].second);
    AllInsertionOperator c2_1(&custIns1, custInfo[2].first, "Builder 1 " + custInfo[2].second);
    AllInsertionOperator c2_3(&custIns3, custInfo[2].first, "Builder 3 " + custInfo[2].second);
    AllInsertionOperator c3_1(&custIns1, custInfo[3].first, "Builder 1 " + custInfo[3].second);
    AllInsertionOperator c3_3(&custIns3, custInfo[3].first, "Builder 3 " + custInfo[3].second);
    AllInsertionOperator c4_1(&custIns1, custInfo[4].first, "Builder 1 " + custInfo[4].second);
    AllInsertionOperator c4_3(&custIns3, custInfo[4].first, "Builder 3 " + custInfo[4].second);
    AllInsertionOperator c5_1(&custIns1, custInfo[5].first, "Builder 1 " + custInfo[5].second);
    AllInsertionOperator c5_3(&custIns3, custInfo[5].first, "Builder 3 " + custInfo[5].second);
    AllInsertionOperator cFixed(&custIns1, custInfo[6].first, "Builder 3 " + custInfo[6].second);

    AllInsertionOperator p03(&prioIns3, priorityInfo[0].first, "Builder 3 " + priorityInfo[0].second);
    AllInsertionOperator p01(&prioIns1, priorityInfo[0].first, "Builder 3 " + priorityInfo[0].second);
    AllInsertionOperator p13(&prioIns3, priorityInfo[1].first, "Builder 3 " + priorityInfo[1].second);
    AllInsertionOperator p11(&prioIns1, priorityInfo[1].first, "Builder 3 " + priorityInfo[1].second);
    AllInsertionOperator p21(&prioIns1, priorityInfo[2].first, "Builder 3 " + priorityInfo[2].second);
    AllInsertionOperator p23(&prioIns3, priorityInfo[2].first, "Builder 3 " + priorityInfo[2].second);
    AllInsertionOperator p31(&prioIns1, priorityInfo[3].first, "Builder 3 " + priorityInfo[3].second);
    AllInsertionOperator p33(&prioIns3, priorityInfo[3].first, "Builder 3 " + priorityInfo[3].second);
    AllInsertionOperator p41(&prioIns1, priorityInfo[4].first, "Builder 3 " + priorityInfo[4].second);
    AllInsertionOperator p43(&prioIns3, priorityInfo[4].first, "Builder 3 " + priorityInfo[4].second);
    AllInsertionOperator p51(&prioIns1, priorityInfo[5].first, "Builder 3 " + priorityInfo[5].second);
    AllInsertionOperator p53(&prioIns3, priorityInfo[5].first, "Builder 3 " + priorityInfo[5].second);

    AllInsertionOperator d0(&driverIns3, driverInfo[0].first, "Builder 3 " + driverInfo[0].second);
    AllInsertionOperator d1(&driverIns3, driverInfo[1].first, "Builder 3 " + driverInfo[1].second);
    AllInsertionOperator d2(&driverIns3, driverInfo[2].first, "Builder 3 " + driverInfo[2].second);


    grasp.AddInsertOperatorVrp(&c0_1);
    grasp.AddInsertOperatorVrp(&c0_3);
    grasp.AddInsertOperatorVrp(&c1_1);
    grasp.AddInsertOperatorVrp(&c1_3);
    grasp.AddInsertOperatorVrp(&c2_1);
    grasp.AddInsertOperatorVrp(&c2_3);
    grasp.AddInsertOperatorVrp(&c3_1);
    grasp.AddInsertOperatorVrp(&c3_3);
    grasp.AddInsertOperatorVrp(&c4_1);
    grasp.AddInsertOperatorVrp(&c4_3);
    grasp.AddInsertOperatorVrp(&c5_1);
    grasp.AddInsertOperatorVrp(&c5_3);
//    grasp.AddInsertOperatorVrp(&cFixed);

    grasp.AddInsertOperatorVrp(&p01);
    grasp.AddInsertOperatorVrp(&p03);
    grasp.AddInsertOperatorVrp(&p11);
    grasp.AddInsertOperatorVrp(&p13);
    grasp.AddInsertOperatorVrp(&p23);
    grasp.AddInsertOperatorVrp(&p33);
    grasp.AddInsertOperatorVrp(&p43);
    grasp.AddInsertOperatorVrp(&p53);

//    grasp.AddInsertOperatorVrp(&d0);
//    grasp.AddInsertOperatorVrp(&d1);
//    grasp.AddInsertOperatorVrp(&d2);

    grasp.verbose = true;
    grasp.SetIterationCount(iter);
    RechercheLocale loc_search(s.keyCustomers);
    loc_search.LinkedClientSlot = linkedClientSlot;
    if (Parameters::LOCAL_SEARCH) {
        grasp.Optimize(s, nullptr, nullptr, &loc_search, false);
    } else {
        grasp.Optimize(s, nullptr, nullptr, nullptr, false);
    }
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
            fprintf(f, "%s;", s.GetLastCost().str().c_str());
            fprintf(f, "%d;", s.GetDepotCount());
            fprintf(f, "%ld\n", Parameters::GetElapsedTime());
            fclose(f);
            printf("Saved to %s\n", _data->result_file.c_str());
        }
    }
}

void Solver::Test() {
    Sol s(_data);
    InsRmvBuilder3 builder3(*_data);
    Parameters::SORT_TYPE = Parameters::SORT::FIVE;
    CustInsertion custIns(*_data, builder3);
//    CDPSolver::SolveInstance(s,*_data,100);
    s.keyCustomers = {59};
    custIns.Insert(s);
//    s.ShowSchedule();
    s.ShowCustomer();
    cout << s.GetLastCost() << endl;
    s.exportCSVFormat("s.csv");
    Parameters::ShowTime();
}

void Solver::TestTimeSlot() {

    auto a = TimeSlot(0, 3);
    auto b = TimeSlot(4, 5);

    std::set < TimeSlot > ab;
    ab.insert(a);
    ab.insert(b);
    Prompt::print(ab);

    exit(1);
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
