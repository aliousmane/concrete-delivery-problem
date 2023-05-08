
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
    CDPSolver::disjointClients.resize(_data->GetCustomerCount(), set<int>());
    CDPSolver::fillOperatorList(*_data);
    std::set<int> feasibleClients;

    for(int i=0;i<_data->GetCustomerCount();i++){
        std::vector<int> temp{i};
        Data  dat = _data->copyCustomersData(temp);
        Sol s(&dat);
        CDPSolver::SolveInstance(s,dat,2 );
        if(s.isFeasible)
        {
            cout<<i<<"-"<<std::flush;
            feasibleClients.insert(i);
        }
        else{
            cout<<"["<<i<<"]-"<<std::flush;
        }
    }
    cout<<endl;

    vector<TimeSlot> listInt;
    for(int i:feasibleClients){
        Customer *c = _data->GetCustomer(i);
        listInt.emplace_back(c->early_tw, c->late_tw, c->custID);
    }
    Data dat=*_data;
    vector<set<int>> linkedClientSlot;
    vector<set<int>> linkedClientDemand;
    vector<set<int>> linkedClients;
    CDPSolver::findCorrelation(dat,listInt, linkedClientSlot,linkedClientDemand,linkedClients);
    Prompt::print(feasibleClients);
    CDPSolver::findDisjointSet(*_data,linkedClients);
    Sol s(&dat);
    s.keyCustomers = feasibleClients;

    CDPSolver::deleteOperator();

}

void Solver::SolveGrasp(Sol &s,Data &dat,vector<set<int>> const & linkedClientSlot){

    Sol::FailureCause.resize(dat.GetNodeCount(),Parameters::FAILURECAUSE::NONE);
    Sol::minDelay.resize(dat.GetNodeCount(),0);
    InsRmvMethodFast insrmv(dat);
    InsRmvBuilder1 builder1(dat);
    InsRmvBuilder2 builder2(dat);
    InsRmvBuilder3 builder3(dat);
    CustInsertion custIns1(dat,builder1);
    CustInsertion custIns2(dat,builder2);
    CustInsertion custIns3(dat,builder3);

    vector<InsertOperator<Customer,Driver>*> grasp_heuristics;

    GRASP<Customer,Driver> grasp(&dat);
    vector<pair<int,string>> custInfo={
            {0,"Cust Sort Early TW"},
            {1,"Cust Sort Greater D"},
            {6,"Cust Random"},
            {7,"Cust Sort Kinable"}
    };

    for(const auto& val:custInfo){
//        custInsertionOp.emplace_back(&custIns1,val.first,"Builder1 "+ val.second);
//        custInsertionOp.emplace_back(&custIns2,val.first,"Builder 2 "+ val.second);
        CustInsertionOperator temp3 = CustInsertionOperator(&custIns3,val.first,"Builder 3 "+ val.second);
        grasp.AddInsertOperatorVrp(&temp3);
    }
    RechercheLocale loc_search(s.keyCustomers);
    loc_search.LinkedClientSlot = linkedClientSlot;
    grasp.Optimize(s, nullptr, nullptr, &loc_search,true);
}

