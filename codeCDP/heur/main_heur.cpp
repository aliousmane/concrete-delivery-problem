#include <iostream>
#include "../Data.h"
#include "../UbManager.h"
#include "Solver.h"

using namespace std;

int main(int argc, const char **argv) {
    if (argc < Parameters::NB_MANDATORY_ARGS) {
        Prompt::ShowMissingArgument(argc);
        exit(1);
    }
    Data data;
    data.GetParams(argc, argv);
    Prompt::print({data.problem_name ,data.input}) ;
    data.Load();
    UbManager::Load("../../instances/cdp/CDP_UB.csv");
    data.UpperBound = UbManager::bounds[data.instance_name];
    Solver solver(&data);
    auto start_time = std::chrono::high_resolution_clock::now();
    solver.run();
    Parameters::ShowTime();
    return 0;
}
