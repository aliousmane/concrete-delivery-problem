#include "Solution.h"
#include "Prompt.h"
#include <iostream>

using namespace std;

void Sol::Update() {

    updateCost = Cost();
    satisfiedCustomers.clear();
    unscheduledCustomers.clear();
    driverUsed.clear();
    BuildFromDepotSetIntervall();
    isFeasible = true;
    computeLastCost=false;
    InitDrivers();
    InitCustomers();
    InitDepots();
    nodeServiceIntervals =
            std::vector<TimeSlot>(GetNodeCount(), TimeSlot(0, 1440));
    UpdateForward();

    for (auto id: driverUsed) {
        Driver *d = GetDriver(id);
        Node *lastOfDriver = DriverPrev[d->EndNodeID];
        shiftDuration[d->id] +=
                Travel(lastOfDriver->distID, d->distID);
        d->shiftDuration = shiftDuration[d->id] - d->start_shift_time;
        updateCost.travelCost += Travel(lastOfDriver->distID, d->distID);
        //TODO: lastOfDriver->travel_time = Travel(lastOfDriver->distID, d->distID);
        updateCost.overTimeCost +=
                Parameters::OVERTIME_PENALTY *
                int(d->shiftDuration > Parameters::NORMAL_WORKING_TIME);
    }
    int count = 0;
    updateCost.waste = 0;
    for (int i = 0; i < GetCustomerCount(); i++) {
        Customer *c = GetCustomer(i);
        if (isClientSatisfied(c)) {
            updateCost.waste += std::abs(clientCapRestante[c->custID]);
            satisfiedCustomers.insert(c->custID);
        } else {
            unscheduledCustomers.insert(c->custID);
            count++;
        }
    }
    if (count == GetCustomerCount()) {
        isFeasible = false;
    }
}

void Sol::UpdateForward() {
    std::fill(VisitFlags.begin(), VisitFlags.end(), false);

    for (int i = 0; i < GetDepotCount(); i++) {
        Depot *dep = GetDepot(i);
        if (DepotSize[dep->depotID] < 1) {
            continue;
        }
        UpdateForward(dep);
        if (!isFeasible)
            break;
    }
}

void Sol::UpdateForward(Depot *dep) {

    Node *prev = DepotNext[dep->StartNodeID];
    while (prev->type != Parameters::END_LINK) {
        if (VisitFlags[prev->id]) {
            prev = DepotNext[prev->id];
            continue;
        }
        if (prev->type == Parameters::DOCK) {
            Driver *d = GetDriverAssignedTo(prev);
            if (d != nullptr) {
                auto *dock = dynamic_cast<Dock *>(prev);
                auto *prec_del = dynamic_cast<Delivery *>(DriverPrev[dock->id]);
                if (prec_del != nullptr) {
                    Depot *prec_dep = GetDepotAssignedTo(DriverPrev[prec_del->id]);
                    if (!VisitFlags[prec_del->id] and dep != prec_dep) {
                        UpdateForward(prec_dep);
                    }
                }
                auto *del = dynamic_cast<Delivery *>(DriverNext[dock->id]);
                if(!VisitFlags[del->id] )
                    Update(dep, dock, del);
            }
        }
        VisitFlags[prev->id] = true;
        prev = DepotNext[prev->id];
    }
}

void Sol::Update(Depot *dep, Dock *dock, Delivery *del) {
    VisitFlags[dock->id] = true;
    VisitFlags[del->id] = true;
    Sol::FailureCount[del->id] = 0;
    Sol::FailureCause[del->id] = Parameters::NONE;
    Driver *d = GetDriverAssignedTo(del);

    Order *o = GetOrder(del->orderID);
    Customer *c = GetCustomer(del->custID);

    OrderVisitCount[c->custID]++;
    clientDriverUsed[c->custID].insert(d->id);
    driverUsed.insert(d->id);

    DeliveryLoad[del->delID] = GetLoad(d, o);
    assert(DeliveryLoad[del->delID] >0);
    updateCost.satisfiedCost += DeliveryLoad[del->delID];
    updateCost.waste += d->capacity;
    UpdateDemand(c, o, DeliveryLoad[del->delID]);
    if (isClientSatisfied(c)) {
        satisfiedCustomers.insert(c->custID);
        updateCost.waste -= d->capacity;
        updateCost.waste += std::abs(clientCapRestante[c->custID]);
        updateCost.undeliveredCost-=c->demand;
    }
    DriverVisitCount[d->id][c->custID]++;
    del->demand = DeliveryLoad[del->delID];
    const int LOAD_DURATION = Data::LoadingTime(dep, del->demand);
    const int ADJUSTMENT_DURATION = Parameters::ADJUSTMENT_DURATION;
    const int UNLOADING_DURATION = Data::UnloadingTime(del, del->demand, d);

    sumServiceTime[c->custID] += UNLOADING_DURATION;

    Node *prec_del = CustomerPrev[del->id];

    int real_del_time = EndServiceTime[prec_del->id];
    Node *lastOfDriver = DriverPrev[dock->id];
    del->travel_time = Travel(lastOfDriver, dock) + Travel(dock, del) +
                             Travel(del->distID, d->distID);

    del->travel_time -= Travel(lastOfDriver->distID, d->distID);
    updateCost.travelCost += del->travel_time;
    UpdateDepotLoadingSet(
            dep, dock,
            TimeSlot(ArrivalTime[dock->id], ArrivalTime[dock->id] + LOAD_DURATION, 'L', *dock));

    StartServiceTime[dock->id] = ArrivalTime[dock->id];
    Dock *prev_dock = dynamic_cast<Dock *>( DepotPrev[dock->id]);
    if (prev_dock != nullptr) {
        StartServiceTime[dock->id] = std::max(ArrivalTime[dock->id], EndServiceTime[prev_dock->id]);
    }
    int arr_node = StartServiceTime[dock->id];
    arr_node += LOAD_DURATION;
    EndServiceTime[dock->id] = arr_node;
    arr_node += ADJUSTMENT_DURATION;
    DepartureTime[dock->id] = arr_node;
    arr_node += Travel(dock, del);
    ArrivalTime[del->id] = std::max(ArrivalTime[del->id], arr_node);
    arr_node = ArrivalTime[del->id];
    WaitingTime[del->id] = real_del_time - ArrivalTime[del->id];
    if (del->rank > 0) {
        if (WaitingTime[del->id] + Parameters::TIME_BTW_DELIVERY < 0) {
            WaitingTime[c->id] += WaitingTime[del->id] + Parameters::TIME_BTW_DELIVERY;
        }
    }
    NodeLateDelivery[del->id] = updateCost.lateDeliveryCost;
    ClientWaitingTime[del->id]=updateCost.clientWaitingCost;
    TruckWaitingTime[del->id]=updateCost.truckWaitingCost;
    Sol::SetTimingCost(del, arr_node, real_del_time, EarlyTW(del),
                       updateCost);
    ClientWaitingTime[del->id]=updateCost.clientWaitingCost- ClientWaitingTime[del->id];
    TruckWaitingTime[del->id]=updateCost.truckWaitingCost- TruckWaitingTime[del->id];

    arr_node = std::max(arr_node, real_del_time);
    StartServiceTime[del->id] = arr_node;
    arr_node += UNLOADING_DURATION;
    EndServiceTime[del->id] = arr_node;

    updateCost.lateDeliveryCost += Sol::GetLateDeliveryCost(del, arr_node, LateTW(del));

    NodeLateDelivery[del->id] = updateCost.lateDeliveryCost - NodeLateDelivery[del->id];
    OrderLateDelivery[del->orderID] += NodeLateDelivery[del->id];
    arr_node += Data::CleaningTime(del, d);
    DepartureTime[del->id] = arr_node;

    shiftDuration[d->id] += arr_node + Travel(lastOfDriver->distID, dock->distID) + Travel(del->distID, d->distID)
                            - Travel(lastOfDriver->distID, d->distID);
    d->shiftDuration = shiftDuration[d->id] - d->start_shift_time;
    updateCost.overTimeCost =
            Parameters::OVERTIME_PENALTY *
            int(d->shiftDuration > Parameters::MAX_WORKING_TIME);

    updateCost.waitingCost = updateCost.clientWaitingCost + updateCost.truckWaitingCost;

    TimeSlot intv = TimeSlot(ArrivalTime[dock->id], DepartureTime[del->id], 'D', *del);
    nodeServiceIntervals[del->id] = intv;
    driverWorkingIntervals[d->id].insert(intv);
    nodeServiceIntervals[dock->id] =
            TimeSlot(ArrivalTime[dock->id], EndServiceTime[dock->id], 'L', *dock);

}

void Sol::Update(Customer *c) {
    InitCustomer(c);
    for (auto order: GetOrders(c)) {
        Update(order);
    }
}

void Sol::Update(Order *o) {
    InitOrder(o);
    for (int i = 0; i < GetDeliveryCount(o); i++) {
        Delivery *del = GetDelivery(o, i);
        if (GetDriverAssignedTo(del) == nullptr) break;
        Dock *dock = GetDock(del->dockID);
        Update(GetDepot(del), dock, del);
    }
}

Cost Sol::GetCost() {
    _last_cost.Init();
    _last_cost.isFeasible = false;
    if (not isFeasible)
        return _last_cost;
    _last_cost.isFeasible = true;

    std::fill(VisitFlagCost.begin(), VisitFlagCost.end(), false);
    for (int i = 0; i < GetDriverCount(); i++) {
        Driver *d = GetDriver(i);
        shiftDurationCost[d->id] = 0;
        d->shiftDurationCost = 0;
    }
    for (int i = 0; i < GetDepotCount(); i++) {
        Depot *dep = GetDepot(i);
        if (DepotSize[dep->depotID] < 1)
            continue;
        GetCost(dep, _last_cost);
        if (not _last_cost.isFeasible)
            break;
    }
    _last_cost.satisfiedCost = 0;
    _last_cost.undeliveredCost = 0;
    _last_cost.waste = 0;
    for (int i = 0; i < GetCustomerCount(); i++) {
        Customer *c = GetCustomer(i);
        _last_cost.undeliveredCost += !(isClientSatisfied(c)) * c->demand;
        _last_cost.satisfiedCost += (isClientSatisfied(c)) * c->demand;
        if (isClientSatisfied(c)) {
            _last_cost.waste += std::abs(clientCapRestante[c->custID]);
        }
    }
    _last_cost.driverUsed = 0;

    for (int i = 0; i < GetDriverCount(); i++) {
        Driver *d = GetDriver(i);

        if (RoutesLength[d->id] < EPS)
            continue;
        _last_cost.driverUsed++;
        Node *lastOfDriver = DriverPrev[d->EndNodeID];
        shiftDurationCost[d->id] += Travel(lastOfDriver->distID, d->distID);
        d->shiftDurationCost = shiftDurationCost[d->id] - d->start_shift_time;
        _last_cost.travelCost += Travel(lastOfDriver->distID, d->distID);
        _last_cost.overTimeCost +=
                Parameters::OVERTIME_PENALTY *
                int(d->shiftDurationCost > Parameters::MAX_WORKING_TIME);
    }
    _last_cost.setTotal();
    computeLastCost=true;
    return _last_cost;
}


void Sol::GetCost(Depot *dep, Cost &cur_cost) {
    Node *prev = DepotNext[dep->StartNodeID];
    while (prev->type != Parameters::END_LINK) {
        if (VisitFlagCost[prev->id]) {
            prev = DepotNext[prev->id];
            continue;
        }
        if (prev->type == Parameters::DOCK) {
            Driver *d = GetDriverAssignedTo(prev);
            if (d != nullptr) {
                auto *dock = dynamic_cast<Dock *>(prev);
                auto *prec_del = dynamic_cast<Delivery *>(DriverPrev[dock->id]);
                if (prec_del != nullptr) {
                    Depot *prec_dep = GetDepotAssignedTo(DriverPrev[prec_del->id]);
                    if (!VisitFlagCost[prec_del->id] and dep != prec_dep) {
                        GetCost(prec_dep, cur_cost);
                    }
                }
                auto *del = dynamic_cast<Delivery *>(DriverNext[dock->id]);
                if(!VisitFlagCost[del->id] )
                    GetCost(dep, dock, del, cur_cost);
            }
        }
        VisitFlagCost[prev->id] = true;
        prev = DepotNext[prev->id];
    }
}

void Sol::GetCost(Depot *dep, Dock *dock, Delivery *del, Cost &cur_cost) {
    VisitFlagCost[dock->id] = true;
    VisitFlagCost[del->id] = true;

    Driver *d = GetDriverAssignedTo(del);

    const int UNLOADING_DURATION = Data::UnloadingTime(del, del->demand, d);

    Node *prec_del = CustomerPrev[del->id];
    cur_cost.waste += d->capacity;
    int real_del_time = EndServiceTime[prec_del->id];
    Node *lastOfDriver = DriverPrev[dock->id];
    cur_cost.travelCost += Travel(lastOfDriver, dock) + Travel(dock, del);

    int arr_node = ArrivalTime[del->id];

    Sol::SetTimingCost(del, arr_node, real_del_time, EarlyTW(del),
                       cur_cost);

    arr_node = StartServiceTime[del->id];
    arr_node += UNLOADING_DURATION;

    cur_cost.lateDeliveryCost += Sol::GetLateDeliveryCost(del, arr_node, LateTW(del));
    arr_node += Data::CleaningTime(del, d);


    shiftDurationCost[d->id] += arr_node + Travel(lastOfDriver->distID, dock->distID) + Travel(del->distID, d->distID)
                                - Travel(lastOfDriver->distID, d->distID);
    d->shiftDurationCost = shiftDurationCost[d->id] - d->start_shift_time;

    cur_cost.waitingCost = cur_cost.clientWaitingCost + cur_cost.truckWaitingCost;
}


Cost Sol::GetCost(Driver *d) {

    return {};
}

void Sol::Show() {

}

void Sol::ShowDrivers() {
    for (int i = 0; i < GetDriverCount(); i++) {
        Show(GetDriver(i));
    }
}

void Sol::Show(Driver *d) {
    if (RoutesLength[d->id] < 1) return;
    double dmd = 0;
    printf("%d cap %d len %d:\n", d->id, d->capacity, (int) RoutesLength[d->id]);
    Node *cur = GetNode(d->StartNodeID);
    while (cur->type != Parameters::END_LINK) {
        dmd += cur->demand;
        if (cur->type != Parameters::START_LINK) {
            printf("%c%d(%d->%d)-", cur->c, cur->id, StartServiceTime[cur->id],
                   EndServiceTime[cur->id]);
            if (cur->type == Parameters::DELIVERY) {
                auto *del = dynamic_cast<Delivery *>(cur);
                printf("C%d-", del->custID);
            }
        } else {
            printf("%d-", cur->id);
        }
        cur = DriverNext[cur->id];
    }
    Cost value = GetCost(d);

    printf("Driver %d: load:%2.lf cap:%d cost:%2.0lf \n", d->id, dmd, d->capacity,
           value.getTotal());
}


void Sol::UpdateDepotLoadingSet(Depot *dep, Dock *dock, TimeSlot const &intv) {

    auto it = std::find_if(
            depotLoadingIntervals[dep->depotID].begin(),
            depotLoadingIntervals[dep->depotID].end(),
            [&intv](const TimeSlot &i1) { return ((i1.nodeID == intv.nodeID) and i1.nodeID != -1); });

    if (it != depotLoadingIntervals[dep->depotID].end()) {
        depotLoadingIntervals[dep->depotID].erase(it);
    }
    depotLoadingIntervals[dep->depotID].insert(intv);
    DepotSize[dep->depotID] = (int) depotLoadingIntervals[dep->depotID].size();
    DepotAssignTo[dock->id] = dep;
}
