
#ifndef CODE_SOLVER_H
#define CODE_SOLVER_H

#include "../Data.h"
#include "../Solution.h"
#include <vector>

class Solver {
public:
    Solver(Data *data):_data(data){
        TimeSlot::myData = *data;
    }
    void run();
    static void SolveInstance(Sol &s,Data &dat);
private:
    Data *_data;
};


#endif //CODE_SOLVER_H
