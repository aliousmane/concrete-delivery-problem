
#include "Solution.h"
#include <iostream>
using namespace std;

std::vector<int> Sol::FailureCause = std::vector<int>();
std::vector<int> Sol::minDelay = std::vector<int>();
Sol::Sol(Data *data): _data(data), _last_cost(false), DriverAssignTo(data->GetNodeCount(),nullptr),
RoutesLength(data->GetDriverCount(), 0),
                      orderCapRestante(data->GetOrderCount(),0),
                      shiftDuration(data->GetDriverCount(), 0),
                      shiftDurationCost(data->GetDriverCount(), 0),
                      DepotSize(data->GetDepotCount(), 0),
                      EndServiceTime(data->GetNodeCount(),0),
                      ExpectedServiceTime(data->GetNodeCount(),0),
                      StartServiceTime(data->GetNodeCount(),0),
                      clientCapRestante(data->GetCustomerCount(),0),
                      driverWorkingIntervals(data->GetDriverCount()),
                      nodeServiceIntervals(data->GetNodeCount()),
                      depotLoadingIntervals(data->GetDepotCount()),
                      ArrivalTime(data->GetNodeCount(),0),
                      DepartureTime(data->GetNodeCount(),0),
                      DeliveryLoad(data->GetNodeCount(),0), WaitingTime(data->GetNodeCount(),0),
                      Unassigned(data->GetNodeCount(),nullptr),
                      VisitFlags(data->GetNodeCount(),false),
                      VisitFlagCost(data->GetNodeCount(),false),
                      DepotAssignTo(data->GetNodeCount(),nullptr),
                      DepotNext(data->GetNodeCount(),nullptr),
                      CustomerNext(data->GetNodeCount(),nullptr),
                      DriverNext(data->GetNodeCount(),nullptr),
                      DriverPrev(data->GetNodeCount(),nullptr),
                      DepotPrev(data->GetNodeCount(),nullptr),
                      CustomerPrev(data->GetNodeCount(),nullptr),
                      UnassignedIndex(data->GetCustomerCount(),0),
                      updateCost(), UnassignedCount(0)
                      {
    InitCustomers();
    InitOrders();
    AssignStationToCustomers();
    AssignStationToDepots();
    AssignStationToDriver();
}

void Sol::InitCustomers(){
    for(int i=0;i<_data->GetCustomerCount();i++){
        Customer *c = _data->GetCustomer(i);
        clientCapRestante[i]=c->demand;
    }
}

void Sol::InitOrders(){
    for(int i=0;i<_data->GetOrderCount();i++){
        Order *o = _data->GetOrder(i);
        orderCapRestante[i]=o->demand;
    }
}
void Sol::AssignStationToDriver(){
    for (int i = 0; i < _data->GetDriverCount(); i++) {
        RoutesLength[i] = 0;
        Driver *d = _data->GetDriver(i);
        Node *n1 = _data->GetNode(d->StartNodeID);
        Node *n2 = _data->GetNode(d->EndNodeID);
        shiftDuration[i] = 0;
        shiftDurationCost[i] = 0;
        DriverNext[n1->id] = n2;
        DriverPrev[n2->id] = n1;
        DriverAssignTo[n1->id] = d;
        DriverAssignTo[n2->id] = d;
    }
}
void Sol::AssignStationToDepots(){
    for (int i = 0; i < _data->GetDepotCount(); i++) {
        Depot *n = _data->GetDepot(i);
        Node *start = _data->GetNode(n->StartNodeID);
        Node *end = _data->GetNode(n->EndNodeID);
        DepotNext[start->id] = end;
        DepotNext[end->id] = start;
    }
}
void Sol::AssignStationToCustomers(){
    for (int i = 0; i < _data->GetCustomerCount(); i++) {
        Customer *c = _data->GetCustomer(i);
        Node *n1 = _data->GetNode(c->StartNodeID);
        Node *n2 = _data->GetNode(c->EndNodeID);
        StartServiceTime[n1->id]=c->early_tw;
        EndServiceTime[n1->id]=c->early_tw;
        CustomerNext[n1->id]=n2;
        CustomerPrev[n2->id]=n1;
        StartServiceTime[n2->id]=c->late_tw;
        EndServiceTime[n2->id]=c->late_tw;
    }
}

void Sol::PutAllCustomersToUnassigned() {
    for (int i = 0; i < GetCustomerCount(); i++) {
        UnassignCustomer(GetCustomer(i));
        AddToUnassigneds(GetCustomer(i));
    }
}

void Sol::UnassignCustomer(Customer *c){
    std::vector<Order*> orders =  GetOrders(c);
    for(auto o1 :orders){
        UnassignOrder(o1);
    }
}
void Sol::UnassignOrder(Order *o) {
    for (int i = 0; i < GetDeliveryCount(o); i++) {
        Delivery *del = GetDelivery(o, i);
        Dock *dock = GetDock(del->dockID);
        Driver *d = DriverAssignTo[del->id];
        if (d != nullptr) {
            RemoveDelivery(del);
            RemoveDock(dock);
        }
    }
}

void Sol::UnassignDelivery(std::vector<Delivery*> const & delList) {
    for (auto del :delList) {
        Dock *dock = GetDock(del->dockID);
        Driver *d = DriverAssignTo[del->id];
        if (d != nullptr) {
            RemoveDelivery(del);
            RemoveDock(dock);
        }
    }
}

void Sol::Remove(Node *n){
    Driver *d = DriverAssignTo[n->id];
    if (d != nullptr) {
        RoutesLength[d->id]--;
        auto it = std::find_if(driverWorkingIntervals[d->id].begin(),
                               driverWorkingIntervals[d->id].end(),
                               [n](const TimeSlot &i1) {
                                   return ((i1.nodeID == n->id) and i1.nodeID != -1);
                               });

        if (it != driverWorkingIntervals[d->id].end()) {
            driverWorkingIntervals[d->id].erase(it);
        }
        // TODO
//        if (n->isDelivery) {
//            DriverVisitCount[d->id][n->custID]--;
//        }
    }
    if (DriverNext[n->id] != nullptr)
        DriverPrev[DriverNext[n->id]->id] = DriverPrev[n->id];
    if (DriverPrev[n->id] != nullptr)
        DriverNext[DriverPrev[n->id]->id] = DriverNext[n->id];
    DriverNext[n->id] = nullptr;
    DriverPrev[n->id] = nullptr;
    DriverAssignTo[n->id] = nullptr;
    VisitFlags [n->id] = false;
    VisitFlagCost[n->id] = false;

    TimeSlot in = TimeSlot(StartServiceTime[n->id], EndServiceTime[n->id]);
    auto it = driverWorkingIntervals[d->id].find(in);
    if (it != driverWorkingIntervals[d->id].end()) {
        driverWorkingIntervals[d->id].erase(it);
    }
}
void Sol::RemoveDelivery(Delivery *del) {
    if (VisitFlags[del->id]) {
        updateCost.lateDeliveryCost -=
                std::max(0, (EndServiceTime[del->id] - LateTW(del))) *
                Parameters::LATE_ARRIVAL_PENALTY;
        updateCost.satisfiedCost -= DeliveryLoad[del->id];
    }
    updateCost.travelCost -= del->travel_time;

    orderCapRestante[del->orderID] += DeliveryLoad[del->id];
    clientCapRestante[del->custID] += DeliveryLoad[del->id];
    DeliveryLoad[del->id] = 0;
    Remove(del);
    RemoveFromCustomer(del);

}
void Sol::RemoveDock(Dock *dock) {
    Remove(dock);
    RemoveFromDepot(dock);
}
void Sol::RemoveFromDepot(Dock *n){

    Node *dep = DepotAssignTo[n->id];
    if (dep == nullptr)
        return;

    DepotSize[dep->depotID]--;

    if (DepotNext[n->id] != nullptr) {
        DepotPrev[DepotNext[n->id]->id] =
                DepotPrev[n->id];
    }
    if (DepotPrev[n->id] != nullptr) {
        DepotNext[DepotPrev[n->id]->id] =
                DepotNext[n->id];
    }
    DepotNext[n->id] = nullptr;
    DepotPrev[n->id] = nullptr;
    TimeSlot in =
            TimeSlot(StartServiceTime[n->id], EndServiceTime[n->id], n->id);

    auto it = std::find_if(depotLoadingIntervals[dep->depotID].begin(),
                           depotLoadingIntervals[dep->depotID].end(),
                           [n](const TimeSlot &i1) {
                               return ((i1.nodeID == n->id) and i1.nodeID != -1);
                           });

    if (it != depotLoadingIntervals[dep->depotID].end()) {
        depotLoadingIntervals[dep->depotID].erase(it);
    }
    DepotAssignTo[n->id] = nullptr;
}
void Sol::AddToUnassigneds(Customer *n){
    UnassignedIndex[n->id] = UnassignedCount;
    Unassigned[UnassignedCount] = n;
    UnassignedCount++;
}
void Sol::RemoveFromCustomer(Delivery *del){
    if (CustomerNext[del->id] != nullptr)
        CustomerPrev[CustomerNext[del->id]->id] = CustomerPrev[del->id];
    if (CustomerPrev[del->id] != nullptr)
        CustomerNext[CustomerPrev[del->id]->id] = CustomerNext[del->id];
    CustomerNext[del->id] = nullptr;
    CustomerPrev[del->id] = nullptr;
}

void Sol::InsertAfter(Node *n, Node *prev) {
    Driver *d = DriverAssignTo[prev->id];
    RoutesLength[d->id]++;
    DriverAssignTo[n->id] = d;
    DriverNext[n->id] = DriverNext[prev->id];
    DriverPrev[n->id] = prev;
    if (DriverPrev[prev->id] != nullptr)
        DriverPrev[DriverNext[prev->id]->id] = n;
    DriverNext[prev->id] = n;
}
void Sol::InsertAfter(Node *n, Node *prev, Driver *d) {
    assert(d == DriverAssignTo[prev->id]);
    RoutesLength[d->id]++;
    DriverAssignTo[n->id] = d;
    DriverNext[n->id] = DriverNext[prev->id];
    DriverPrev[n->id] = prev;
    if (DriverNext[prev->id] != nullptr)
        DriverPrev[DriverNext[prev->id]->id] = n;
    DriverNext[prev->id] = n;
    assert(DriverAssignTo[n->id] != nullptr);
}
void Sol::InsertAfterDepot(Node *n, Node *prev, Node *dep) {

    DepotSize[dep->depotID]++;
    DepotAssignTo[n->id] = dynamic_cast<Depot *>(dep);
    Node *next_of_prev = DepotNext[prev->id];
    DepotNext[n->id] = next_of_prev;
    DepotPrev[n->id] = prev;
    DepotPrev[next_of_prev->id] = n;
    DepotNext[prev->id] = n;
}
void Sol::AssignDeliveryToCustomer(Delivery *n) {

//   TODO OrderVisitCount[n->custID]++;

    Node *end = GetNode(n->EndNodeID);
    Node *prev = CustomerPrev[n->EndNodeID];
    CustomerNext[prev->id] = n;
    CustomerPrev[n->id] = prev;
    CustomerNext[n->id] = end;
    CustomerPrev[end->id] = n;
    assert(CustomerPrev[n->id] != n);
}

void Sol::ShowSchedule(Delivery *del)  {
    Driver *d = GetDriverAssignedTo(del);
    if (d == nullptr)
        return;
    // printf("Node  | Cl  |Driver   |Load| From | -> |Dock | Depart  | Arrival   |
    // Wait  | Start | End   | To \n");
    Dock *dock = GetDock(del->dockID);
    printf("%04d |%02d(%02d)|%02d(%02d)| %02d |%04d|%02d|%04d|%04d-%04d| %04d"
           "  |%03d "
           "|%04d "
           "| %04d |%04d|\n",
           del->id, del->orderID, del->custID, d->id, d->capacity,
           DeliveryLoad[del->id], DriverPrev[dock->id]->id, Travel(DriverPrev[dock->id],dock), dock->id,
           StartServiceTime[dock->id], EndServiceTime[dock->id],
           ArrivalTime[del->id], WaitingTime[del->id],
           StartServiceTime[del->id], EndServiceTime[del->id],
           DriverNext[del->id]->id);
}

void Sol::ShowSchedule() {

    for (int i = 0; i < GetOrderCount(); i++) {
        Order *o = GetOrder(i);
        if(!isOrderSatisfied(o)) continue;
        ShowSchedule(o);
    }
    Cost coutSol = GetCost();
    printf(" travel cost %2.1lf\n", coutSol.travelCost);
    printf(" Waiting cost %2.1lf\n", coutSol.waitingCost);
    printf(" total cost %2.1lf\n", coutSol.getTotal());
}

void Sol::ShowSchedule(Order *o){
    cout<<*o<<endl;
    printf("Node| Cl    |Driver|Load|From|->|Dock| Depart  |Arrival|"
           "Wait|Start| End  | To |\n");
    for (int j = 0; j < GetDeliveryCount(o); j++) {
        Delivery *del = GetDelivery(o, j);
        if (DriverAssignTo[del->id] != nullptr)
            ShowSchedule(del);
    }
}

void Sol::ShowCustomer()  {
    double sum_ = 0;
    for (int i = 0; i < GetCustomerCount(); i++) {
        Customer *n = GetCustomer(i);
        if (!isClientSatisfied(n))
            continue;
        std::cout << "Customer " << n->custID << " (" << n->demand << ")-";
        sum_ += n->demand;
    }
    std::cout << ": cost:*" << sum_
              << "\n "
                 "*********************************************************"
                 "************\n";
}

void Sol::BuildFromDepotSetIntervall() {
    // cout << "In build\n";
    for (int count = 0; count < GetDepotCount(); count++) {
        Depot *depot = GetDepot(count);
        BuildFromDepotSetIntervall(depot);
    }
}
void Sol::BuildFromDepotSetIntervall(Depot *depot) {
    Node *end = GetNode(depot->EndNodeID);
    Node *prev = GetNode(depot->StartNodeID);
    DepotSize[depot->depotID] = 0;
    DepotNext[prev->id] = end;
    DepotPrev[end->id] = prev;
//    ShowLoadingSlot(depot);
    for (auto const & intv : depotLoadingIntervals[depot->depotID]) {
        Dock *dock_int = dynamic_cast<Dock*>(GetNode(intv.nodeID));
        InsertAfterDepot(dock_int, prev, depot);
        prev = dock_int;
    }
}

void Sol::ShowLoadingSlot(Depot *myDep) {
    cout << "Slots for " << myDep->depotID << endl;
    for (auto const &x : depotLoadingIntervals[myDep->depotID]) {
        cout << x << "--";
    }
    cout << endl;
}
bool Sol::operator<(const Sol &rhs) const {
    return const_cast<Sol*>(this)->GetCost() < const_cast<Sol&>(rhs).GetCost();
}

Sol::~Sol() {

}
