
#ifndef CODE_SOLVEREXACT_CQ_H
#define CODE_SOLVEREXACT_CQ_H

#include "../Data.h"
#include "Graph.h"

class SolverExact {
public:
    SolverExact(Data *data):_data(data),_graph(_data){}
    void run();
private:
    Data *_data;
    Graph _graph;
};


#endif //CODE_SOLVEREXACT_CQ_H
