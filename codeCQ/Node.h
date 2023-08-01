
#ifndef CODE_NODE_H
#define CODE_NODE_H


#include <ostream>
#include <iostream>
#include "Parameters.h"

class Node {
public:
    int id, no, nodeID, type, depotID,siteID;
    double x, y;
    double demand;
    int distID;
    int early_tw, late_tw;
    int StartNodeID, EndNodeID;
    char c;

    Node() : id(-1), no(-1), nodeID(-1), x(0), y(0), early_tw(0), late_tw(0), StartNodeID(-1), EndNodeID(-1),
             distID(0), depotID(-1), demand(0), type(Parameters::NODE), c('N'),siteID(-1) {}

    virtual ~Node() = default;

    friend std::ostream &operator<<(std::ostream &os, const Node &node) {
        os << "id: " << node.id << " no: " << node.no << "["
           << node.early_tw << " - " << node.late_tw << " ] Depot"<<node.depotID;
        return os;
    }

    void SetTW(int a, int b) {
        early_tw = a;
        late_tw = b;
    }

    virtual void Show() const {
        std::cout << *this << std::endl;
    }
};

class Customer : public Node {
public:
    int custID, nbOrder, constID;
    long orderNbr;

    Customer() : custID(-1), orderNbr(-1), nbOrder(0), constID(-1) {}

    friend std::ostream &operator<<(std::ostream &os, const Customer &customer) {
        os << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
        os << "Customer " << customer.constID << " demand: " << customer.demand << " nbOrder:"<<customer.nbOrder  << " "
           << static_cast<const Node &>(customer) << "|\n";
        os << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
        return os;
    }

    void Show() const override {
        std::cout << *this << std::endl;
    }
};

class Depot : public Node {
public:
    int depotLoc;
    double capacity;

    Depot() : capacity(0), depotLoc(0) {}

    friend std::ostream &operator<<(std::ostream &os, const Depot &depot) {
        os << "Depot " << static_cast<const Node &>(depot) << " depotLoc: " << depot.depotLoc << " cap: "
           << depot.capacity;
        return os;
    }
};

class Dock : public Node {
public:
    int dockID, delID, orderID, custID, rank;
    double arrival_time, start_service, end_service, departure_time;

    Dock() : dockID(-1), arrival_time(0), start_service(0), end_service(0), departure_time(0), delID(-1), custID(-1),
             rank(-1), orderID(-1) {}

    friend std::ostream &operator<<(std::ostream &os, const Dock &dock) {
        os << static_cast<const Node &>(dock) << " dockID: " << dock.dockID << " depotID: " << dock.depotID
           << " arr_time: " << dock.arrival_time << " start: " << dock.start_service << " end: "
           << dock.end_service << " departure: " << dock.departure_time << " Del " << dock.delID
           <<" Cust: "<<dock.custID;
        return os;
    }
};

class Delivery : public Node {
public:
    int delID, orderID, rank, custID, dockID;
    double travel_time, distance;
    double arrival_time, start_service, end_service, departure_time;
    bool isdelayed;

    Delivery() : delID(-1), orderID(-1), arrival_time(0), start_service(0), end_service(0), departure_time(0),
                 rank(-1), custID(-1), dockID(-1), travel_time(0), isdelayed(false),distance(0) {}

    friend std::ostream &operator<<(std::ostream &os, const Delivery &delivery) {
        os << static_cast<const Node &>(delivery) << " delID: " << delivery.delID << " o: " << delivery.orderID << " c: " << delivery.custID
           << " Rank: " << delivery.rank << " load: " << delivery.demand << " arr_time: " << delivery.arrival_time
           << " start: "
           << delivery.start_service << " end: " << delivery.end_service << " departure: "
           << delivery.departure_time;
        return os;
    }
};

class Order {
public:
    int custID;
    int service_duration;
    int nbDelMin, nbDelMax;
    int orderID;
    double demand;
    int early_tw, late_tw;

    Order() : orderID(-1), demand(0), custID(-1), nbDelMin(0), nbDelMax(0), service_duration(0),
              early_tw(0), late_tw(0) {}

public:
    friend std::ostream &operator<<(std::ostream &os, const Order &order) {
        os << "ooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n";
        os << "Order: demand: " << order.demand << " custID: " << order.custID << " nbDelMin: " << order.nbDelMin
           << " nbDelMax: " << order.nbDelMax << " orderID: " << order.orderID << " [" << order.early_tw << "-"
           << order.late_tw << "]  |\n";
        os << "ooooooooooooooooooooooooooooooooooooooooooooooooooooooooo";
        return os;
    }
};

#endif //CODE_NODE_H
