
#include "Parameters.h"

 int Parameters::NB_MANDATORY_ARGS=2;
 int Parameters::MAX_LATE_TW=1440;
 int Parameters::TW_WIDTH=60;
 int Parameters::UNDELIVERY_PENALTY=10;
 int Parameters::FIRST_DEL_PENALTY = 10;
 int Parameters::LATE_ARRIVAL_PENALTY=10;
 int Parameters::OVERTIME_PENALTY=10;
 int Parameters::MIN_WORKING_TIME=180;
 int Parameters::NORMAL_WORKING_TIME=480;
 int Parameters::MAX_WORKING_TIME=600;
 int Parameters::ADJUSTMENT_DURATION=10;
int Parameters::CLEANING_DURATION = 5;

int Parameters::UNLOADING_RATE=10;

std::mt19937 Parameters::RANDOM_GEN =
        std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());

int Parameters::LOADING_DURATION=5;
int Parameters::TIME_BTW_DELIVERY=5;
bool Parameters::PENALTY_COST = false;
bool Parameters::KINABLE= true;
bool Parameters::SHOW = false;
int Parameters::SORT_TYPE= Parameters::SORT::TWO;


