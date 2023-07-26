
#ifndef CODE_DRIVER_H
#define CODE_DRIVER_H

#include "Parameters.h"
#include <ostream>

class Driver {
public:
    int id, StartNodeID, EndNodeID, depotID, distID, start_shift_time,
            end_shift_time, serviceDuration;
    long no, rank;
    std::string truck_nbr{};
    double capacity, sumDemand, shiftDuration, shiftDurationCost;
    double underWork{0}, overTime{0};

    Driver() : id(-1), no(-1), rank(0), StartNodeID(0), EndNodeID(-1), distID(-1), depotID(-1), capacity(0),
               sumDemand(0), start_shift_time(0), end_shift_time(0), shiftDuration(0), shiftDurationCost(0),
               serviceDuration(0) {}

    friend std::ostream &operator<<(std::ostream &os, const Driver &driver) {
        os << "Driver id: " << driver.id << " no: " << driver.no << " rank: " << driver.rank << " depotID: "
           << "[ " << driver.start_shift_time << "-"
           << driver.end_shift_time << "] capacity: " << driver.capacity
           << " Shift " << driver.shiftDuration <<" VS "<< Parameters::NORMAL_WORKING_TIME;
        return os;
    }

    void Init() {
        sumDemand = 0;
        shiftDuration = 0;
        underWork = 0;
        overTime = 0;
    }
};


#endif //CODE_DRIVER_H
