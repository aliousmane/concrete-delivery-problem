

#ifndef CODE_SOLUTION_H
#define CODE_SOLUTION_H

#include "Data.h"
#include "Cost.h"
#include "TimeSlot.h"
#include "Parameters.h"
#include "Driver.h"
#include "Node.h"
#include <utility>
#include <vector>
#include <set>
#include <cassert>
#include <iostream>


class Sol {
public:
    Sol() = default;

    explicit Sol(Data *data);
    explicit Sol(Data *data, std::set<int> const & usedCustomers):Sol(data){
        this->keyCustomers = usedCustomers;
    }

    bool isFeasible{false};
    int iter{-1};
    double duration_time{0};
    Cost updateCost;
    bool computeLastCost{false};
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
    std::vector<int> shiftDuration;
    std::vector<int> shiftDurationCost;
    std::vector<int> DepotSize;
    std::vector<int> ArrivalTime;
    std::vector<int> StartServiceTime;
    std::vector<int> EndServiceTime;
    std::vector<int> ExpectedServiceTime;
    std::vector<int> DepartureTime;
    std::vector<int> DeliveryLoad; // key: delID
    std::vector<int> WaitingTime;
    std::vector<int> ClientWaitingTime;
    std::vector<int> TruckWaitingTime;
    std::vector<int> OrderLateDelivery;
    std::vector<int> NodeLateDelivery;
    std::vector<int> UnassignedIndex;
    std::vector<int> OrderVisitCount;
    std::vector<int> sumServiceTime;
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
    std::set<int> unscheduledCustomers;


    static std::vector<int> FailureCause;
    static std::vector<std::set<int>> CustomerConflict;
    static std::vector<std::set<int>> TabuFleet;
    static std::vector<int> FailureCount;
    static std::vector<int> minDelay;
    static std::vector<int> pullVisit;
    static std::vector<int> pushVisit;
    static std::vector<int> StartBefore;
    static std::map<std::tuple<int, int>, int> nodeMaxStartService;

    void InitCustomers();

    void InitCustomer(Customer *c);

    void InitDepots();

    void InitDepot(Depot *dep);

    void InitDrivers();

    void InitDriver(Driver *d);

    void InitOrders();

    void InitOrder(Order *o);

    void AssignStationToDriver();

    void AssignStationToDepots();

    void AssignStationToCustomers();

    void PutAllCustomersToUnassigned();

    int GetCustomerCount() const { return _data->GetCustomerCount(); }

    int GetOrderCount() const { return _data->GetOrderCount(); }

    int GetNodeCount() const { return _data->GetNodeCount(); }

    int GetDriverCount() const { return _data->GetDriverCount(); }

    int GetDepotCount() const { return _data->GetDepotCount(); }

    int GetDeliveryCount(Order *o) const { return _data->GetDeliveryCount(o); }

    int GetDeliveryCount() const { return _data->GetDeliveryCount(); }

    Customer *GetCustomer(int i) { return _data->GetCustomer(i); }

    Driver *GetDriver(int i) { return _data->GetDriver(i); }

    Driver *GetDriverAssignedTo(Node *n) {
        return DriverAssignTo[n->id];
    }

    Depot *GetDepotAssignedTo(Node *n) {
        return DepotAssignTo[n->id];
    }

    Data *GetData() { return _data; }

    Node *GetNode(int i) { return _data->GetNode(i); }

    Delivery *GetDelivery(Order *o, int index) { return _data->GetDelivery(o, index); }

    Delivery *GetDelivery(int orderID, int index) { return _data->GetDelivery(GetOrder(orderID), index); }

    std::vector<Delivery *> GetDeliveries(Customer *c) {
        std::vector<Delivery *> temp;
        for (auto o: GetOrders(c)) {
            std::vector<Delivery *> temp2 = GetDeliveries(o);
            temp.insert(temp.end(), temp2.begin(), temp2.end());
        }
        return temp;
    }

    std::vector<Delivery *> GetDeliveries(Order *o) {
        std::vector<Delivery *> temp(GetDeliveryCount(o));
        for (int i = 0; i < GetDeliveryCount(o); i++) {
            temp[i] = GetDelivery(o, i);
        }
        return temp;
    }

    std::vector<Delivery *> GetDeliveries(Order *o, int imin, int imax) {
        std::vector<Delivery *> temp(std::min(imax, GetDeliveryCount(o) - 1) - imin + 1);
        int id = 0;
        for (int i = imin; i <= std::min(imax, GetDeliveryCount(o) - 1); i++) {
            temp[id++] = GetDelivery(o, i);
        }
        return temp;
    }

    Delivery *GetDelivery(int index) { return _data->GetDelivery(index); }

    Depot *GetDepot(int index) { return _data->GetDepot(index); }

    Depot *GetDepot(Node *n) { return _data->GetDepot(n); }

    Dock *GetDock(int dockID) { return _data->GetDock(dockID); }

    Order *GetOrder(int orderId) { return _data->GetOrder(orderId); }

    std::vector<Order *> GetOrders(Customer *c) { return _data->GetOrders(c); }

    std::vector<Order *> GetOrders(int custId) { return _data->GetOrders(GetCustomer(custId)); }

    Order *GetRandomOrder(Customer *c) {
        std::vector<Order *> temp(GetOrders(c));
        std::shuffle(temp.begin(), temp.end(), Parameters::RANDOM_GEN);
        Order *cur_order = nullptr;
        for (auto o1: temp) {
            if (isOrderSatisfied(o1))
                continue;
            cur_order = o1;
            break;
        }
        assert(cur_order != nullptr);
        return cur_order;
    }

    void UnassignCustomer(Customer *c);

    void UnassignCustomer(int custID) {
        assert(custID < GetCustomerCount());
        UnassignCustomer(GetCustomer(custID));
    }

    void UnassignOrder(Order *o);

    void AddToUnassigneds(Customer *n);

    void UnassignDelivery(std::vector<Delivery *> const &delList);

    void RemoveFromCustomer(Delivery *del);

    void RemoveDelivery(Delivery *del);

    void RemoveDock(Dock *dock);

    void Remove(Node *n);

    void RemoveFromDepot(Dock *dock);

    void Update();

    void Update(Customer *c);

    void Update(Order *o);

    void UpdateForward();

    void UpdateForward(Depot *dep);

    void Update(Depot *dep, Dock *dock, Delivery *del);

    Cost GetCost();

    Cost GetLastCost() {
        if (!computeLastCost) {
            return GetCost();
        }
        return _last_cost;
    }

    void GetCost(Depot *dep, Cost &cur_cost);

    void GetCost(Depot *dep, Dock *dock, Delivery *del, Cost &cur_cost);

    Cost GetCost(Driver *d);

    void Show();

    void ShowDrivers();

    void Show(Driver *d);

    void ShowSchedule(Delivery *del);

    void ShowSchedule(Node *n) {
        auto n1 = dynamic_cast<Delivery *>(n);
        if (n1 != nullptr) {
            ShowSchedule(n1);
        } else {
            auto n2 = dynamic_cast<Customer *>(n);
            if (n2 != nullptr) {
                ShowSchedule(n2);
            }
        }
    }

    void ShowSchedule();

    void ShowSchedule(Order *o);

    void ShowSchedule(Customer *c);

    void ShowCustomer();

    int EarlyTW(Delivery *n) const { return _data->EarlyTW(n); }

    int LateTW(Delivery *n) const { return _data->LateTW(n); }

    bool isOrderSatisfied(Order *o) const { return (orderCapRestante[o->orderID] <= 0); }

    bool isSatisfied(Order *o) const { return (orderCapRestante[o->orderID] <= 0); }

    bool isOrderSatisfied(Delivery *del) const { return (orderCapRestante[del->orderID] <= 0); }

    bool isOrderSatisfied(int ordId) const { return (orderCapRestante[ordId] <= 0); }

    bool isClientSatisfied(Customer *c) const { return (clientCapRestante[c->custID] <= 0); }

    bool isSatisfied(Customer *c) const { return (clientCapRestante[c->custID] <= 0); }

    bool isClientSatisfied(int custId) const { return (clientCapRestante[custId] <= 0); }

    void InsertAfter(Node *n, Node *prev);

    void InsertAfter(Node *n, Node *prev, Driver *d);

    void AssignDeliveryToCustomer(Delivery *n);

    int Travel(Node *from, Node *to) {
        return _data->Travel(from, to);
    }

    int Travel(Customer *from, Customer *to) {
        return _data->Travel(from, to);
    }

    int Travel(int from, int to) {
        return _data->Travel(from, to);
    }

    int GetLoad(Driver *d, Order *o) {
        return std::min(d->capacity, orderCapRestante[o->orderID]);
    }

    void UpdateDemand(Customer *c, Order *o, int load) {
        orderCapRestante[o->orderID] -= load;
        clientCapRestante[c->custID] -= load;
    }

    void UpdateDepotLoadingSet(Depot *dep, Dock *dock, TimeSlot const &intv);

    static double GetFirstDeliveryCost(Delivery *del, double arrival,
                                       double expected_arrival) {
        return Parameters::FIRST_DEL_PENALTY *
               std::max(0., arrival - expected_arrival);
    }

    static int GetFirstDeliveryCount(Delivery *del, double arrival,
                                     double expected_arrival) {
        return int(arrival > expected_arrival);
    }

    static int GetTruckWaitingCost(Delivery *del, double arrival,
                                   double expected_arrival) {
        return int(std::max(0., expected_arrival - arrival));
    }

    static int GetClientWaitingCost(Delivery *del, double arrival,
                                    double expected_arrival) {
        return int(std::max(0., arrival - expected_arrival));
    }

    static int GetLateDeliveryCost(Delivery *del, double arrival,
                                   double expected_arrival) {
        return int(Parameters::LATE_ARRIVAL_PENALTY *
                   std::max(0., arrival - expected_arrival));
    }

    static int GetLateDeliveryCount(Delivery *del, double arrival,
                                    double expected_arrival) {
        return int(arrival > expected_arrival);
    }

    static void SetTimingCost(Delivery *del, int arrival,
                              int expected_arrival, int earlytw, Cost &newcost) {

        if (del->rank == 0) {
            newcost.firstDeliveryCost +=
                    Sol::GetFirstDeliveryCost(del, arrival, earlytw);

            newcost.firstDeliveryCount +=
                    Sol::GetFirstDeliveryCount(del, arrival, earlytw);

            newcost.truckWaitingCost += Sol::GetTruckWaitingCost(del, arrival, earlytw);

            newcost.clientWaitingCost +=
                    Sol::GetClientWaitingCost(del, arrival, earlytw);
        } else {
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

    void BuildFromDepotSetIntervall();

    void BuildFromDepotSetIntervall(Depot *depot);

    void BuildFromDriverSetIntervall();

    void BuildFromDriverSetIntervall(Driver *d);

    void InsertAfterDepot(Node *n, Node *prev, Node *dep);

    bool operator<(const Sol &rhs) const;


    std::string CustomerString();

    ~Sol() = default;

    template<typename T>
    bool hasScheduled(const T &container) {
        return (std::includes(satisfiedCustomers.begin(), satisfiedCustomers.end(),
                              container.begin(), container.end()));
    }

    std::string toString() const;
    std::string toString(Customer *c) const ;

    Order *findIdleOrder(Customer *c) {
        std::vector<Order *> orders(GetOrders(c));
        std::shuffle(orders.begin(), orders.end(), Parameters::RANDOM_GEN);
        for (Order *cur_o: orders) {
            if (isOrderSatisfied(cur_o))
                continue;
            return cur_o;
        }
        return nullptr;
    }

    Delivery *GetNextIdleDelivery(Order *o) {

        for (int i = 0; i < GetDeliveryCount(o); i++) {
            Delivery *del = GetDelivery(o, i);
            if (GetDriverAssignedTo(del) == nullptr)
                return del;
        }
        return nullptr;
    }

    void ShowDepotSlots() {
        for (int i = 0; i < GetDepotCount(); i++) {
            ShowSlot(GetDepot(i));
        }
    }

    void ShowSlot(Depot *myDep) {
        std::cout << "Slots for depot " << myDep->depotID << std::endl;
        for (const auto &x: depotLoadingIntervals[myDep->depotID]) {
            std::cout << x << "--";
        }
        std::cout << std::endl;
    }

    void ShowDriverSlots() {
        for (int i = 0; i < GetDriverCount(); i++) {
            ShowSlot(GetDriver(i));
        }
    }

    void ShowSlot(Driver *d) {
        std::cout << "Slots for driver " << d->id << std::endl;
        for (const auto &x: driverWorkingIntervals[d->id]) {
            std::cout << x << "--";
        }
        std::cout << std::endl;
    }

    void ShowAllSchedule();

    void exportCSVFormat(const std::string &fileName);

    static void InitStructure(Data *dat) {

        Sol::CustomerConflict.clear();
        Sol::CustomerConflict.resize(dat->GetCustomerCount());
        Sol::FailureCause.clear();
        Sol::FailureCause.resize(dat->GetNodeCount(), Parameters::FAILURECAUSE::NONE);
        Sol::FailureCount.clear();
        Sol::FailureCount.resize(dat->GetNodeCount(), 0);
        Sol::StartBefore.clear();
        Sol::StartBefore.resize(dat->GetNodeCount(), 0);
        Sol::pullVisit.clear();
        Sol::pullVisit.resize(dat->GetNodeCount(), 0);
        Sol::pushVisit.clear();
        Sol::pushVisit.resize(dat->GetNodeCount(), 0);
        Sol::TabuFleet.clear();
        Sol::TabuFleet.resize(dat->GetNodeCount());
        Sol::minDelay.clear();
        Sol::minDelay.resize(dat->GetNodeCount(), 0);
        TimeSlot::myData = *dat;
    }

private:
    Data *_data{};
    Cost _last_cost;
    int UnassignedCount{};
};

class MyHashFunction {
public:
    size_t operator()(const std::string &str) const {
        return std::hash<std::string>()(str) ;
    };
};

#endif //CODE_SOLUTION_H
