#include <iostream>
#include "../Data.h"
#include "Solver.h"
#include "../model/SolverExact.h"

using namespace std;

int main(int argc, const char **argv) {
    if (argc < Parameters::NB_MANDATORY_ARGS) {
        Prompt::ShowMissingArgument(argc);
        exit(1);
    }
    Data data;
    data.GetParams(argc, argv);
    Prompt::print({data.problem_name ,data.input, data.path}) ;
    data.Load();

//    SolverExact solverEx(&data);
//    solverEx.run();

    Solver solver(&data);
    solver.run();
    Parameters::ShowTime();
    return 0;
}
