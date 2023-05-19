
#include "DriverInsertion.h"
#include "../Prompt.h"
#include <iostream>

using namespace std;

void DriverInsertion::Insert(Sol &s) {
    s.Update();
    customersListId = std::set<int>();
    removedListId = std::set<int>();
    customersList.clear();
    removedList.clear();
    listMoves.clear();
    driversList.clear();
    driverListId.clear();

    if (not s.availableDrivers.empty()) {
        for (int i: s.availableDrivers) {
            driversList.push_back(s.GetDriver(i));
            driverListId.push_back(s.GetDriver(i)->id);
        }
    } else {
        Init();
    }
    if (not s.keyCustomers.empty()) {
        for (int i: s.keyCustomers) {
            Customer *c = s.GetCustomer(i);
            if (s.isClientSatisfied(c))
                continue;
            customersList.emplace_back(c);
            customersListId.insert(c->custID);
        }
    } else {
        for (int i = 0; i < s.GetCustomerCount(); i++) {
            Customer *c = s.GetCustomer(i);
            customersList.emplace_back(c);
            customersListId.insert(c->custID);
        }
    }
    std::vector<int> listId(customersListId.begin(),customersListId.end());
    Insert(s, listId);
    bool sortie = false;
    while (!removedListId.empty() && !sortie) {
        double demand = s.updateCost.satisfiedCost;
       listId.clear();
       listId = vector<int>(removedListId.begin(),removedListId.end());
        Insert(s, listId);
        // cout << s.updateCost << endl;
        sortie = (s.updateCost.satisfiedCost == demand);
    }
    s.Update();
}

void DriverInsertion::Sort(Sol &s, std::vector<Driver *> &list,
                           std::vector<int> &list_ID, int k) {
    switch (k) {
        case 0:
            std::stable_sort(list_ID.begin(), list_ID.end(), [&s](int a, int b) {
                return s.GetDriver(a)->capacity <
                       s.GetDriver(b)->capacity;
            });
            break;
        case 1:
            std::stable_sort(list_ID.begin(), list_ID.end(), [&s](int a, int b) {
                return s.GetDriver(a)->capacity >
                       s.GetDriver(b)->capacity;
            });
            break;
        case 3:
            std::shuffle(list_ID.begin(), list_ID.end(), Parameters::RANDOM_GEN);
            break;
        default:
            std::shuffle(list_ID.begin(), list_ID.end(), Parameters::RANDOM_GEN);
            break;
    }
}

void DriverInsertion::Insert(Sol &s, std::vector<int> &custListID) {
    Sort(s, driversList, driverListId, DriverInsertion::_k);
    std::shuffle(custListID.begin(),custListID.end(),Parameters::RANDOM_GEN);
    Prompt::print(custListID);
    listMoves.clear();
    listMoves.resize(s.GetDriverCount());
    std::vector<Delivery*> unScheduledDel;
    removedList.clear();
    removedListId.clear();
    for (int k=0;k < driverListId.size();) {
        Driver *d = s.GetDriver(k);
        listMoves[d->id].Clear();
        std::vector<Driver *> candidateDriver{d};
        for (auto custId: custListID) {
            Customer *c=s.GetCustomer(custId);
//            cout << *c << "--" << std::endl;
            if (s.isClientSatisfied(c))
                continue;
            Order *cur_order = s.GetRandomOrder(c);

            for (int j = 0; j < s.GetDeliveryCount(cur_order); j++) {
                Delivery *del = s.GetDelivery(cur_order, j);
                if (s.GetDriverAssignedTo(del) != nullptr)
                    continue;

                if(del->rank>0){
                    Delivery *prec_del = s.GetDelivery(cur_order, del->rank-1);
                    if (s.GetDriverAssignedTo(prec_del) == nullptr)
                        break;
                }
//                cout << this->name << " Try to insert del " << del->id << "(" << del->rank << ") for "<< cur_order->orderID << endl;
                std::vector<int> listId{del->delID};
                Move<Delivery, Driver, MoveVrp> mo;
                _insrmv.GetBestInsertion(s, listId, candidateDriver, mo);
                if (mo.IsFeasible) {
                    listMoves[d->id].Add(mo);
                }
                else
                {
                    unScheduledDel.push_back(del);
                    break;
                }
            }
        }
        Move<Delivery, Driver, MoveVrp> best;
        if (listMoves[d->id].Count() > 0) {
        listMoves[d->id].Sort();
//        listMoves[d->id].Show();
            best = listMoves[d->id].Extract();
        }
        if (!best.IsFeasible) {
            k=k+1;
        } else {
            assert(s.GetDriverAssignedTo(best.n)== nullptr);
            _insrmv.ApplyInsertMove(s, best);
            s.Update(best.move.depot, best.move.dock, best.n);
        }
    }

    for(auto del:unScheduledDel){
        if( del->rank>0) {
            Order *cur_order = s.GetOrder(del->orderID);
            Delivery *prec_del = s.GetDelivery(cur_order,del->rank-1);

            if (Sol::FailureCount[del->id] < 3 &&
                Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::DELAY) {
                Sol::minDelay[prec_del->id] = Sol::pushVisit[del->id];
                Sol::minDelay[del->id] = 0;
                Sol::pushVisit[del->id] = 0;
                Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
//                  cout<<" Push "<<prec_del->id << " by "<<Sol::minDelay[prec_del->id]<<endl;
//                        s.ShowSchedule(cur_order);
                s.UnassignDelivery({prec_del});
//                cout<<"Uninstall "<<prec_del->id<< endl;
                prec_del->isdelayed = true;
            }
        }
        removedListId.insert(del->custID);
    }

//    s.ShowCustomer();
//     cout<<s.GetCost()<<std::endl;
}

void DriverInsertion::Insert(Sol &s, std::vector<int> const &list) {
    Sol::FailureCause.resize(s.GetNodeCount(), Parameters::FAILURECAUSE::NONE);
    Sol::FailureCount.resize(s.GetNodeCount(), 0);
    Sol::minDelay.resize(s.GetNodeCount(), 0);
    Sol::StartBefore.resize(s.GetNodeCount(), 0);
    Sol::pullVisit.resize(s.GetNodeCount(), 0);
    Sol::pushVisit.resize(s.GetNodeCount(), 0);
    TimeSlot::myData = *s.GetData();
    s.Update();
    listMoves.clear();
    listMoves.resize(s.GetDeliveryCount());
    std::shuffle(driversList.begin(), driversList.end(), Parameters::RANDOM_GEN);
    for (auto custID: list) {
//        cout<<*c<<"--"<<std::endl;
        Customer *c = s.GetCustomer(custID);
        if (s.isClientSatisfied(c))
            continue;
        Order *cur_order = s.GetRandomOrder(c);

        // std::shuffle(_ins_rmv_perators.begin(), _ins_rmv_perators.end(), Parameters::RANDOM_GEN);
        for (int j = 0; j < s.GetDeliveryCount(cur_order);) {
            Delivery *del = s.GetDelivery(cur_order, j);
//             cout << this->name << " Try to insert del " << del->id << "("<<del->rank<<") for "<<cur_order->orderID<< endl;
            Move<Delivery, Driver, MoveVrp> best;
            std::vector<int> listId{del->delID};
            if (listMoves[del->delID].Count() == 0) {
                _insrmv.GetBestInsertion(s, listId, driversList, &listMoves[del->delID]);

                if (listMoves[del->delID].Count() > 0) {
                    best = listMoves[del->delID].Extract();
                }
            } else if (listMoves[del->delID].Count() > 0) {
                best = listMoves[del->delID].Extract();
            }
//            cout<< listMoves[del->delID].Count()<<" moves remaining for "<<del->id<<endl;
            if (!best.IsFeasible) {
                if (Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::DRIVERBUSY) {
//                    cout<<"driver is busy for "<<del->id<<"("<<c->custID<<")"<<" rank: "<<del->rank<<" "<<endl;
//                    s.ShowSchedule();
//                    s.ShowSchedule(cur_order);
                }
                if (del->rank > 0) {
                    Delivery *prec_del = s.GetDelivery(cur_order, del->rank - 1);

                    if (Sol::FailureCount[del->id] < 3 &&
                        Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::DELAY) {
                        Sol::minDelay[prec_del->id] = Sol::pushVisit[del->id];
                        Sol::minDelay[del->id] = 0;
                        Sol::pushVisit[del->id] = 0;
                        Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                        Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
//                        cout<<" Push "<<prec_del->id << " by "<<Sol::minDelay[prec_del->id]<<endl;
//                        s.ShowSchedule(cur_order);
                        s.UnassignDelivery({prec_del});
//                        cout<<"Uninstall "<<prec_del->id<< endl;

                        prec_del->isdelayed = true;
                        listMoves[del->delID].Clear();
                        j = j - 1;
                        continue;
                    }
                    if (Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::LATETW) {
                        Sol::StartBefore[prec_del->id] = s.StartServiceTime[prec_del->id] - Sol::pullVisit[del->id];
                        Sol::StartBefore[del->id] = 0;
                        Sol::pullVisit[del->id] = 0;
                        Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                        Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
//                    Sol::FailureCount[del->id]=0;
//                        cout<<" Start  "<<prec_del->id << " before "<<Sol::StartBefore[prec_del->id]<<endl;
//                        s.ShowSchedule(cur_order);
//                    prec_del->isdelayed = true;
                        listMoves[del->delID].Clear();
                        int index = -1;
                        for (int rank = j - 1; rank >= 0; rank--) {
                            Delivery *temp = s.GetDelivery(cur_order, rank);
                            s.UnassignDelivery({temp});
                            if (listMoves[temp->delID].Count() == 0) {
                                continue;
                            }
                            index = rank;
                            break;
                        }
                        if (index != -1) {
                            j = index;
                            continue;
                        }
                    }
                    if (listMoves[prec_del->delID].Count() > 0) {
                        j = j - 1;
                        s.UnassignDelivery({prec_del});
                        continue;
                    }
                }
                s.UnassignOrder(cur_order);
                break;
            }
            if (best.IsFeasible) {
                _insrmv.ApplyInsertMove(s, best);
                s.Update(best.move.depot, best.move.dock, best.n);
                assert(Sol::FailureCount[del->id] == 0);
//                s.ShowSchedule(cur_order);
                j++;
            }
            if (s.isOrderSatisfied(cur_order)) {
                break;
            }
        }
        if (s.isClientSatisfied(c)) {
//            s.ShowSchedule(cur_order);
//            cout<<"Insert "<<c->custID<<endl;

        }
//        s.ShowSchedule(cur_order);

    }
//    s.ShowCustomer();
    s.Update();
}


DriverInsertion::~DriverInsertion() = default;
