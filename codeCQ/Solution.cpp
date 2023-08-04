
#include "Solution.h"
#include <fstream>

using namespace std;

std::vector<int> Sol::FailureCause = std::vector<int>();
std::vector<std::set<int>> Sol::CustomerConflict = std::vector<std::set<int>>();
std::vector<int> Sol::FailureCount = std::vector<int>();
std::vector<double> Sol::minDelay = std::vector<double>();
std::vector<double> Sol::StartBefore = std::vector<double>();
std::vector<double> Sol::pullVisit = std::vector<double>();
std::vector<double> Sol::pushVisit = std::vector<double>();
std::vector<std::set<int>> Sol::TabuFleet = std::vector<std::set<int>>();
std::map<std::tuple<int, int>, double> Sol::nodeMaxStartService = std::map < std::tuple<int, int>,
double>();
std::vector<double> Sol::FixLoad = std::vector<double>();
std::vector<double> Sol::FixStartLoad = std::vector<double>();
std::vector<double> Sol::FixCustomerStartLoad = std::vector<double>();

Sol::Sol(Data *data) : _data(data), _last_cost(false), DriverAssignTo(data->GetNodeCount(), nullptr),
                       RoutesLength(data->GetDriverCount(), 0),
                       orderCapRestante(data->GetOrderCount(), 0),
                       orderLoads(data->GetOrderCount()),
                       OrderLateDelivery(data->GetOrderCount(), 0),
                       shiftDuration(data->GetDriverCount(), 0),
                       shiftDurationCost(data->GetDriverCount(), 0),
                       DriverVisitCount(data->GetDriverCount()),
                       LastOfDriver(data->GetDriverCount(), nullptr),
                       DepotSize(data->GetDepotCount(), 0),
                       EndServiceTime(data->GetNodeCount(), 0),
                       ExpectedServiceTime(data->GetNodeCount(), 0),
                       StartServiceTime(data->GetNodeCount(), 0),
                       clientCapRestante(data->GetCustomerCount(), 0),
                       driverWorkingIntervals(data->GetDriverCount()),
                       nodeServiceIntervals(data->GetNodeCount()),
                       depotLoadingIntervals(data->GetDepotCount()),
                       ArrivalTime(data->GetNodeCount(), 0),
                       DepartureTime(data->GetNodeCount(), 0),
                       ClientWaitingTime(data->GetNodeCount(), 0),
                       TruckWaitingTime(data->GetNodeCount(), 0),
                       WaitingTime(data->GetNodeCount(), 0),
                       Unassigned(data->GetNodeCount(), nullptr),
                       VisitFlags(data->GetNodeCount(), false),
                       VisitFlagCost(data->GetNodeCount(), false),
                       DepotAssignTo(data->GetNodeCount(), nullptr),
                       DepotNext(data->GetNodeCount(), nullptr),
                       CustomerNext(data->GetNodeCount(), nullptr),
                       DriverNext(data->GetNodeCount(), nullptr),
                       DriverPrev(data->GetNodeCount(), nullptr),
                       DepotPrev(data->GetNodeCount(), nullptr),
                       CustomerPrev(data->GetNodeCount(), nullptr),
                       NodeLateDelivery(data->GetNodeCount(), 0),
                       UnassignedIndex(data->GetCustomerCount(), 0),
                       clientDriverUsed(data->GetCustomerCount()),
                       OrderVisitCount(data->GetCustomerCount(), 0),
                       sumServiceTime(data->GetCustomerCount(), 0),
                       updateCost(), UnassignedCount(0),
                       DeliveryLoad(data->GetDeliveryCount(), 0) {
    InitCustomers();
    InitDepots();
//    InitOrders();
    AssignStationToCustomers();
    AssignStationToDepots();
    AssignStationToDriver();
    InitDrivers();
}

void Sol::InitCustomers() {
    updateCost.undeliveredCost = 0;
//    if (keyCustomers.empty())
    {
        for (int i = 0; i < _data->GetCustomerCount(); i++) {
            InitCustomer(_data->GetCustomer(i));
        }
    }
}

void Sol::InitCustomer(Customer *c) {
    clientCapRestante[c->custID] = 0;
    clientDriverUsed[c->custID].clear();
    OrderVisitCount[c->custID] = 0;
    sumServiceTime[c->custID] = 0;
    WaitingTime[c->id] = 0;
//    updateCost.undeliveredCost += c->demand;
    for (auto o: GetOrders(c)) {
        InitOrder(o);
    }
}

void Sol::InitDrivers() {
    if (availableDrivers.empty()) {
        for (int i = 0; i < GetDriverCount(); i++) {
            InitDriver(GetDriver(i));
        }
    } else {
        for (auto id: availableDrivers) {
            InitDriver(GetDriver(id));
        }
    }
}

void Sol::InitDriver(Driver *d) {
    d->Init();
    shiftDuration[d->id] = 0;
    LastOfDriver[d->id] = GetNode(d->StartNodeID);
    driverWorkingIntervals[d->id].clear();
    DriverVisitCount[d->id] = std::vector<int>(_data->GetCustomerCount(), 0);
}

void Sol::InitDepots() {
    for (int i = 0; i < GetDepotCount(); i++) {
        InitDepot(GetDepot(i));
    }
}

void Sol::InitDepot(Depot *dep) {
    depotLoadingIntervals[dep->depotID].clear();
}

void Sol::InitOrders() {
    for (int i = 0; i < _data->GetOrderCount(); i++) {
        InitOrder(_data->GetOrder(i));
    }
}

void Sol::InitOrder(Order *o) {

    orderCapRestante[o->orderID] = o->demand;
    OrderLateDelivery[o->orderID] = 0;
    clientCapRestante[o->custID] += o->demand;
    orderLoads[o->orderID].clear();
    updateCost.undeliveredCost += o->demand * Parameters::UNDELIVERY_PENALTY;
}

void Sol::AssignStationToDriver() {
    for (int i = 0; i < _data->GetDriverCount(); i++) {
        RoutesLength[i] = 0;
        Driver *d = _data->GetDriver(i);
        Node *n1 = _data->GetNode(d->StartNodeID);
        Node *n2 = _data->GetNode(d->EndNodeID);
        DriverNext[n1->id] = n2;
        DriverPrev[n2->id] = n1;
        DriverAssignTo[n1->id] = d;
        DriverAssignTo[n2->id] = d;
    }
}

void Sol::AssignStationToDepots() {
    for (int i = 0; i < _data->GetDepotCount(); i++) {
        Depot *n = _data->GetDepot(i);
        Node *start = _data->GetNode(n->StartNodeID);
        Node *end = _data->GetNode(n->EndNodeID);
        DepotNext[start->id] = end;
        DepotNext[end->id] = start;
    }
}

void Sol::AssignStationToCustomers() {
    for (int i = 0; i < _data->GetCustomerCount(); i++) {
        Customer *c = _data->GetCustomer(i);
        Node *n1 = _data->GetNode(c->StartNodeID);
        Node *n2 = _data->GetNode(c->EndNodeID);
        StartServiceTime[n1->id] = c->early_tw;
        EndServiceTime[n1->id] = c->early_tw;
        CustomerNext[n1->id] = n2;
        CustomerPrev[n2->id] = n1;
        StartServiceTime[n2->id] = c->late_tw;
        EndServiceTime[n2->id] = c->late_tw;
    }
}

void Sol::PutAllCustomersToUnassigned() {
    for (int i = 0; i < GetCustomerCount(); i++) {
        UnassignCustomer(GetCustomer(i));
        AddToUnassigneds(GetCustomer(i));
    }
}

void Sol::UnassignCustomer(Customer *c) {
    std::vector<Order *> orders = GetOrders(c);
    for (auto o1: orders) {
        UnassignOrder(o1);
    }
}

void Sol::UnassignOrder(Order *o) {
    for (int i = 0; i < GetDeliveryCount(o); i++) {
        Delivery *del = GetDelivery(o, i);
        Driver *d = DriverAssignTo[del->id];
        if (d == nullptr) break;
        RemoveDelivery(del);
        assert(GetDriverAssignedTo(del) == nullptr);
//            RemoveDock(dock);
    }
}

void Sol::UnassignDelivery(std::vector<Delivery *> const &delList) {
    for (auto del: delList) {
        Driver *d = DriverAssignTo[del->id];
        if (d != nullptr) {
            RemoveDelivery(del);
        }
    }
}

void Sol::Remove(Node *n) {
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
    }
    if (DriverNext[n->id] != nullptr)
        DriverPrev[DriverNext[n->id]->id] = DriverPrev[n->id];
    if (DriverPrev[n->id] != nullptr)
        DriverNext[DriverPrev[n->id]->id] = DriverNext[n->id];
    DriverNext[n->id] = nullptr;
    DriverPrev[n->id] = nullptr;
    DriverAssignTo[n->id] = nullptr;
    VisitFlags[n->id] = false;
    VisitFlagCost[n->id] = false;

    TimeSlot in = TimeSlot(StartServiceTime[n->id], EndServiceTime[n->id]);
    auto it = driverWorkingIntervals[d->id].find(in);
    if (it != driverWorkingIntervals[d->id].end()) {
        driverWorkingIntervals[d->id].erase(it);
    }
    if (RoutesLength[d->id] == 0) {
        driverUsed.erase(d->id);
    }
}

void Sol::RemoveDelivery(Delivery *del) {
    assert(del!= nullptr) ;
    if (VisitFlags[del->id]) {
        updateCost.lateDeliveryCost -=
                std::max(0., (EndServiceTime[del->id] - LateTW(del))) *
                Parameters::LATE_ARRIVAL_PENALTY;
        updateCost.satisfiedCost -= DeliveryLoad[del->delID];
    }
    updateCost.travelCost -= del->travel_time;

    updateCost.clientWaitingCost -= ClientWaitingTime[del->id];
    updateCost.truckWaitingCost -= TruckWaitingTime[del->id];
    updateCost.waitingCost = updateCost.clientWaitingCost +
                             updateCost.truckWaitingCost;
    Sol::FailureCount[del->id] = 0;
    assert(DeliveryLoad[del->delID] > 0);
    orderCapRestante[del->orderID] += DeliveryLoad[del->delID];
    clientCapRestante[del->custID] += DeliveryLoad[del->delID];
    OrderLateDelivery[del->orderID] -= NodeLateDelivery[del->id];
    NodeLateDelivery[del->id] = 0;
    DeliveryLoad[del->delID] = 0;
    Driver *d = DriverAssignTo[del->id];
    updateCost.waste -= d->capacity;
    if (!DriverVisitCount[d->id].empty()) {
        DriverVisitCount[d->id][del->custID]--;
    }
    Remove(del);
    Dock *dock = GetDock(del->dockID);
    RemoveDock(dock);
    RemoveFromCustomer(del);
    bool find = false;
    for (int r = 0; r < del->rank; r++) {
        Delivery *del1 = GetDelivery(del->orderID, r);
        Driver *d1 = GetDriverAssignedTo(del1);
        if (d1 == nullptr) break;
        if (d1 == d) {
            find = true;
            break;
        }
    }
    if (!find) {
        clientDriverUsed[del->custID].erase(d->id);
    }
    lateCustomers.erase(del->custID);
}

void Sol::RemoveDock(Dock *dock) {
    Remove(dock);
    RemoveFromDepot(dock);
}

void Sol::RemoveFromDepot(Dock *n) {

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

    auto it = std::find_if(depotLoadingIntervals[dep->depotID].begin(),
                           depotLoadingIntervals[dep->depotID].end(),
                           [n](const TimeSlot &i1) {
                               return ((i1.n.id == n->id) and i1.nodeID != -1);
                           });

    if (it != depotLoadingIntervals[dep->depotID].end()) {
        depotLoadingIntervals[dep->depotID].erase(it);       
    }
    DepotAssignTo[n->id] = nullptr;
}

void Sol::AddToUnassigneds(Customer *n) {
    UnassignedIndex[n->custID] = UnassignedCount;
    Unassigned[UnassignedCount] = n;
    UnassignedCount++;
}

void Sol::RemoveFromCustomer(Delivery *del) {
    if (CustomerNext[del->id] != nullptr)
        CustomerPrev[CustomerNext[del->id]->id] = CustomerPrev[del->id];
    if (CustomerPrev[del->id] != nullptr)
        CustomerNext[CustomerPrev[del->id]->id] = CustomerNext[del->id];
    CustomerNext[del->id] = nullptr;
    CustomerPrev[del->id] = nullptr;
    OrderVisitCount[del->custID]--;
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
    LastOfDriver[d->id] = DriverPrev[d->EndNodeID];
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
    Node *end = GetNode(n->EndNodeID);
    Node *prev = CustomerPrev[n->EndNodeID];
    CustomerNext[prev->id] = n;
    CustomerPrev[n->id] = prev;
    CustomerNext[n->id] = end;
    CustomerPrev[end->id] = n;
    assert(CustomerPrev[n->id] != n);
    if (prev->c == 'D') {
        assert(GetDriverAssignedTo(prev) != nullptr);
    }
}

void Sol::ShowSchedule(Delivery *del) {
    Driver *d = GetDriverAssignedTo(del);
    // printf("Node  | Cl  |Driver   |Load| From | -> |Dock | Depart|->  | Arrival   |
    // Wait  | Start | End   | To \n");
    Dock *dock = GetDock(del->dockID);
    if (d != nullptr) {
        printf("%3d %04d |%02d(%02d)|%02d(%02.0lf)| %02.1lf |%04d|%01.1lf|%04d|%04.0lf-%04.0lf|%01.1lf|%04.0lf "
               "  |%03.0lf "
               "|%04.0lf "
               "| %04.0lf |%04d|",
               del->rank+1, del->id, del->orderID, del->custID, d->id, d->capacity,
               DeliveryLoad[del->delID], DriverPrev[dock->id]->id, Travel(DriverPrev[dock->id], dock), dock->id,
               StartServiceTime[dock->id], EndServiceTime[dock->id], Travel(dock, del),
               ArrivalTime[del->id], WaitingTime[del->id],
               StartServiceTime[del->id], EndServiceTime[del->id],
               DriverNext[del->id]->id);
    } else {
        printf("%04d |%02d(%02d)|%02d(%02d)| %02.1lf |%04d|%02d|%04d|%04.0lf-%04.0lf|%01.1lf|%04.0lf "
               "  |%03.0lf "
               "|%04.0lf "
               "| %04.0lf |%04d|",
               del->id, del->orderID, del->custID, -1, -1,
               DeliveryLoad[del->delID], -1, -1, dock->id,
               StartServiceTime[dock->id], EndServiceTime[dock->id], Travel(dock, del),
               ArrivalTime[del->id], WaitingTime[del->id],
               StartServiceTime[del->id], EndServiceTime[del->id],
               -1);
    }
    if (CustomerNext[del->StartNodeID]->id == del->id) {
        printf("*");
    }
    printf("\n");
}

void Sol::ShowSchedule() {

    for (int i = 0; i < GetOrderCount(); i++) {
        Order *o = GetOrder(i);
        if (!isOrderSatisfied(o)) {
            continue;
        }
        ShowSchedule(o);
    }
    Cost coutSol = GetCost();
    printf(" travel cost %2.1lf|", coutSol.travelCost);
    printf(" Waiting cost %2.1lf| ", coutSol.waitingCost);
    printf(" total cost %2.1lf\n", coutSol.getTotal());
}

void Sol::ShowAllSchedule() {
    for (int i = 0; i < GetOrderCount(); i++) {
        Order *o = GetOrder(i);
        if (!isOrderSatisfied(o)) {
            cout << "Incomplete schedule remaining " <<orderCapRestante[o->orderID]<< endl;
//            continue;
        }
        ShowSchedule(o);
    }
    Cost coutSol = GetCost();
    printf(" travel cost %2.1lf|", coutSol.travelCost);
    printf(" Waiting cost %2.1lf| ", coutSol.waitingCost);
    printf(" total cost %2.1lf\n", coutSol.getTotal());
}

void Sol::ShowSchedule(Order *o) {
    cout << *o << endl;
    printf("Node| Cl    |Driver|Load|From|->|Dock| Depart  |->|Arrival|"
           "Wait|Start| End  | To |\n");
    for (int j = 0; j < GetDeliveryCount(o); j++) {
        Delivery *del = GetDelivery(o, j);
        if (DriverAssignTo[del->id] != nullptr) {
            if (j > 0) {
                assert(DriverAssignTo[GetDelivery(o, j - 1)->id] != nullptr);
            }
            ShowSchedule(del);

        }
    }
}

void Sol::ShowSchedule(Customer *c) {

    for (auto o: GetOrders(c)) {
        ShowSchedule(o);
    }
}

void Sol::ShowCustomer() {
    double sum_ = 0;
    for (int i = 0; i < GetCustomerCount(); i++) {
        Customer *n = GetCustomer(i);
        if (!isClientSatisfied(n))
            continue;
        std::cout << "C" << n->constID << " (" << n->demand << ")-";
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
//    ShowSlot(depot);
    for (auto const &intv: depotLoadingIntervals[depot->depotID]) {
        Dock *dock_int = dynamic_cast<Dock *>(GetNode(intv.nodeID));
        InsertAfterDepot(dock_int, prev, depot);
        prev = dock_int;
    }
}


void Sol::BuildFromDriverSetIntervall() {
    for (int k = 0; k < GetDriverCount(); k++) {
        BuildFromDriverSetIntervall(GetDriver(k));
    }
}

void Sol::BuildFromDriverSetIntervall(Driver *d) {
    Node *end = GetNode(d->EndNodeID);
    Node *prev = GetNode(d->StartNodeID);

    DriverNext[prev->id] = end;
    DriverPrev[end->id] = prev;

    for (const auto &intv: driverWorkingIntervals[d->id]) {
        auto *del_int = dynamic_cast<Delivery *>(GetNode(intv.nodeID));
        Dock *dock_int = GetDock(del_int->dockID);
        Node *dep = GetDepotAssignedTo(dock_int);
        InsertAfter(dock_int, prev, d);
        InsertAfter(del_int, dock_int, d);
        prev = del_int;
    }
}

bool Sol::operator<(const Sol &rhs) const {
    return const_cast<Sol *>(this)->GetCost() < const_cast<Sol &>(rhs).GetCost();
}

std::string Sol::CustomerString() {
    std::stringstream ss;
    double sum_ = 0;
    for (int i = 0; i < GetCustomerCount(); i++) {
        Customer *n = GetCustomer(i);
        if (clientCapRestante[n->custID] > 0)
            continue;
        ss << n->constID << ":" << n->demand << "-";
        sum_ += n->demand;
    }
    ss << " | " << sum_;
    return ss.str();
}

std::string Sol::toString() const {
    std::stringstream ss;
    ss << updateCost.satisfiedCost << ":";
    for (int i = 0; i < GetCustomerCount(); i++) {
        Customer *c = const_cast<Sol *>(this)->GetCustomer(i);
        ss << const_cast<Sol *>(this)->toString(c);
    }
    return ss.str();
}

std::string Sol::toString(Customer *c) const {
    std::stringstream ss;
    for (auto o1: const_cast<Sol *>(this)->GetOrders(c)) {
        if (!isOrderSatisfied(o1))
            continue;
        std::vector<int> countDriver(GetDriverCount(), 0);
        ss << "<" << o1->orderID << ":";
        for (int j = 0; j < GetDeliveryCount(o1); j++) {
            Delivery *del = const_cast<Sol *>(this)->GetDelivery(o1, j);
            Driver *d = const_cast<Sol *>(this)->GetDriverAssignedTo(del);
            if (d != nullptr) {
                ss << del->id << "-(" << int(DriverVisitCount[d->id][del->custID] > 1) << ")-[" << d->capacity << "-"
                   << DeliveryLoad[del->delID];
                if (del->rank == 0) {
                    if (StartServiceTime[del->id] <= EarlyTW(del)) {
                        ss << "<]-";
                    } else {
                        ss << ">]-";
                    }
                } else {
                    ss << "]-";
                }
                countDriver[d->id]++;
            } else {
                ss << del->id << "[0-0]";
            }
        }
        ss << ">";
    }
    return ss.str();
}

void Sol::exportCSVFormat(const std::string &fileName) {
    std::ofstream myfile(fileName);
    if (myfile.is_open()) {
        myfile
                << "instance;Del;Order;Cust;Driver;Load;ArrDock;FinDock;ArrDel;Waiting;StartDel;FinDel;ELT;LTW;Cap;rank;Prev;TravelToDock;Dock;TravelToDel;Next;Depot"
                << std::endl;
        for (int i = 0; i < GetOrderCount(); i++) {
            Order *o = GetOrder(i);
            for (int j = 0; j < GetDeliveryCount(o); j++) {
                Delivery *del = GetDelivery(o, j);
                Dock *dock = GetDock(del->dockID);
                Driver *d = GetDriverAssignedTo(del);
                if (d == nullptr) break;
                // std::cout << _prob->instance_name << std::endl;
                myfile << _data->instance_name
                       << ";" << del->id
                       << ";" << del->orderID
                       << ";" << del->custID
                       << ";" << d->id
                       << ";" << DeliveryLoad[del->delID]
                       << ";" << StartServiceTime[dock->id]
                       << ";" << EndServiceTime[dock->id]
                       << ";" << ArrivalTime[del->id]
                       << ";" << WaitingTime[del->id]
                       << ";" << StartServiceTime[del->id]
                       << ";" << EndServiceTime[del->id]
                       << ";" << EarlyTW(del)
                       << ";" << LateTW(del)
                       << ";" << d->capacity
                       << ";" << d->rank
                       << ";" << DriverPrev[dock->id]->id
                       << ";" << Travel(DriverPrev[dock->id], dock)
                       << ";" << dock->id
                       << ";" << Travel(dock, del)
                       << ";" << DriverNext[del->id]->id
                       << ";" << del->depotID
                       << endl;

            }
        }
        std::cout << "./" << fileName << " ----- CREATED " << std::endl;
    } else
        std::cout << "----- IMPOSSIBLE TO OPEN: " << fileName << std::endl;
}