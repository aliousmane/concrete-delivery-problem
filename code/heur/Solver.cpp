
#include "Solver.h"
#include "CustInsertion.h"
#include "CustInsertionOperator.h"
#include "InsRmvMethodFast.h"
#include "InsRmvBuilder1.h"
#include "InsRmvBuilder2.h"
#include "InsRmvBuilder3.h"
#include <istream>
#include "RechercheLocale.h"


using namespace std;
std::vector<std::set<int>> Solver::disjointClients = vector<set<int>>();
std::vector<InsertOperator<Node,Driver>*> Solver::listOperators=
        std::vector< InsertOperator<Node,Driver>*>();


void Solver::fillOperatorList(Data &dat){
    if(!Solver::listOperators.empty())
        deleteOperator();
    Sol::FailureCause.resize(dat.GetNodeCount(),Parameters::FAILURECAUSE::NONE);
    Sol::minDelay.resize(dat.GetNodeCount(),0);
    InsRmvMethodFast insrmv(dat);
    InsRmvBuilder1 builder1(dat);
    InsRmvBuilder2 builder2(dat);
    InsRmvBuilder3 builder3(dat);
//    CustInsertion custIns1(dat,builder1);
//    CustInsertion custIns2(dat,builder2);
//    CustInsertion custIns3(dat,builder3);


    vector<CustInsertionOperator> custInsertionOp;

    vector<pair<int,string>> custInfo={
//            {0,"Cust Sort Early TW"},
//            {1,"Cust Sort Greater D"},
            {6,"Cust Random"},
            {7,"Cust Sort Kinable"}
    };
    custInsertionOp.reserve(3*custInfo.size());
    for(const auto& val:custInfo){
//        custInsertionOp.emplace_back(&custIns1,val.first,"Builder1 "+ val.second);
//        custInsertionOp.emplace_back(&custIns2,val.first,"Builder 2 "+ val.second);
//        custInsertionOp.emplace_back(&custIns3,val.first,"Builder 3 "+ val.second);
        std::unique_ptr<InsRmvBuilder> build3 ( new InsRmvBuilder3(dat));

        std::unique_ptr<CustInsertion> temp3 ( new CustInsertion (dat,std::move(build3)));
        cout<<temp3<<endl;
        Solver::listOperators.push_back(new CustInsertionOperator(std::move( temp3),val.first,"Builder 3 "+ val.second));
        cout<<temp3<<endl;
    }

}

void Solver::run() {
    Solver::disjointClients.resize(_data->GetCustomerCount(), set<int>());
    Solver::fillOperatorList(*_data);
    std::set<int> feasibleClients;

    for(int i=0;i<_data->GetCustomerCount();i++){
        std::vector<int> temp{i};
        Data  dat = _data->copyCustomersData(temp);
        Sol s(&dat);
        SolveInstance(s,dat,2 );
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
    Solver::findCorrelation(dat,listInt, linkedClientSlot,linkedClientDemand,linkedClients);
    Prompt::print(feasibleClients);
    Solver::findDisjointSet(*_data,linkedClients);
    Sol s(&dat);
    s.keyCustomers = feasibleClients;


}

void Solver::SolveGrasp(Sol &s,Data &dat,vector<set<int>> const & linkedClientSlot){
    Solver::fillOperatorList(dat);
    cout<<" Initial "<<s.CustomerString()<<endl;
    RechercheLocale loc_search(s.keyCustomers);
    int iter=0;
    while(iter++<50){
        Sol cur(&dat);
        cur.keyCustomers = s.keyCustomers;
//        Solver::SolveInstance(cur,*_data,1);
        loc_search.Run(cur,linkedClientSlot);
        if(cur<s){
            s=cur;
            cout<<"Find best"<<cur.CustomerString()<<endl;
        }
    }
    s.ShowCustomer();
}

void Solver::SolveInstance(Sol &s,Data &dat,int iter){

    Sol::FailureCause.resize(dat.GetNodeCount(),Parameters::FAILURECAUSE::NONE);
    Sol::minDelay.resize(dat.GetNodeCount(),0);
    InsRmvMethodFast insrmv(dat);
    InsRmvBuilder1 builder1(dat);
    InsRmvBuilder2 builder2(dat);
    InsRmvBuilder3 builder3(dat);
//    CustInsertion custIns1(dat,builder1);
//    CustInsertion custIns2(dat,builder2);
//    CustInsertion custIns3(dat,builder3);


    vector<CustInsertionOperator> custInsertionOp;


    vector<pair<int,string>> custInfo={
            {0,"Cust Sort Early TW"},
            {1,"Cust Sort Greater D"},
            {6,"Cust Random"},
            {7,"Cust Sort Kinable"}
    };
    custInsertionOp.reserve(3*custInfo.size());
    for(const auto& val:custInfo){
//        custInsertionOp.emplace_back(&custIns1,val.first,"Builder1 "+ val.second);
//        custInsertionOp.emplace_back(&custIns2,val.first,"Builder 2 "+ val.second);
//        CustInsertion *temp3 = new CustInsertion (dat,builder3);
//        custInsertionOp.emplace_back(&custIns3,val.first,"Builder 3 "+ val.second);
//        listOperators.emplace_back()
    }

    for(int i=0;i<iter;i++) {
        for (auto  heur : Solver::listOperators) {

            Sol cur(&dat);
            cur.keyCustomers = s.keyCustomers;
            cur.PutAllCustomersToUnassigned();
            heur->Insert(cur);
            if (cur < s) {
                cout << "current best cost " << cur.GetCost().satisfiedCost << " with " << heur->name << endl;
                s = cur;
            }
        }
    }
}

void Solver::findCorrelation(Data &dat,  std::vector<TimeSlot> const & listInt, std::vector<std::set<int>> & linkedClientSlot,
                            std::vector<std::set<int>> & linkedClientDemand, std::vector<std::set<int>>& linkedClients){
        linkedClientSlot.clear();
        linkedClientDemand.clear();
        linkedClients.clear();
        linkedClientSlot.resize(dat.GetCustomerCount());
        linkedClientDemand.resize(dat.GetCustomerCount());
        linkedClients.resize(dat.GetCustomerCount());
        for (int i = 0; i < listInt.size(); i++)
        {
            auto intv1 = listInt[i];
            Customer *c1 = dat.GetCustomer(intv1.nodeID);
            Depot *dep1 = dat.GetDepot(c1->depotID);
            for (int j = 0; j < listInt.size(); j++)
            {
                if (i == j)
                    continue;
                auto intv2 = listInt[j];
                Customer *c2 = dat.GetCustomer(intv2.nodeID);
                Depot *dep2 = dat.GetDepot(c2->depotID);
                if (intv1 == intv2)
                {
                    linkedClientSlot[intv1.nodeID].insert(intv2.nodeID);
                    linkedClients[intv1.nodeID].insert(intv2.nodeID);
                }
                else if (intv1 < intv2)
                {
                    if (intv1.upper + dat.Travel(c1,dep2) +
                        dat.Travel(dep2, c2) >= intv2.lower)
                    {
                        linkedClients[intv1.nodeID].insert(intv2.nodeID);
                    }
                }
                else if (intv1 > intv2)
                {
                    if (intv2.upper + dat.Travel(c2, dep1) +
                        dat.Travel(dep1, c1) >= intv1.lower)
                    {
                        linkedClients[intv1.nodeID].insert(intv2.nodeID);
                    }
                }

                if (c1->demand == c2->demand)
                {
                    linkedClientDemand[intv1.nodeID].insert(intv2.nodeID);
                    linkedClients[intv1.nodeID].insert(intv2.nodeID);
                }
            }
        }
}

void Solver::findDisjointSet(Data &dat, std::vector<std::set<int>> const & linkedClients){
    map<set<int>, bool> seen_pairs;

    for (int i = 0; i < linkedClients.size(); i++)
    {
        auto _set = linkedClients[i];
        if (_set.empty())
            continue;
        for (auto val : _set)
        {
            set<int> my_set{i, val};
            if (seen_pairs.find(my_set) != seen_pairs.end())
            {
                continue;
            }
            else
            {
                seen_pairs[my_set] = true;
//                Prompt::print(my_set);
                Data dat1 = dat.copyCustomersData(my_set);
                Sol cur(&dat1);
                SolveInstance(cur,dat1,5);
//                cur.ShowCustomer();
                if (!cur.hasScheduled(my_set))
                {
                    Solver::disjointClients[i].insert(val);
                    Solver::disjointClients[val].insert(i);

                }
            }
        }
    }
}


void Solver::PathRelinking(Sol &new_cur, std::vector<Customer *> &list_cust,
                          std::vector<BestSolutionList<Customer, Driver>>  & vectSolutions,
                          Cost &bestCout, Sol &best){
    // cout << "begin path relinking\n";
    for (Customer *c : list_cust)
    {
        //TODO if (Sol::nbSatisfied[c->custID] > 0)
        {
            std::vector<Sol *> v;
            vectSolutions[c->custID].GetSolutions(v);
            std::vector<Order*> orders = new_cur.GetOrders(c);

            int count = 0;
            bool isfeasible = true;
            for (auto cur : v)
            {
                std::vector<Delivery *> deliveries;
                for (auto o1 : orders)
                {
                    for (int i = 0; i < cur->GetDeliveryCount(o1); i++)
                    {
                        Delivery *del = cur->GetDelivery(o1, i);
                        if (cur->GetDriverAssignedTo(del) != nullptr)
                            deliveries.push_back(del);
                    }
                }
                // Insert elements of deliveries in new_cur
                for (Delivery *del : deliveries)
                {
                    if (new_cur.GetDriverAssignedTo(del) != nullptr)
                        continue;
                    isfeasible = InsertDel(new_cur, cur, del);
                    if (not isfeasible)
                        break;
                }
                if (not isfeasible)
                    continue;
                count++;
                // printf("%d realisable\n", count - 1);
                // cout << "Insert " << c->custID << " in new_cost\n"
                break;
            }
            if (not isfeasible)
            {
                for (auto o1 : orders)
                {
                    new_cur.UnassignOrder(o1);
                }
            }
        }
    }

    repairSolution(new_cur);
    if (!new_cur.isFeasible)
        return;
    Cost coutActuel = new_cur.GetCost();

}


bool Solver::InsertDel(Sol &sol_cur, Sol *cur, Delivery *del)
{
    Dock *dock = cur->GetDock(del->dockID);
    Depot *dep = cur->GetDepotAssignedTo(dock);
    Driver *d = cur->GetDriverAssignedTo(del);
    assert(d == cur->GetDriverAssignedTo(dock));
    // si chauffeur occupé, arrêter et supprimer les autres noeuds
    TimeSlot intv = cur->nodeServiceIntervals[dock->id];
    if (sol_cur.depotLoadingIntervals[dep->depotID].find(intv) !=
        sol_cur.depotLoadingIntervals[dep->depotID].end())
    {
        // cout << intv << " depot occupied\n";
        // return false;
    }
    intv = cur->nodeServiceIntervals[del->id];
    auto *prec_del = dynamic_cast<Delivery*>( sol_cur.CustomerPrev[del->id]);
    if (prec_del != nullptr)
    {
            Dock *prec_dock = sol_cur.GetDock(prec_del->dockID);
            if (intv.lower < sol_cur.nodeServiceIntervals[prec_dock->id].lower)
                return false;
            if (sol_cur.nodeServiceIntervals[prec_del->id].lower > intv.lower)
                return false;
    }

    if (sol_cur.driverWorkingIntervals[d->id].find(intv) !=
        sol_cur.driverWorkingIntervals[d->id].end())
    {
        return false;
    }
    auto it = sol_cur.driverWorkingIntervals[d->id].lower_bound(intv);

    if (it == sol_cur.driverWorkingIntervals[d->id].end() and
        not sol_cur.driverWorkingIntervals[d->id].empty())
    {
        it--;
    }

    if (it != sol_cur.driverWorkingIntervals[d->id].end())
    {
        auto *del_it = dynamic_cast<Delivery*>( sol_cur.GetNode(it->nodeID));
        Dock *dock_it = sol_cur.GetDock( del_it->dockID);
        if (*it < intv)
        {
            // finir it->upper avant de commencer au dock à intv.lower
            if (it->upper + sol_cur.Travel(del_it, dock) > intv.lower)
            {
                return false;
            }
            auto it1 = std::next(it);
            if (it1 != sol_cur.driverWorkingIntervals[d->id].end())
            {
                auto *del_it1 = dynamic_cast<Delivery*>( sol_cur.GetNode(it1->nodeID));
                Dock *dock_it2 = sol_cur.GetDock(del_it1->dockID);
                if (intv.upper + sol_cur.Travel(del, dock_it2) > it1->lower)
                {
                    return false;
                }
            }
        }
        else if (*it > intv)
        {
            if (intv.upper + sol_cur.Travel(del, dock_it) > it->lower)
            {
                return false;
            }
            if (it != sol_cur.driverWorkingIntervals[d->id].begin())
            {
                auto it2 = std::prev(it);
                if (it2 != it)
                {
                    if (it2 != sol_cur.driverWorkingIntervals[d->id].end())
                    {
                        auto *temp_del = dynamic_cast<Delivery*>( sol_cur.GetNode(it2->nodeID));
                        if (it2->upper + sol_cur.Travel(temp_del, dock) > intv.lower)
                        {
                            return false;
                        }
                    }
                }
            }
        }
    }
    sol_cur.DriverAssignTo[dock->id] = d;
    sol_cur.InsertAfter(del, dock, d);
    sol_cur.AssignDeliveryToCustomer(del);
    sol_cur.UpdateDepotLoadingSet(dep, dock, cur->nodeServiceIntervals[dock->id]);
    sol_cur.driverWorkingIntervals[d->id].insert(
            cur->nodeServiceIntervals[del->id]);
    sol_cur.ArrivalTime[dock->id] = cur->ArrivalTime[dock->id];
    sol_cur.ArrivalTime[del->id] = cur->ArrivalTime[del->id];

    return true;
}


void Solver::repairSolution(Sol &new_cur)
{
    new_cur.BuildFromDepotSetIntervall();
//TODO	new_cur.BuildFromDriverSetIntervall();
    repairSchedule(new_cur);
}

void Solver::repairSchedule(Sol &sol)
{
//TODO    if (_insrmv != nullptr)
//    {
//        _insrmv->repairSolution(sol);
//    }
}
