
#include "SolverExact.h"
#include "gurobi/gurobiSolver.h"
#include "cplex/cplexSolver.h"

void SolverExact::run() {
    cplexSolver cplex_solver(_graph);
    cplex_solver.run();
}
