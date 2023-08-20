
#include "Parameters.h"

int Parameters::NB_MANDATORY_ARGS = 2;
long Parameters::RUNTIME = 300;
int Parameters::MAX_LATE_TW = 1600;//1440;
int Parameters::TW_WIDTH = 60;
int Parameters::MAX_WORKING_TIME = 600;
int Parameters::MAX_OVERTIME = 120;//120;

double Parameters::UNLOADING_RATE = 30; //30 m3/h

std::mt19937 Parameters::RANDOM_GEN =
        std::mt19937(std::chrono::system_clock::now().time_since_epoch().count());

bool Parameters::PENALTY_COST = false;
bool Parameters::KINABLE = false;
bool Parameters::SHOW = false;
int Parameters::DRIVER_USE = Parameters::MINIMIZEDRIVER::SOLUTION;
int Parameters::SORT_TYPE = Parameters::SORT::TWO;
std::chrono::time_point<std::chrono::steady_clock> Parameters::START_TIME = std::chrono::steady_clock::now();


int Parameters::INTRA_ORDER_DELIVERY = 20;
int Parameters::INTER_ORDER_DELIVERY = 25;
int Parameters::MIN_WORKING_TIME = 180;
int Parameters::NORMAL_WORKING_TIME = 480;
double Parameters::MAX_TRAVEL_TIME = 120;

int Parameters::ADJUSTMENT_DURATION = 10;
int Parameters::CLEANING_DURATION = 10;
int Parameters::BACKTRACK_DEPTH = 150;

int Parameters::UNDELIVERY_PENALTY = 1;
int Parameters::FIRST_DEL_PENALTY = 1;//;1000;
int Parameters::UNDERWORK_PENALTY = 1;//;100;
int Parameters::OVERTIME_PENALTY = 1;//200;
int Parameters::LATE_ARRIVAL_PENALTY = 10;
int Parameters::FIRST_DEL_MAX_ARRIVAL = 60;

int Parameters::LOAD_INSERTION = Parameters::DEPOTINSERTION::FORWARD;

bool Parameters::LOCAL_SEARCH = true;
int Parameters::ITERATION = 100;
