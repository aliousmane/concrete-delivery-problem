#include "Cost.h"
#include "Parameters.h"
Cost::Cost(bool value)
        : travelCost(0), waitingCost(0), undeliveredCost(0), totalCost(0),
          truckWaitingCost(0), clientWaitingCost(0), driverUsed(0),
          firstDeliveryCost(0), lateDeliveryCost(0), overTimeCost(0),
          firstDeliveryCount(0), lateDeliveryCount(0), satisfiedCost(-1),isFeasible(value),waste(0) {
    if (not value) {
        totalCost = INFINI;
        waitingCost = INFINI;
        travelCost = INFINI;
        undeliveredCost = INFINI;
    }
}

bool Cost::operator<(const Cost &rhs) const {
    if (this->isFeasible and not rhs.isFeasible)
        return true;
    if (!this->isFeasible)
        return false;
    if(Parameters::KINABLE)
        return ObtainMinCDP(rhs);
    else
        return ObtainMinCQ(rhs);
}

bool Cost::ObtainMinCDP(const Cost &rhs)const{
    if (-this->satisfiedCost < -rhs.satisfiedCost) {
        return true;
    }
    else if(this->satisfiedCost == rhs.satisfiedCost){
        if(this->driverUsed<rhs.driverUsed)
            return true;
        if(this->travelCost < rhs.travelCost)
            return true;
        return (this->waste<rhs.waste);

    }
    return false;
}
bool Cost::ObtainMinCQ(const Cost &rhs)const{
    exit(1);
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
    os << "travelCost: " << cost.travelCost << " waitingCost: " << cost.waitingCost << " undeliveredCost: "
       << cost.undeliveredCost << " clientWaitingCost: " << cost.clientWaitingCost << " overTimeCost: "
       << cost.overTimeCost << " driverUsed: " << cost.driverUsed <<" satisfiedCost: " << cost.satisfiedCost;
    return os;
}
