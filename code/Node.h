
#ifndef CODE_NODE_H
#define CODE_NODE_H


#include <ostream>
#include <iostream>
#include "Parameters.h"

class Node {
public:
    int id,no,nodeID,type,depotID;
    double x,y;
    int demand;
    int distID;
    int early_tw,late_tw;
    int StartNodeID, EndNodeID;

    Node() : id(-1), no(-1),nodeID(-1), x(0), y(0), early_tw(0), late_tw(0),StartNodeID(-1),EndNodeID(-1),
    distID(0),depotID(-1),demand(0),type(Parameters::NODE)
    {}
    virtual ~Node()= default;
    friend std::ostream &operator<<(std::ostream &os, const Node &node) {
        os << "id: " << node.id << " no: " << node.no  << "["
           << node.early_tw << " - " << node.late_tw << " ]";
        return os;
    }
    void SetTW(int a,int b){
        early_tw=a;late_tw=b;
    }
};

class Customer :public Node{
public:
    int custID,nbOrder;
    long orderID;
    Customer():custID(-1),orderID(-1),nbOrder(0){}

    friend std::ostream &operator<<(std::ostream &os, const Customer &customer) {
        os<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n";
        os << "Customer "<< customer.custID << " demand: " << customer.demand  <<" "<<static_cast<const Node &>(customer) <<"|\n";
        os<<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
        return os;
    }

    ~Customer() override {
    }
};

class Depot:public Node{
public:
    int depotLoc;
    double capacity;
    Depot():capacity(0),depotLoc(0){}

    friend std::ostream &operator<<(std::ostream &os, const Depot &depot) {
        os << "Depot "<< static_cast<const Node &>(depot) << " depotLoc: " << depot.depotLoc<< " cap: " << depot.capacity;
        return os;
    }
};

class Dock:public Node{
public:
    int dockID,delID, custID,rank;
    double arrival_time,start_service,end_service, departure_time;
    Dock():dockID(-1),arrival_time(0),start_service(0),end_service(0), departure_time(0),delID(-1),custID(-1),rank(-1)
    {}

    friend std::ostream &operator<<(std::ostream &os, const Dock &dock) {
        os << static_cast<const Node &>(dock) << " dockID: " << dock.dockID << " depotID: " << dock.depotID
           << " arrival_time: " << dock.arrival_time << " start_service: " << dock.start_service << " end_service: "
           << dock.end_service << " departure_time: " << dock.departure_time;
        return os;
    }
};

class Delivery:public Node{
public:
    int delID,orderID,rank,custID,dockID;
    double travel_time;
    double arrival_time,start_service,end_service, departure_time;
    bool isdelayed;
    Delivery():delID(-1),orderID(-1),arrival_time(0),start_service(0),end_service(0), departure_time(0),
    rank(-1),custID(-1),dockID(-1),travel_time(0),isdelayed(false){}

    friend std::ostream &operator<<(std::ostream &os, const Delivery &delivery) {
        os << static_cast<const Node &>(delivery) << " delID: " << delivery.delID << " orderID: " << delivery.orderID
           << " load: " << delivery.demand << " arrival_time: " << delivery.arrival_time << " start_service: "
           << delivery.start_service << " end_service: " << delivery.end_service << " departure_time: "
           << delivery.departure_time;
        return os;
    }
};

class Order{
public:
    int custID;
    int service_duration;
    int nbDelMin, nbDelMax;
    int orderID;
    int demand;
    Order():orderID(-1),demand(0),custID(-1),nbDelMin(0),nbDelMax(0),service_duration(0){}

public:
    friend std::ostream &operator<<(std::ostream &os, const Order &order) {
        os<<"ooooooooooooooooooooooooooooooooooooooooooooooooooooooooo\n";
        os << "Order: demand: " << order.demand << " custID: " << order.custID << " nbDelMin: " << order.nbDelMin
           << " nbDelMax: " << order.nbDelMax << " orderID: " << order.orderID <<"  |\n";
        os<<"ooooooooooooooooooooooooooooooooooooooooooooooooooooooooo";
        return os;
    }
};

#endif //CODE_NODE_H
