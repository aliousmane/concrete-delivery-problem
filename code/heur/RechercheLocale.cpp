#include "RechercheLocale.h"
#include "../Prompt.h"


using namespace std;

RechercheLocale::RechercheLocale() : customerIdList(0), bestCost(false)
{
}

RechercheLocale::RechercheLocale(std::set<int> const& feasibleClients)
{
    customerIdList = std::vector<int>(feasibleClients.begin(), feasibleClients.end());
}

void RechercheLocale::Run(Sol &s)
{
//    cout<<"Start LS\n";
//    s.ShowCustomer();
    bestCost=s.GetLastCost();
    Sol cur = s;
    found = true;
    customerIdList= vector<int>(s.unscheduledCustomers.begin(),s.unscheduledCustomers.end());
    shuffle(customerIdList.begin(), customerIdList.end(), Parameters::RANDOM_GEN);
    std::vector<int> seen(s.GetCustomerCount(), -1);
    while (found)
    {
        // cout<<"Iteration ******************\n";
        found = false;
        for (auto id : customerIdList)
        {
            Customer *c1 = s.GetCustomer(id);
            if (LinkedClientSlot[c1->custID].empty())
                continue;
            // if(seen[c1->custID]!=-1)                continue;
            if (s.isClientSatisfied(c1))
                continue;
            TimeSlot Intc1 = TimeSlot(c1->early_tw, c1->late_tw, c1->custID);
            bool isInserted = false;
            std::set<int> _set;
            for (auto custId : LinkedClientSlot[c1->custID])
            {
                Customer *c2 = s.GetCustomer(custId);
                if (!s.isClientSatisfied(c2))
                    continue;
                TimeSlot Intc2 = TimeSlot(c2->early_tw, c2->late_tw, c2->custID);
                if (Intc1 == Intc2)
                {
                    _set.insert(c2->custID);
                }
                if (Swap(s, c1, c2))
                {
                    isInserted = true;
                    if (!s.isClientSatisfied(c2))
                        seen[c2->custID] = c2->custID;
                    break;
                }
                if (Relocate(s, c1, c2))
                {
                    isInserted = true;
                    if (!s.isClientSatisfied(c2))
                        seen[c2->custID] = c2->custID;
                    break;
                }
            }
            // if(!s.isClientSatisfied(c1) && !_set.empty()){
            //     Relocate(s,c1,_set);
            // }
        }
    }
//    cout<<"End LS\n";
}

void RechercheLocale::Run(Sol &s, std::vector<std::set<int>> const &LinkedClients)
{
    Sol cur = s;
    found = true;
    shuffle(customerIdList.begin(), customerIdList.end(), Parameters::RANDOM_GEN);
    std::vector<int> seen(s.GetCustomerCount(), -1);
    while (found)
    {
        // cout<<"Iteration ******************\n";
        found = false;
        for (auto id : customerIdList)
        {
            Customer *c1 = s.GetCustomer(id);
            if (LinkedClients[c1->custID].empty())
                continue;
            // if(seen[c1->custID]!=-1)                continue;
            if (s.isClientSatisfied(c1))
                continue;
            TimeSlot Intc1 = TimeSlot(c1->early_tw, c1->late_tw, c1->custID);
            bool isInserted = false;
            std::set<int> _set;
            for (auto custId : LinkedClients[c1->custID])
            {
                Customer *c2 = s.GetCustomer(custId);
                if (!s.isClientSatisfied(c2))
                    continue;
                TimeSlot Intc2 = TimeSlot(c2->early_tw, c2->late_tw, c2->custID);
                if (Intc1 == Intc2)
                {
                    _set.insert(c2->custID);
                }
                if (Relocate(s, c1, c2))
                {
                    isInserted = true;
                    if (!s.isClientSatisfied(c2))
                        seen[c2->custID] = c2->custID;
                    break;
                }
                if (Swap(s, c1, c2))
                {
                    isInserted = true;
                    if (!s.isClientSatisfied(c2))
                        seen[c2->custID] = c2->custID;
                    break;
                }
            }
            // if(!s.isClientSatisfied(c1) && !_set.empty()){
            //     Relocate(s,c1,_set);
            // }
        }
    }
}

void RechercheLocale::Run(Sol &s, std::vector<std::set<int>> &LinkedClientSlot, std::vector<std::set<int>> &LinkedClientDemand)
{
    Sol cur = s;
    found = true;
    shuffle(customerIdList.begin(), customerIdList.end(), Parameters::RANDOM_GEN);
    while (found)
    {
        found = false;
        for (auto id : customerIdList)
        {
            Customer *c1 = s.GetCustomer(id);
            if (LinkedClientSlot[c1->custID].empty())
                continue;

            if (s.isClientSatisfied(c1))
                continue;
            c1->Show();
            bool isInserted = false;
            std::set<int> _set;
            for (auto custId : LinkedClientSlot[c1->custID])
            {
                Customer *c2 = s.GetCustomer(custId);
                if (!s.isClientSatisfied(c2))
                    continue;
                TimeSlot Intc2 = TimeSlot(c2->early_tw, c2->late_tw, c2->custID);

                _set.insert(c2->custID);
                if (Relocate(s, c1, c2))
                {
                    isInserted = true;
                    break;
                }
                if (Swap(s, c1, c2))
                {
                    isInserted = true;
                    break;
                }
            }
            if (!s.isClientSatisfied(c1) && !_set.empty())
            {
                Relocate(s, c1, _set);
            }
            _set.clear();
            for (auto custId : LinkedClientDemand[c1->custID])
            {
                Customer *c2 = s.GetCustomer(custId);
                if (!s.isClientSatisfied(c2))
                    continue;
                TimeSlot Intc2 = TimeSlot(c2->early_tw, c2->late_tw, c2->custID);

                _set.insert(c2->custID);

                if (Relocate(s, c1, c2))
                {
                    cout << "Now demand\n";

                    isInserted = true;
                    break;
                }
                if (Swap(s, c1, c2))
                {
                    cout << "Now demand\n";

                    isInserted = true;
                    break;
                }
            }
            if (!s.isClientSatisfied(c1) && !_set.empty())
            {

                if (Relocate(s, c1, _set))
                {
                    cout << "Now demand\n";
                }
            }
        }
    }
}

bool RechercheLocale::Swap(Sol &s, Customer *c1, Customer *c2)
{
    if (c1->demand < c2->demand) return false;
    std::set<int> clients = s.satisfiedCustomers;
    clients.erase(c2->custID);
    clients.insert(c1->custID);

    std::set<int> v_intersection;
    // TODO
    std::set_intersection(clients.begin(), clients.end(),
                          CDPSolver::disjointClients[c1->custID].begin(), CDPSolver::disjointClients[c1->custID].end(),
                          std::inserter(v_intersection, v_intersection.end()));
    if (!v_intersection.empty())
        return false;

    Sol cur=s;
    cur.keyCustomers = clients;
    bool sortie = false;
     cur.UnassignCustomer(c2);

//    if (SolverReduce::findSequence(s, clients))
//        return false;
    if (CDPSolver::ComputeCost(s, clients) < bestCost.satisfiedCost)
        return false;
    CDPSolver::BuildOnSolution(cur, *s.GetData(),1);
//    SolverReduce::SolvedSequence[cur.satisfiedCustomers] =
//        SequenceInfo(true, cur.GetLastCost().satisfiedCost);
    if (cur.GetLastCost().satisfiedCost > s.GetLastCost().satisfiedCost)
    {
        s = cur;
        found = true;
        sortie = true;
//        cout << "new sol swap " << cur.CustomerString() << endl;
        if (cur.GetLastCost()< bestCost)
        {
            bestCost = s.GetLastCost();
        }
    }

    return sortie;
}
bool RechercheLocale::Relocate(Sol &s, Customer *c1, std::set<int> &_set)
{
    Sol cur = s;
    bool sortie = false;
    for (auto id : _set)
    {
        cur.UnassignCustomer(s.GetCustomer(id));
    }
    _set.insert(c1->custID);
    // cur.ShowCustomer();
//    cout << " try to relocate with all  " << endl;
//    SolverReduce::SolveCustomer(cur, _set, 1, nullptr, false);
//    if (cur.GetLastCost() < s.GetLastCost())
//    {
//        s = cur;
//        found = true;
//        sortie = true;
//        cout << "new sol relocate all " << cur.CustomerString() << endl;
//    }
//    else
//    {
//        for (auto val : s.satisfiedCustomers)
//        {
//            _set.insert(val);
//        }
//        Sol cur2(s.GetProb());
//
//        SolverReduce::SolveCustomer(cur2, _set, 1, nullptr, false);
//        if (cur2.GetLastCost() < s.GetLastCost())
//        {
//            s = cur2;
//            found = true;
//            sortie = true;
//            cout << "new sol relocate all 2 " << cur2.CustomerString() << endl;
//        }
//    }
    return sortie;
}

bool RechercheLocale::Relocate(Sol &s, Customer *c1, Customer *c2)
{
    std::set<int> v_intersection;
    std::set_intersection(s.satisfiedCustomers.begin(), s.satisfiedCustomers.end(),
                          CDPSolver::disjointClients[c1->custID].begin(), CDPSolver::disjointClients[c1->custID].end(),
                          std::inserter(v_intersection, v_intersection.end()));
    if (!v_intersection.empty())
        return false;
    set<int> clients = s.satisfiedCustomers;
    clients.insert(c1->custID);
    bool sortie = false;
//    if (SolverReduce::findSequence(s, clients))
//        return false;
    if (CDPSolver::ComputeCost(s, clients) < bestCost.satisfiedCost)
        return false;
//    cout << " try to relocate " << c1->custID << " near " << c2->custID << " " << s.CustomerString() << endl;
    Sol cur = s;
    cur.keyCustomers = clients;
    //    Data dat = s.GetData()->copyCustomersData(clients);
//    Prompt::print(Sol::CustomerConflict[c1->custID]);
//    Sol::CustomerConflict[c1->custID].clear();
//    exit(1);
//    CDPSolver::SolveInstance(cur,*s.GetData(),2);
    CDPSolver::BuildOnSolution(cur,*s.GetData(),1);

//    // cout<<" sol relocate 1:  "<<cur.CustomerString()<<endl;
//    SolverReduce::SolvedSequence[cur.satisfiedCustomers] =
//        SequenceInfo(true, cur.GetLastCost().satisfiedCost);
//    cur.keyCustomers = s.keyCustomers;
    if (cur.GetCost().satisfiedCost > s.GetCost().satisfiedCost)
    {
        s = cur;
        found = true;
        sortie = true;
//        cout << "best sol relocate 1) " << cur.CustomerString() << endl;
    }
    else{

//        if(!Sol::CustomerConflict[c1->custID].empty()){
//            c1->Show();
//            Prompt::print(Sol::CustomerConflict[c1->custID]);
//           for(auto conflict_id:Sol::CustomerConflict[c1->custID]){
//               Customer *c2= cur.GetCustomer(conflict_id);
//               int late= c2->late_tw;
//               c2->late_tw = late-16;
////               cout<<*c2<<endl;
////               cur.ShowSchedule();
//               cur.PutAllCustomersToUnassigned();
//               Parameters::SHOW=true;
//               CDPSolver::SolveInstance(cur,*cur.GetData(),1);
//               Prompt::print(Sol::CustomerConflict[c1->custID]);
////               cur.ShowSchedule();
//               cur.ShowCustomer();
//               exit(1);
//           }
//            exit(1);
//        }
//        exit(1);
    }


    if (s.GetLastCost() < bestCost)
    {
        bestCost = s.GetLastCost();
    }

    return sortie;
}