
#ifndef CODE_PARAMETERS_H
#define CODE_PARAMETERS_H

#define MAX_RANK 100000
#define INFINI 99999999
#define EPS 10e-3

#include <chrono>
#include <random>
#include <iostream>

class Parameters {
public:
    static int NB_MANDATORY_ARGS;
    static long RUNTIME;
    static int MAX_LATE_TW;
    static int LOADING_DURATION;
    static int TW_WIDTH;
    static int UNDELIVERY_PENALTY;
    static int FIRST_DEL_PENALTY;
    static int LATE_ARRIVAL_PENALTY;
    static int OVERTIME_PENALTY;
    static int UNDERWORK_PENALTY;
    static int MIN_WORKING_TIME;
    static int NORMAL_WORKING_TIME;
    static int ADJUSTMENT_DURATION;
    static int MAX_WORKING_TIME;
    static double UNLOADING_RATE;
    static std::mt19937 RANDOM_GEN;
    static int INTRA_ORDER_DELIVERY;
    static int INTER_ORDER_DELIVERY;
    static int LOAD_INSERTION;

    static int CLEANING_DURATION;
    static bool PENALTY_COST;
    static bool KINABLE;
    static bool SHOW;
    static int DRIVER_USE;
    static int SORT_TYPE;
    static std::chrono::time_point<std::chrono::steady_clock> START_TIME;
    static double MAX_TRAVEL_TIME;
    static int BACKTRACK_DEPTH;

    static void ShowTime() {
        auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - Parameters::START_TIME).count();
        std::cout << "\nTemps " << elapsed_time / 1000.0 << " s " << (elapsed_time / 1000.0) / 60 << " min"
                  << std::endl;
    }

    static long GetElapsedTime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()
                                                                     - Parameters::START_TIME).count();
    }

    enum TypeNode {
        NODE,
        DEPOT,
        CUSTOMER,
        START_LINK,
        END_LINK,
        DELIVERY,
        DOCK
    };
    enum FAILURECAUSE {
        NONE = -1,
        DELAY,
        LATETW,
        DRIVERBUSY,
        DEPOTBUSY,
        PUSH_PREC_CUSTOMER
    };
    enum SORT {
        ONE = 1,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SHUFFLE
    };
    enum MINIMIZEDRIVER {
        SOLUTION,
        CLIENT,
        HYBRID
    };
    enum DEPOTINSERTION {
        FORWARD,
        BACKWARD,
        RANDOM
    };

};


#endif //CODE_PARAMETERS_H
