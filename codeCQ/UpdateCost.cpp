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
    computeLastCost = false;
    InitDrivers();
    InitCustomers();
    InitDepots();
    nodeServiceIntervals =
            std::vector<TimeSlot>(GetNodeCount(), TimeSlot(0, 1440));
    UpdateForward();
    updateCost.underWorkCost = 0;
    updateCost.overTimeCost = 0;
    updateCost.driverUsed = 0;
    for (int i = 0; i < GetDriverCount(); i++) {
        Driver *d = GetDriver(i);
        if (RoutesLength[d->id] > 0) {
            updateCost.driverUsed++;
        }
        d->underWork = Sol::GetUnderWorkCost(shiftDuration[d->id]);
        updateCost.underWorkCost += d->underWork;
        d->overTime = Sol::GetOvertimeCost(shiftDuration[d->id]);
        updateCost.overTimeCost += d->overTime;
    }
    int count = 0;
    updateCost.waste = 0;

//TODO    for (int i = 0; i < GetOrderCount(); i++) {
//        Order *o = GetOrder(i);
//
//    }

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
    updateCost.setTotal();
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
                if (!VisitFlags[del->id])
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
    updateCost.driverUsed = (int) driverUsed.size();
    assert(DeliveryLoad[del->delID] > 0);
    updateCost.satisfiedCost += DeliveryLoad[del->delID];
    updateCost.waste += d->capacity;
    UpdateDemand(c, o, DeliveryLoad[del->delID]);
    orderLoads[o->orderID].insert(DeliveryLoad[del->delID]);
    if (isOrderSatisfied(o)) {
        updateCost.undeliveredCost -= o->demand * Parameters::UNDELIVERY_PENALTY;
    }

    if (isClientSatisfied(c)) {
        satisfiedCustomers.insert(c->custID);
        updateCost.waste -= d->capacity;
        updateCost.waste += std::abs(clientCapRestante[c->custID]);
//        updateCost.undeliveredCost -= c->demand;
    }
    if (Parameters::SHOW) {
        cout << *del << " " << orderCapRestante[del->orderID] << " " << clientCapRestante[del->custID] << endl;
    }
    DriverVisitCount[d->id][c->custID]++;
    del->demand = DeliveryLoad[del->delID];
    const double LOAD_DURATION = Data::LoadingTime(dep, del->demand);
    const int ADJUSTMENT_DURATION = Parameters::ADJUSTMENT_DURATION;
    const double UNLOADING_DURATION = Data::UnloadingTime(del, del->demand, d);

    sumServiceTime[c->custID] += UNLOADING_DURATION;

    Node *prec_del = CustomerPrev[del->id];

    double real_del_time = EndServiceTime[prec_del->id];
    Node *lastOfDriver = DriverPrev[dock->id];
    del->travel_time = Travel(lastOfDriver, dock) + Travel(dock, del) +
                       Travel(del->distID, d->distID);

    del->travel_time -= Travel(lastOfDriver->distID, d->distID);
    updateCost.travelCost += del->travel_time;

    StartServiceTime[dock->id] = ArrivalTime[dock->id];
    double start1 = StartServiceTime[dock->id];
    Dock *prev_dock = dynamic_cast<Dock *>( DepotPrev[dock->id]);
    if (prev_dock != nullptr) {
        StartServiceTime[dock->id] =
                std::max(StartServiceTime[dock->id], EndServiceTime[prev_dock->id]);
    }

    TimeSlot loadSlot(StartServiceTime[dock->id], StartServiceTime[dock->id] + LOAD_DURATION,
                      'L', *dock);

// !   Sol::FindEmptySlot(depotLoadingIntervals[dep->depotID], loadSlot, LOAD_DURATION);

    UpdateDepotLoadingSet(dep, dock, loadSlot);

    StartServiceTime[dock->id] = loadSlot.lower;

    assert(start1 == StartServiceTime[dock->id]);
    double arr_node = StartServiceTime[dock->id];
    arr_node += LOAD_DURATION;
    EndServiceTime[dock->id] = arr_node;

    arr_node += ADJUSTMENT_DURATION;
    DepartureTime[dock->id] = arr_node;
    arr_node += Travel(dock, del);
    ArrivalTime[del->id] = std::max(ArrivalTime[del->id], arr_node);
    arr_node = ArrivalTime[del->id];
    WaitingTime[del->id] = real_del_time - ArrivalTime[del->id];
    if (prec_del->type == Parameters::DELIVERY) {
        if (WaitingTime[del->id] + this->GetTimeBtwDel(del) < 0) {
            WaitingTime[c->id] += WaitingTime[del->id] + this->GetTimeBtwDel(del);
        }
        assert(GetDriverAssignedTo(prec_del) != nullptr);
        assert(-WaitingTime[del->id] <= this->GetTimeBtwDel(del) + EPS);
    }
    NodeLateDelivery[del->id] = updateCost.lateDeliveryCost;
    ClientWaitingTime[del->id] = updateCost.clientWaitingCost;
    TruckWaitingTime[del->id] = updateCost.truckWaitingCost;
    SetTimingCost(del, arr_node, real_del_time, EarlyTW(del),
                  updateCost);
    ClientWaitingTime[del->id] = updateCost.clientWaitingCost - ClientWaitingTime[del->id];
    TruckWaitingTime[del->id] = updateCost.truckWaitingCost - TruckWaitingTime[del->id];

    arr_node = std::max(arr_node, real_del_time);
    StartServiceTime[del->id] = arr_node;
    arr_node += UNLOADING_DURATION;
    EndServiceTime[del->id] = arr_node;
    assert(EndServiceTime[del->id] > 0);
    assert(arr_node > 0);
    updateCost.lateDeliveryCost += Sol::GetLateDeliveryCost(del, arr_node, LateTW(del));

    NodeLateDelivery[del->id] = updateCost.lateDeliveryCost - NodeLateDelivery[del->id];
    OrderLateDelivery[del->orderID] += NodeLateDelivery[del->id];
    arr_node += Data::CleaningTime(del, d);
    DepartureTime[del->id] = arr_node;

    ArrivalTime[d->EndNodeID] = arr_node +
                                Travel(del->distID, d->distID);
    shiftDuration[d->id] = ArrivalTime[d->EndNodeID] - d->start_shift_time;

    d->shiftDuration = shiftDuration[d->id];
    updateCost.overTimeCost -= d->overTime;
    d->overTime = Sol::GetOvertimeCost(d->shiftDuration);
    updateCost.overTimeCost += d->overTime;

    updateCost.underWorkCost -= d->underWork;
    d->underWork = Sol::GetUnderWorkCost(d->shiftDuration);
    updateCost.underWorkCost += d->underWork;

    updateCost.waitingCost = updateCost.clientWaitingCost + updateCost.truckWaitingCost;

    TimeSlot intv = TimeSlot(ArrivalTime[dock->id], DepartureTime[del->id], 'D', *del);
    nodeServiceIntervals[del->id] = intv;
    driverWorkingIntervals[d->id].insert(intv);
    nodeServiceIntervals[dock->id] = loadSlot;
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
//    for (int i = 0; i < GetCustomerCount(); i++) {
//        Customer *c = GetCustomer(i);
//        _last_cost.undeliveredCost += !(isClientSatisfied(c)) * c->demand;
//        _last_cost.satisfiedCost += (isClientSatisfied(c)) * c->demand;
//        if (isClientSatisfied(c)) {
//            _last_cost.waste += std::abs(clientCapRestante[c->custID]);
//        }
//    }

    for (int i = 0; i < GetOrderCount(); i++) {
        Order *o = GetOrder(i);
        _last_cost.undeliveredCost += !(isOrderSatisfied(o)) * o->demand * Parameters::UNDELIVERY_PENALTY;
        _last_cost.satisfiedCost += (isOrderSatisfied(o)) * o->demand;
        if (isOrderSatisfied(o)) {
            _last_cost.waste += std::abs(orderCapRestante[o->orderID]);
        }
    }


    _last_cost.driverUsed = 0;
    _last_cost.underWorkCost = 0;
    _last_cost.overTimeCost = 0;
    for (int i = 0; i < GetDriverCount(); i++) {
        Driver *d = GetDriver(i);
        if (RoutesLength[d->id] > 0) {
            _last_cost.driverUsed++;
        }
        _last_cost.overTimeCost += Sol::GetOvertimeCost(shiftDurationCost[d->id]);
        _last_cost.underWorkCost += Sol::GetUnderWorkCost(shiftDurationCost[d->id]);
    }
    _last_cost.setTotal();
    computeLastCost = true;
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
                if (!VisitFlagCost[del->id])
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
    double real_del_time = EndServiceTime[prec_del->id];
    Node *lastOfDriver = DriverPrev[dock->id];
    cur_cost.travelCost += Travel(lastOfDriver, dock) +
                           Travel(dock, del) +
                           Travel(del->distID, d->distID) -
                           Travel(lastOfDriver->distID, d->distID);

    double arr_node = ArrivalTime[del->id];

    Sol::SetTimingCost(del, arr_node, real_del_time, EarlyTW(del),
                       cur_cost);

    arr_node = StartServiceTime[del->id];
    arr_node += UNLOADING_DURATION;

    cur_cost.lateDeliveryCost += Sol::GetLateDeliveryCost(del, arr_node, LateTW(del));
    arr_node += Data::CleaningTime(del, d);


    shiftDurationCost[d->id] = arr_node + Travel(del->distID, d->distID) - d->start_shift_time;
    d->shiftDurationCost = shiftDurationCost[d->id];
    cur_cost.overTimeCost +=
            Parameters::OVERTIME_PENALTY *
            int(shiftDurationCost[d->id] > Parameters::NORMAL_WORKING_TIME);
    cur_cost.underWorkCost += Sol::GetUnderWorkCost(shiftDurationCost[d->id]);

//            Parameters::UNDERWORK_PENALTY *
//            int(shiftDurationCost[d->id] <  Parameters::MIN_WORKING_TIME);

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
    cout << *d << endl;
    printf("%d cap %2.0lf len %d:\n", d->id, d->capacity, (int) RoutesLength[d->id]);
    Node *cur = GetNode(d->StartNodeID);
    double dist = 0;
    while (cur->type != Parameters::END_LINK) {
        dmd += cur->demand;
        Node *next = DriverNext[cur->id];
        dist += Travel(cur, next);
        if (cur->type != Parameters::START_LINK) {
            printf("%c%d(%2.0lf->%2.0lf)-", cur->c, cur->id, StartServiceTime[cur->id],
                   EndServiceTime[cur->id]);
            if (cur->type == Parameters::DELIVERY) {
                auto *del = dynamic_cast<Delivery *>(cur);
                printf("C%d-", del->custID);
            }
        } else {
            printf("%d-", cur->id);
        }
        cur = next;
    }
    Cost value = GetCost(d);

    printf("Driver %d: load:%2.lf cap:%2.0lf cost:%2.0lf dist: %2.0lf \n", d->id, dmd, d->capacity,
           value.getTotal(), dist);
}


void Sol::UpdateDepotLoadingSet(Depot *dep, Dock *dock, TimeSlot const &intv) {

    auto it = std::find_if(
            depotLoadingIntervals[dep->depotID].begin(),
            depotLoadingIntervals[dep->depotID].end(),
            [&intv](const TimeSlot &i1) { return ((i1.nodeID == intv.nodeID) and i1.nodeID != -1); });

    if (it != depotLoadingIntervals[dep->depotID].end()) {
        depotLoadingIntervals[dep->depotID].erase(it);
    }
    auto it_load = depotLoadingIntervals[dep->depotID].find(intv);
    if (it_load != depotLoadingIntervals[dep->depotID].end()) {
        cout << "1 " << *it_load << intv << endl;
        cout << *dock << endl;
        cout<<"Slot occupé update\n";
        ShowSlot(dep);
        exit(1);
    }
    depotLoadingIntervals[dep->depotID].insert(intv);
    DepotSize[dep->depotID] = (int) depotLoadingIntervals[dep->depotID].size();
    DepotAssignTo[dock->id] = dep;
}


void Sol::FindEmptySlot(std::set<TimeSlot> const &SlotSet, TimeSlot &slot, const double duration) {
    auto it_load = SlotSet.find(slot);
//    if(Parameters::SHOW)
    if (it_load != SlotSet.end()) {
//        cout << "Debut " << slot << endl;
        bool forward = (it_load->n.siteID == slot.n.siteID);
        if ((Parameters::LOAD_INSERTION == Parameters::DEPOTINSERTION::FORWARD) ||
            forward) {
            FindForwardSlot(SlotSet, slot, duration);
        } else if (Parameters::LOAD_INSERTION == Parameters::DEPOTINSERTION::BACKWARD) {
            FindBackwardSlot(SlotSet, slot, duration);
        } else if (Parameters::LOAD_INSERTION == Parameters::DEPOTINSERTION::RANDOM) {

            mat_func_get_rand_double() <= 0.5 || forward ? FindForwardSlot(SlotSet, slot, duration) : FindBackwardSlot(SlotSet,
                                                                                                            slot,
                                                                                                            duration);
//            if (mat_func_get_rand_double() <= 0.5) {
//                FindForwardSlot(SlotSet,slot,duration);
//            } else {
//                FindBackwardSlot(SlotSet,slot,duration);
//            }
        }
    }
}

void Sol::FindForwardSlot(std::set<TimeSlot> const &SlotSet, TimeSlot &slot, const double duration) {
    auto it_load = SlotSet.find(slot);
//    if(Parameters::SHOW)
    if (it_load != SlotSet.end()) {
//        cout << "Debut " << slot << endl;

        for (auto cur_it = it_load; cur_it != SlotSet.end();) {
            auto next_it = std::next(cur_it);
            slot.lower = cur_it->upper;
            slot.upper = slot.lower + duration;

            if (next_it != SlotSet.end()) {
                if (slot.upper <= next_it->lower) {
                    break;
                }
            } else {
                break;
            }
            cur_it = next_it;
        }
    }
//    if(Parameters::SHOW)
}

void Sol::FindBackwardSlot(std::set<TimeSlot> const &SlotSet, TimeSlot &slot, const double duration) {
    auto it_load = SlotSet.find(slot);
//    if(Parameters::SHOW)
    if (it_load != SlotSet.end()) {
//        cout << "Debut " << slot << endl;
        bool find = false;
        for (auto cur_it = it_load; cur_it != SlotSet.begin();) {
            auto prev_it = std::prev(cur_it);
            slot.upper = cur_it->lower;
            slot.lower = slot.upper - duration;
            if (prev_it != SlotSet.begin()) {
                if (slot.lower >= prev_it->upper) {
                    find = true;
                    break;
                }
            } else {
                slot.upper = prev_it->lower;
                slot.lower = slot.upper - duration;
                find = true;
                break;
            }
            cur_it = prev_it;
        }
        if (!find) {
            slot.upper = it_load->lower;
            slot.lower = slot.upper - duration;
        }

//        cout << "End " << slot << endl;
//        Prompt::print(SlotSet);
    }
//    if(Parameters::SHOW)
}