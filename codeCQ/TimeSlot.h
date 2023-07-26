

#ifndef CODE_TIMESLOT_H
#define CODE_TIMESLOT_H

#include <ostream>
#include <cassert>
#include "Node.h"
#include "Data.h"
class TimeSlot {
public:
    TimeSlot() : lower(0), upper(0), nodeID(-1), typeNoeud('N'), n() {}
    TimeSlot(double a, double b)
            : lower(a), upper(b), nodeID(-1), typeNoeud('N'), n(){
        assert(a >= 0);
        assert(b >= 0);
    }
    explicit TimeSlot(double a)
            : lower(a), upper(a), nodeID(-1), typeNoeud('N'), n(){
        assert(a >= 0);
    }
    explicit TimeSlot(Node const&n)
            : lower(n.early_tw), upper(n.late_tw), nodeID(n.id), typeNoeud('N'), n(n){
    }
    TimeSlot(double a, double b,Node & _n)
            : lower(a), upper(b), nodeID(_n.id), typeNoeud('N'), n(_n){
        assert(a >= 0);
        assert(b >= 0);
    }
    TimeSlot(double a, double b,int id)
            : lower(a), upper(b), nodeID(id), typeNoeud('N'), n() {
        assert(a >= 0);
        assert(b >= 0);
    }
    TimeSlot(double a, double b,  char c,Node &_n)
            : lower(a), upper(b), nodeID(_n.id), typeNoeud(c), n(_n) {
        assert(a >= 0);
        assert(b >= 0);
    }

    bool operator==(const TimeSlot &rhs) const;

    bool operator!=(const TimeSlot &rhs) const;

    bool operator<(const TimeSlot &rhs) const;

    bool operator>(const TimeSlot &rhs) const;

    bool operator<=(const TimeSlot &rhs) const;

    bool operator>=(const TimeSlot &rhs) const;

    static Data myData;

    friend std::ostream &operator<<(std::ostream &os, const TimeSlot &slot);

    double lower, upper;
    int nodeID;
    char typeNoeud;
    Node n;
};


#endif //CODE_TIMESLOT_H
