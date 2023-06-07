#include <iostream>
#include "../Data.h"
#include "Solver.h"

using namespace std;

int main(int argc, const char **argv) {
    std::cout << "Ciment Quebec" << std::endl;
    if (argc < Parameters::NB_MANDATORY_ARGS) {
        Prompt::ShowMissingArgument(argc);
        exit(1);
    }
    Data data;
    data.GetParams(argc, argv);
    cout << data.input << endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    data.Load();
    Solver solver(&data);
    solver.run();
    Parameters::ShowTime();
    return 0;
}
