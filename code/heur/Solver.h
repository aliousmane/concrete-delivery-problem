
#ifndef CODE_SOLVER_H
#define CODE_SOLVER_H

#include "../Data.h"
#include "../Solution.h"
#include "../Prompt.h"
#include "../Combinatorial.h"
#include "../InsertOperator.h"
#include "../BestSolutionList.h"
#include "RechercheLocale.h"
#include "../GRASP.h"
#include <vector>
#include <set>
#include <unordered_map>


class Solver {
public:
    explicit Solver(Data *data):_data(data){
        Sol::FailureCause.resize(_data->GetNodeCount(), Parameters::FAILURECAUSE::NONE);
        Sol::FailureCount.resize(_data->GetNodeCount(), 0);
        Sol::StartBefore.resize(_data->GetNodeCount(), 0);
        Sol::pullVisit.resize(_data->GetNodeCount(), 0);
        Sol::pushVisit.resize(_data->GetNodeCount(), 0);
        Sol::TabuFleet.resize(_data->GetNodeCount());
        CDPSolver::disjointClients.resize(_data->GetCustomerCount(), std::set < int > ());
        Sol::minDelay.resize(_data->GetNodeCount(), 0);
        TimeSlot::myData = *_data;
    }
    void run();
    static void SolveGrasp(Sol &s,Data &dat,std::vector<std::set<int>> const & linkedClientSlot);
    std::set<int> feasibleClients;

private:
    void Test();
    void Test(Sol &s);
    bool TabuDriverNode(Sol &s,Customer *c,Delivery *del,Driver *d);
    bool TabuDriverSolution(Sol &s,Customer *c,Driver *d);
    void Test(Sol &s,Customer *c,std::vector<std::set<int>> const & linkedClient);
    void run2(Sol &s,const std::set<int>& feasible);
    void TestRecursive(Sol &cur, Delivery *del,int cap);
    void TestRecursive2(Sol &cur, Delivery *del,std::unordered_map<std::string, Sol, MyHashFunction> * umap);
    Data *_data;
};


#endif //CODE_SOLVER_H
