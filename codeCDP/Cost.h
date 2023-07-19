
#ifndef CODE_COST_H
#define CODE_COST_H


#include <ostream>

class Cost {
public:

    Cost(): travelCost(0), waitingCost(0), undeliveredCost(0), totalCost(0),
              truckWaitingCost(0), clientWaitingCost(0), driverUsed(0),
              firstDeliveryCost(0), lateDeliveryCost(0), overTimeCost(0),
              firstDeliveryCount(0), lateDeliveryCount(0), satisfiedCost(0),
              isFeasible(true),waste(0) {}
    explicit Cost(bool value);

    bool operator==(const Cost &rhs) const {
        return travelCost == rhs.travelCost &&
               satisfiedCost == rhs.satisfiedCost &&
               waitingCost == rhs.waitingCost &&
               undeliveredCost == rhs.undeliveredCost &&
               totalCost == rhs.totalCost &&
               truckWaitingCost == rhs.truckWaitingCost &&
               clientWaitingCost == rhs.clientWaitingCost &&
               firstDeliveryCost == rhs.firstDeliveryCost &&
               firstDeliveryCount == rhs.firstDeliveryCount &&
               lateDeliveryCost == rhs.lateDeliveryCost &&
               lateDeliveryCount == rhs.lateDeliveryCount &&
               overTimeCost == rhs.overTimeCost &&
               driverUsed == rhs.driverUsed &&
               waste == rhs.waste &&
               isFeasible == rhs.isFeasible;
    }

    bool operator!=(const Cost &rhs) const {
        return !(rhs == *this);
    }

    bool operator<(const Cost &rhs) const;

    bool operator>(const Cost &rhs) const;

    bool operator<=(const Cost &rhs) const;

    bool operator>=(const Cost &rhs) const;
    Cost &operator+=(const Cost &c) {
        this->travelCost += c.travelCost;
        this->waitingCost += c.waitingCost;
        this->undeliveredCost += c.undeliveredCost;
        this->truckWaitingCost += c.truckWaitingCost;
        this->clientWaitingCost += c.clientWaitingCost;
        this->driverUsed += c.driverUsed;
        this->firstDeliveryCost += c.firstDeliveryCost;
        this->lateDeliveryCost += c.lateDeliveryCost;
        this->overTimeCost += c.overTimeCost;
        this->firstDeliveryCount += c.firstDeliveryCount;
        this->lateDeliveryCount += c.lateDeliveryCount;
        this->satisfiedCost += c.satisfiedCost;
        this->totalCost += c.totalCost;
        this->isFeasible = c.isFeasible;
        this->waste = c.waste;
        return *this;
    }
    double getTotal() const {
        return travelCost + waitingCost + undeliveredCost + firstDeliveryCost +
               lateDeliveryCost + overTimeCost;
    }
    void setTotal() {
        totalCost = travelCost + waitingCost + undeliveredCost + firstDeliveryCost +
                    lateDeliveryCost + overTimeCost;
    }

    friend std::ostream &operator<<(std::ostream &os, const Cost &cost);
    void Init(){
        travelCost=0; waitingCost=0; undeliveredCost=0; totalCost=0;
                truckWaitingCost=0; clientWaitingCost=0; driverUsed=0;
                firstDeliveryCost=0; lateDeliveryCost=0; overTimeCost=0;
                firstDeliveryCount=0; lateDeliveryCount=0; satisfiedCost=0;waste=0;
    }
    double travelCost;
    double satisfiedCost;
    int waitingCost;
    int undeliveredCost;
    double totalCost;
    int truckWaitingCost;
    int clientWaitingCost;
    int firstDeliveryCost;
    int firstDeliveryCount;
    int lateDeliveryCost;
    int lateDeliveryCount;
    int overTimeCost;
    int driverUsed;
    bool isFeasible;
    int waste;
private:
    bool ObtainMinCDP(const Cost &rhs)const;
    bool ObtainMinCQ(const Cost &rhs)const;
};


#endif //CODE_COST_H
