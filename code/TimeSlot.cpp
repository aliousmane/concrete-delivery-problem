
#include "TimeSlot.h"
#include "Parameters.h"


 Data TimeSlot::myData = Data();

bool TimeSlot::operator<(const TimeSlot &rhs) const {
    if (this->nodeID == -1 and rhs.nodeID == -1) {
        return (this->upper < rhs.lower);
    }
    if(*this == rhs) {
        if (this->nodeID != -1 and rhs.nodeID != -1) {
            if (this->n.type == Parameters::DOCK and rhs.n.type == Parameters::DOCK) {
                auto dock1 = dynamic_cast<Dock*>( TimeSlot::myData.GetNode(this->n.id));
                auto dock2 = dynamic_cast<Dock*>( TimeSlot::myData.GetNode(rhs.n.id));
                if (dock1->custID == dock2->custID) {
                    return (dock1->rank < dock2->rank);
                } else if (TimeSlot::myData.EarlyTW(dock1->custID) <= TimeSlot::myData.EarlyTW(dock2->custID)) {
                    return true;
                } else if (TimeSlot::myData.LateTW(dock1->custID) <= TimeSlot::myData.LateTW(dock2->custID)) {
                    return true;
                } else
                {
                    return (TimeSlot::myData.LateTW(dock1->custID) - TimeSlot::myData.EarlyTW(dock1->custID) <=
                            TimeSlot::myData.LateTW(dock2->custID)
                            - TimeSlot::myData.EarlyTW(dock2->custID));
                }
            }
            //TODO
//        if (this->n.isLoadingDock and rhs.n.isLoadingDock) {
//            if (this->n.custID == rhs.n.custID) {
//                return (this->n.rank < rhs.n.rank);
//            } else if (this->upper == rhs.lower) {
//                if(this->n.late_tw < rhs.n.late_tw){
//                    return true;
//                }
//                else if (this->n.early_tw < rhs.n.early_tw){
//                    return true;
//                }
//                return (this->n.id < rhs.n.id);
//            }
//        }
//        if (this->n.isDelivery and rhs.n.isDelivery) {
//            if (this->n.custID == rhs.n.custID) {
//                return (this->n.rank < rhs.n.rank);
//            }
//        }
        }
    }
    return (this->upper < rhs.lower);
}

bool TimeSlot::operator>(const TimeSlot &rhs) const {
    return (this->lower > rhs.upper);
}

bool TimeSlot::operator<=(const TimeSlot &rhs) const {
    return !(rhs < *this);
}

bool TimeSlot::operator>=(const TimeSlot &rhs) const {
    return  (this->lower >= rhs.upper);
}

bool TimeSlot::operator==(const TimeSlot &rhs) const {
    if(rhs.lower>=this->lower && rhs.lower<=this->upper)
        return true;
    if(this->lower>=rhs.lower && this->lower<=rhs.upper)
        return true;
    if(this->upper >= rhs.lower && this->upper<=rhs.upper)
        return true;
    if(rhs.upper >= this->lower && rhs.upper<=this->upper)
        return true;
    return false;
}

bool TimeSlot::operator!=(const TimeSlot &rhs) const {
    return !(rhs == *this);
}

std::ostream &operator<<(std::ostream &os, const TimeSlot &slot) {
    os <<  slot.typeNoeud <<slot.nodeID <<  ":[" << slot.lower << ":" << slot.upper << "]-";
    return os;
}
