
#ifndef CODE_SOLVER_H
#define CODE_SOLVER_H

#include "../Data.h"
#include "../Solution.h"
#include "../Prompt.h"
#include "../Combinatorial.h"
#include "../InsertOperator.h"
#include "../BestSolutionList.h"
#include "AllInsertionOperator.h"
#include "CustInsertionOperator.h"
#include "PriorityQueueOperator.h"
#include "RechercheLocale.h"
#include "../GRASP.h"
#include <vector>
#include <set>
#include <unordered_map>


class Solver {
public:
    explicit Solver(Data *data):_data(data){
        CDPSolver::disjointClients.resize(_data->GetCustomerCount(), std::set < int > ());
    }
    void run();
    static void SolveGrasp(Sol &s,Data &dat,std::vector<std::set<int>> const & linkedClientSlot,int iter);
    std::set<int> feasibleClients;

private:
    void Test();
    void TestTimeSlot();
    void Test(Sol &s);
    void SaveResults(Sol &s);
    Data *_data;

};


#endif //CODE_SOLVER_H
