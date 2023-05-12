

#ifndef CODE_SOLUTION_H
#define CODE_SOLUTION_H
#include "Data.h"
#include "Cost.h"
#include "TimeSlot.h"
#include "Parameters.h"
#include "Driver.h"
#include "Node.h"
#include <vector>
#include <set>
#include <cassert>
#include <iostream>

class Sol {
public:
    Sol()=default;
    explicit Sol(Data *data);
    bool isFeasible{false};
    int iter{-1};
    double duration_time{0};
    Cost updateCost;
    std::vector<Driver *> DriverAssignTo;
    std::vector<Depot *> DepotAssignTo;
    std::vector<Node *> DriverNext;
    std::vector<Node *> DepotNext;
    std::vector<Node *> CustomerNext;
    std::vector<Node *> DriverPrev;
    std::vector<Node *> DepotPrev;
    std::vector<Node *> CustomerPrev;
    std::vector<Node *> Unassigned;

    std::vector<int> RoutesLength;
    std::vector<int> orderCapRestante;
    std::vector<int> clientCapRestante;
    std::vector<int>  shiftDuration;
    std::vector<int>  shiftDurationCost;
    std::vector<int>  DepotSize;
    std::vector<int>  ArrivalTime;
    std::vector<int>  StartServiceTime;
    std::vector<int>  EndServiceTime;
    std::vector<int>  ExpectedServiceTime;
    std::vector<int>  DepartureTime;
    std::vector<int>  DeliveryLoad;
    std::vector<int>  WaitingTime;
    std::vector<int>  UnassignedIndex;
    std::vector<bool> VisitFlags;
    std::vector<bool> VisitFlagCost;
    std::vector<std::vector<int>> DriverVisitCount;
    std::vector<std::set<TimeSlot>> depotLoadingIntervals;
    std::vector<std::set<TimeSlot>> driverWorkingIntervals;
    std::vector<TimeSlot> nodeServiceIntervals;
    std::set<int> keyCustomers;
    std::set<int> availableDrivers;
    std::set<int> driverUsed;
    std::vector<std::set<int>> clientDriverUsed;
    std::set<int> satisfiedCustomers;

    void InitCustomers();
    void InitOrders();
    void AssignStationToDriver();
    void AssignStationToDepots();
    void AssignStationToCustomers();

    void  PutAllCustomersToUnassigned();

    int GetCustomerCount()const {return _data->GetCustomerCount();}
    int GetOrderCount()const {return _data->GetOrderCount();}
    int GetNodeCount()const {return _data->GetNodeCount();}
    int GetDriverCount()const {return _data->GetDriverCount();}
    int GetDepotCount()const {return _data->GetDepotCount();}
    int GetDeliveryCount(Order *o)const{return _data->GetDeliveryCount(o);}
    int GetDeliveryCount()const{return _data->GetDeliveryCount();}
    Customer *GetCustomer(int i){return _data->GetCustomer(i);}
    Driver *GetDriver(int i){return _data->GetDriver(i);}
    Driver *GetDriverAssignedTo(Node *n)  {
        return DriverAssignTo[n->id];
    }
    Depot *GetDepotAssignedTo(Node *n)  {
        return DepotAssignTo[n->id];
    }
    Data * GetData(){return _data;}
    Node *GetNode(int i){return _data->GetNode(i);}
    Delivery *GetDelivery(Order *o,int index){return _data->GetDelivery(o,index);}
    Delivery *GetDelivery(int index){return _data->GetDelivery(index);}
    Depot *GetDepot(int index){return _data->GetDepot(index);}
    Dock * GetDock(int dockID){return _data->GetDock(dockID);}
    Order * GetOrder(int orderId){return _data->GetOrder(orderId);}
    std::vector<Order*> GetOrders(Customer *c){ return _data->GetOrders(c);}
    std::vector<Order*> GetOrders(int custId ){ return _data->GetOrders(GetCustomer(custId));}
    Order * GetRandomOrder(Customer *c){
        std::vector<Order*> temp(GetOrders(c));
        std::shuffle(temp.begin(), temp.end(), Parameters::RANDOM_GEN);
        Order *cur_order = nullptr;
        for (auto o1 : temp)
        {
            if (isOrderSatisfied(o1))
                continue;
            cur_order = o1;
            break;
        }
        assert(cur_order != nullptr);
        return cur_order;
    }
    void UnassignCustomer(Customer *c);
    void UnassignOrder(Order *o);
    void AddToUnassigneds(Customer *n);
    void UnassignDelivery(std::vector<Delivery*> const & delList);
    void RemoveFromCustomer(Delivery *del);
    void RemoveDelivery(Delivery *del);
    void RemoveDock(Dock *dock);
    void Remove(Node *n);
    void RemoveFromDepot(Dock *dock);
    void Update();
    void UpdateForward();
    void UpdateForward(Depot *dep);
    void Update(Depot *dep, Dock *dock,Delivery *del);
    Cost GetCost();
    Cost GetLastCost(){ return _last_cost;}
    void GetCost(Depot *dep, Cost &cur_cost);
    void GetCost(Depot *dep, Dock *dock,Delivery *del,Cost &cur_cost);
    Cost GetCost(Driver *d);
    void Show();
    void ShowDrivers();
    void Show(Driver *d);
    void ShowSchedule(Delivery *del);
    void ShowSchedule();
    void ShowSchedule(Order *o);
    void ShowCustomer();
    int EarlyTW(Delivery *n) const { return _data->EarlyTW(n); }
    int LateTW(Delivery *n) const { return _data->LateTW(n); }
    bool isOrderSatisfied(Order *o)const{return (orderCapRestante[o->orderID]<=0); }
    bool isOrderSatisfied(Delivery *del)const{return (orderCapRestante[del->orderID] <=0); }
    bool isOrderSatisfied(int ordId)const{return (orderCapRestante[ordId]<=0); }
    bool isClientSatisfied(Customer *c)const{return (clientCapRestante[c->custID]<=0); }
    bool isClientSatisfied(int custId)const{return (clientCapRestante[custId]<=0); }
    void InsertAfter(Node *n, Node *prev);
    void InsertAfter(Node *n, Node *prev,Driver *d);
    void AssignDeliveryToCustomer(Delivery *n);
    int Travel(Node *from, Node *to) {
        return  _data->Travel(from,to);
    }
    int Travel(Customer *from, Customer *to) {
        return  _data->Travel(from,to);
    }
    int Travel(int from, int to) {
        return  _data->Travel(from,to);
    }
    int GetLoad(Driver *d,Order *o){
        return std::min(d->capacity, orderCapRestante[o->orderID]);
    }
    void UpdateDemand(Customer *c, Order *o, int load){
        orderCapRestante[o->orderID] -= load;
        clientCapRestante[c->custID] -= load;
    }

    void UpdateDepotLoadingSet(Depot *dep, Dock *dock,TimeSlot const & intv);

    static double GetFirstDeliveryCost(Delivery *del, double arrival,
                                     double expected_arrival) {
        return Parameters::FIRST_DEL_PENALTY *
               std::max(0., arrival - expected_arrival);
    }
    static int GetFirstDeliveryCount(Delivery *del, double arrival,
                                   double expected_arrival) {
        return int(arrival > expected_arrival);
    }
    static double GetTruckWaitingCost(Delivery *del, double arrival,
                                    double expected_arrival) {
        return std::max(0., expected_arrival - arrival);
    }
    static int GetClientWaitingCost(Delivery *del, double arrival,
                                     double expected_arrival) {
        return std::max(0., arrival - expected_arrival);
    }
    static double GetLateDeliveryCost(Delivery *del, double arrival,
                                    double expected_arrival) {
        return Parameters::LATE_ARRIVAL_PENALTY *
               std::max(0., arrival - expected_arrival);
    }
    static int GetLateDeliveryCount(Delivery *del, double arrival,
                                  double expected_arrival) {
        return int(arrival > expected_arrival);
    }
    static void SetTimingCost(Delivery *del,  int arrival,
                            int expected_arrival,int earlytw, Cost &newcost) {

        if (del->rank ==0 ) {
            newcost.firstDeliveryCost +=
                    Sol::GetFirstDeliveryCost(del, arrival, earlytw);

            newcost.firstDeliveryCount +=
                    Sol::GetFirstDeliveryCount(del, arrival, earlytw);

            newcost.truckWaitingCost += Sol::GetTruckWaitingCost(del, arrival, earlytw);

            newcost.clientWaitingCost +=
                    Sol::GetClientWaitingCost(del, arrival, earlytw);
        }
        else {
            newcost.truckWaitingCost +=
                    Sol::GetTruckWaitingCost(del, arrival, expected_arrival);

            newcost.clientWaitingCost += Sol::GetClientWaitingCost(
                    del, arrival, expected_arrival + Parameters::TIME_BTW_DELIVERY);

            newcost.lateDeliveryCost += Sol::GetLateDeliveryCost(
                    del, arrival, expected_arrival + Parameters::TIME_BTW_DELIVERY);

            newcost.lateDeliveryCount += Sol::GetLateDeliveryCount(
                    del, arrival, expected_arrival + Parameters::TIME_BTW_DELIVERY);
        }
    }
    void ShowLoadingSlot(Depot *myDep);
    void BuildFromDepotSetIntervall();
    void BuildFromDepotSetIntervall(Depot *depot);
    void InsertAfterDepot(Node *n, Node *prev, Node *dep);

    bool operator<(const Sol &rhs) const;

    static std::vector<int> FailureCause;
    static std::vector<int> FailureCount;
    static std::vector<int> minDelay;
    static std::vector<int> pullVisit;
    static std::vector<int> pushVisit;
    static std::vector<int> StartBefore;
    std::string CustomerString() ;
    ~Sol()=default;
    template<typename T>
    bool hasScheduled(  const T &  container){
        return (std::includes(satisfiedCustomers.begin(), satisfiedCustomers.end(),
                                  container.begin(), container.end()));
    }
    std::string toString() const;
    Order * findIdleOrder(Customer *c){
        std::vector<Order*> orders(GetOrders(c));
        std::shuffle(orders.begin(), orders.end(), Parameters::RANDOM_GEN);
        for (Order *cur_o: orders) {
            if (isOrderSatisfied(cur_o))
                continue;
            return cur_o;
        }
        return nullptr;
    }
    Delivery * GetNextIdleDelivery(Order *o) {

        for (int i = 0; i < GetDeliveryCount(o); i++) {
            Delivery *del = GetDelivery(o, i);
            if (DeliveryLoad[del->id] == 0)
                return del;
        }
        return nullptr;
    }
private:
    Data *_data{};
    Cost _last_cost;
    int UnassignedCount{};
};


#endif //CODE_SOLUTION_H
