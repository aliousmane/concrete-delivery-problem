
#ifndef CODE_PARAMETERS_H
#define CODE_PARAMETERS_H

#define MAX_RANK 100000
#define INFINI 9999999
#define EPS 10e-3

#include <chrono>
#include <random>

class Parameters {
public:
    static int NB_MANDATORY_ARGS;
    static int MAX_LATE_TW;
    static int LOADING_DURATION;
    static int TW_WIDTH;
    static int UNDELIVERY_PENALTY;
    static int FIRST_DEL_PENALTY ;
    static int LATE_ARRIVAL_PENALTY;
    static int OVERTIME_PENALTY;
    static int MIN_WORKING_TIME;
    static int NORMAL_WORKING_TIME;
    static int ADJUSTMENT_DURATION;
    static int MAX_WORKING_TIME;
    static int UNLOADING_RATE;
    static std::mt19937 RANDOM_GEN;
    static int TIME_BTW_DELIVERY;
    static int CLEANING_DURATION ;
    static bool PENALTY_COST;
    static bool KINABLE;
    static bool SHOW;
    static int SORT_TYPE;
    enum TypeNode
    {
        NODE,
        DEPOT,
        CUSTOMER,
        START_LINK,
        END_LINK,
        DELIVERY,
        DOCK
    };
    enum FAILURECAUSE{
        NONE,
        DELAY,
        LATETW,
        DRIVERBUSY,
        DEPOTBUSY
    };
    enum SORT{
        ONE,
        TWO,
        THREE,
        FOUR
    };

};


#endif //CODE_PARAMETERS_H
