#include "Cost.h"
#include "Parameters.h"
#include <sstream>


using namespace std;

Cost::Cost(bool value)
        : Cost() {
    if (not value) {
        totalCost = (double) std::numeric_limits<time_t>::max();
        waitingCost = (double) std::numeric_limits<time_t>::max();
        travelCost = (double) std::numeric_limits<time_t>::max();
        distanceCost = (double) std::numeric_limits<time_t>::max();
        undeliveredCost = (double) std::numeric_limits<time_t>::max();
    }
}

bool Cost::operator<(const Cost &rhs) const {
    if (this->isFeasible and not rhs.isFeasible)
        return true;
    if (!this->isFeasible)
        return false;
    if (Parameters::KINABLE)
        return ObtainMinCDP(rhs);
    else
        return ObtainMinCQ(rhs);
}

bool Cost::ObtainMinCDP(const Cost &rhs) const {
    if (-this->satisfiedCost < -rhs.satisfiedCost) {
        return true;
    } else if (this->satisfiedCost == rhs.satisfiedCost) {
        if (this->driverUsed < rhs.driverUsed)
            return true;
        if (this->travelCost < rhs.travelCost)
            return true;
        return (this->waste < rhs.waste);

    }
    return false;
}

bool Cost::ObtainMinCQ(const Cost &rhs) const {

    if (this->undeliveredCost < rhs.undeliveredCost)
        return true;
    if (this->undeliveredCost == rhs.undeliveredCost) {

        if (this->firstDeliveryCost < rhs.firstDeliveryCost) {
            return true;
        } else if (this->firstDeliveryCost == rhs.firstDeliveryCost) {
            if (true) {
                if (this->travelCost < rhs.travelCost)
                    return true;
                else if (this->travelCost == rhs.travelCost) {
                    return (this->waitingCost < rhs.waitingCost);
                }
                return false;
            }
            if (false) {
                if (this->distanceCost < rhs.distanceCost)
                    return true;
                else if (this->distanceCost == rhs.distanceCost) {
                    return (this->waitingCost < rhs.waitingCost);
                }
                return false;
            }

            if (false) {
                if (this->overTimeCost < rhs.overTimeCost) {
                    return true;
                } else if (this->overTimeCost == rhs.overTimeCost) {

                    if (this->underWorkCost < rhs.underWorkCost) {
                        return true;
                    } else if (this->underWorkCost == rhs.underWorkCost) {
                        return (this->travelCost < rhs.travelCost);
                    }
                }
            }
        }
    }
    return false;
}

bool Cost::operator>(const Cost &rhs) const {
    return rhs < *this;
}

bool Cost::operator<=(const Cost &rhs) const {
    return !(rhs < *this);
}

bool Cost::operator>=(const Cost &rhs) const {
    return !(*this < rhs);
}

std::ostream &operator<<(std::ostream &os, const Cost &cost) {
    os << " satisfiedCost: " << cost.satisfiedCost <<
       " travelCost: " << cost.travelCost <<
       " distanceCost: " << cost.distanceCost <<
       " waitingCost: "
       << cost.waitingCost << " undeliveredCost: "
       << cost.undeliveredCost << " clientWaitingCost: " << cost.clientWaitingCost << " driverUsed: " << cost.driverUsed
       << " firstDelivery: " << cost.firstDeliveryCost << " underTime: "
       << cost.underWorkCost << " overTime: "
       << cost.overTimeCost << " Total: " << cost.totalCost;
    return os;
}

std::string Cost::str() const {
    std::stringstream ss;
    ss << satisfiedCost << ";" << undeliveredCost << ";" << travelCost << ";" << distanceCost << ";"
       << firstDeliveryCost << ";";
    ss << clientWaitingCost << ";" << driverUsed << ";" << underWorkCost << ";" << overTimeCost;
    return ss.str();
}