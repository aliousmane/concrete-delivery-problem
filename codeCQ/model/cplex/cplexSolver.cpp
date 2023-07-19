
#include "cplexSolver.h"

void cplexSolver::run() {
    createVariables();
    createObjective();
    createConstraints();
    cplex = IloCplex(model);
    setSettings();
    solveModel();

}
void cplexSolver::setSettings(){
    cplex.setParam(IloCplex::Param::Threads, 1);
    cplex.setParam(IloCplex::Param::TimeLimit, Parameters::RUNTIME);
    cplex.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 0);
    cplex.setParam(IloCplex::Param::Emphasis::MIP, 3);

}

