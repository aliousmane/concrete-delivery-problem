
#ifndef CODE_SOLVER_H
#define CODE_SOLVER_H

#include "../Data.h"
#include "../Solution.h"
#include "../Prompt.h"
#include "../InsertOperator.h"
#include "../BestSolutionList.h"
#include "RechercheLocale.h"
#include "../GRASP.h"
#include <vector>


class Solver {
public:
    explicit Solver(Data *data):_data(data){
        TimeSlot::myData = *data;
    }
    void run();
    void SolveGrasp(Sol &s,Data &dat,std::vector<std::set<int>> const & linkedClientSlot);

private:
    Data *_data;

};


#endif //CODE_SOLVER_H
