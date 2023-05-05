
#include "Solver.h"
#include "CustInsertion.h"
#include "CustInsertionOperator.h"
#include "InsRmvMethodFast.h"
#include "InsRmvBuilder1.h"
#include "InsRmvBuilder2.h"
#include "InsRmvBuilder3.h"
#include <istream>

using namespace std;
void Solver::run() {

    std::set<int> feasibleClients;
    for(int i=0;i<_data->GetCustomerCount();i++){
//        cout<<*_data->GetCustomer(i)<<endl;
        std::vector<int> temp{i};
        Data dat = _data->copyCustomersData(temp);
        Sol s;
        SolveInstance(s,dat );
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
    Data dat = _data->copyCustomersData(feasibleClients);
    Sol s;
    SolveInstance(s,dat );
    s.ShowCustomer();
}

void Solver::SolveInstance(Sol &sol,Data &dat){
    Sol::FailureCause.resize(dat.GetNodeCount(),Parameters::FAILURECAUSE::NONE);
    Sol::minDelay.resize(dat.GetNodeCount(),0);
    Sol s(&dat);
    s.PutAllCustomersToUnassigned();

    InsRmvMethodFast insrmv(dat);
    InsRmvBuilder1 builder1(dat);
    InsRmvBuilder2 builder2(dat);
    InsRmvBuilder3 builder3(dat);
    CustInsertion custIns1(dat,builder1);
    CustInsertion custIns2(dat,builder2);
    CustInsertion custIns3(dat,builder3);

    vector<CustInsertionOperator> custInsertionOp;

    vector<pair<int,string>> custInfo={
//            {0,"Cust Sort Early TW"},
            {1,"Cust Sort Greater D"},
//            {6,"Cust Random"},
//            {7,"Cust Sort Kinable"}
    };


    custInsertionOp.reserve(3*custInfo.size());
    for(const auto& val:custInfo){
//        custInsertionOp.emplace_back(&custIns1,val.first,"Builder1 "+ val.second);
//        custInsertionOp.emplace_back(&custIns2,val.first,"Builder 2 "+ val.second);
        custInsertionOp.emplace_back(&custIns3,val.first,"Builder 3 "+ val.second);
    }

    for(int i=0;i<10;i++){
        for(auto heur: custInsertionOp){
            Sol cur(&dat);
            cur.PutAllCustomersToUnassigned();
            heur.Insert(cur);
            if(cur<s){
//                cout<<"current best cost " <<cur.GetCost().satisfiedCost <<" with "<<heur.name <<endl;
                s=cur;
            }
//            break;
        }
    }
    sol = s;
//    s.ShowSchedule();
//    sol.ShowCustomer();
}
