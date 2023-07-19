
#include "CustInsertion.h"
#include "../Prompt.h"
#include <iostream>
#include "../Combinatorial.h"

using namespace std;

void CustInsertion::Insert(Sol &s) {
    Sol::InitStructure(s.GetData());
    customersList.clear();
    customersListId.clear();
    s.Update();
    if (not s.availableDrivers.empty()) {
        driversList.clear();
        for (int i: s.availableDrivers) {
            driversList.push_back(s.GetDriver(i));
        }
    } else {
        driversList.clear();
        Init();
    }
    if (not s.keyCustomers.empty()) {
        for (int i: s.keyCustomers) {
            Customer *c = s.GetCustomer(i);
            if (s.isClientSatisfied(c))
                continue;
            customersList.emplace_back(c);
            customersListId.emplace_back(c->custID);
        }
    } else {
        for (int i = 0; i < s.GetCustomerCount(); i++) {
            Customer *c = s.GetCustomer(i);
            customersList.emplace_back(c);
            customersListId.emplace_back(c->custID);
        }
    }
    Insert(s, customersList, customersListId);
    bool sortie = false;
    while (!removedList.empty() && !sortie) {
        double demand = s.updateCost.satisfiedCost;
        // cout << s.updateCost << endl;
        customersList = removedList;
        customersListId = removedListId;
        Insert(s, customersList, customersListId);
        // cout << s.updateCost << endl;
        sortie = (s.updateCost.satisfiedCost == demand);
    }
    s.Update();
}


void CustInsertion::Insert(Sol &s, std::vector<Customer *> &list,
                           std::vector<int> &list_ID) {
    if (list.size() > 1) {
        Sort(s, list, list_ID, CustInsertion::_k);
//        Prompt::print(list,",");
//        Prompt::print({to_string(list.size())});
//        Parameters::SHOW = true;
    }
    removedList.clear();
    removedListId.clear();
    listMoves.clear();
    listMoves.resize(s.GetDeliveryCount());
//TODO    std::shuffle(driversList.begin(), driversList.end(), Parameters::RANDOM_GEN);
    Customer *c = list[0];
//    c->early_tw=144;
     for (int i = 0; i < list.size();) {
//        cout << *c << "--" << std::endl;
        bool custom_client = false;
        if (s.isClientSatisfied(c)) {
            if (i == list.size() - 1) break;
            i++;
            c = list[i];
            continue;
        }
        Order *cur_order = s.GetRandomOrder(c);
        for (int j = 0; j < s.GetDeliveryCount(cur_order);) {
            Delivery *del = s.GetDelivery(cur_order, j);
            if (Parameters::SHOW) {
                Prompt::print(
                        {this->name, " Try to insert del ", to_string(del->id), "(", to_string(del->rank), ") for",
                         to_string(cur_order->orderID)});
            }
//            if (cur_order->demand==45 )
            std::vector<int> listId{del->delID};

            if (listMoves[del->delID].Count() > 0) {
                if (Sol::minDelay[del->id] > 0) {
                    ListMoveVrp temp_moves;
                    _insrmv.GetBestInsertion(s, listId, driversList, &temp_moves);
                    if (temp_moves.Count() > 0) {
                        listMoves[del->delID].Insert(temp_moves);
                    }
                    Sol::minDelay[del->id] = 0;
                }
            } else if (listMoves[del->delID].Count() == 0) {
                _insrmv.GetBestInsertion(s, listId, driversList, &listMoves[del->delID]);
            }
            Move<Delivery, Driver, MoveVrp> best;
            if (listMoves[del->delID].Count() > 0) {
                best = listMoves[del->delID].Extract();
            }
            if (best.FailureCause != Parameters::FAILURECAUSE::NONE) {
                if (best.FailureCause == Parameters::FAILURECAUSE::DELAY) {
//                    cout << "Delay " << best.move.prevMaxTime << endl;
                    auto cur_del = dynamic_cast<Delivery *>(best.move.prev);
//                    best.move.prev->Show();
//                    s.ShowSchedule(cur_del);
                    listMoves[del->delID].Clear();
                    Sol::minDelay[cur_del->id] = best.move.prevMaxTime;
                    Sol::minDelay[del->id] = 0;
                    if (cur_del->custID == del->custID) {
                        s.UnassignDelivery({cur_del});
                        j = cur_del->rank;
                        continue;
                    }
                } else if (best.FailureCause == Parameters::FAILURECAUSE::LATETW) {
                    cout << "latetw" << endl;
                    auto temp_del = dynamic_cast<Delivery *>(best.move.prev);
                    auto temp_order = s.GetOrder(temp_del->orderID);
                    for (int rank = temp_del->rank; rank < temp_order->nbDelMax; rank++) {
                        Delivery *temp = s.GetDelivery(temp_order, rank);
                        if (s.GetDriverAssignedTo(temp) == nullptr) break;
                        s.UnassignDelivery({temp});
                    }
                    c = s.GetCustomer(temp_del->custID);
                    custom_client = true;
                    break;
                }
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
                    bool backtrackOrder = false;
                    if (Sol::FailureCount[del->id] < 3 &&
                        Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::DELAY) {

                        int delay = Sol::pushVisit[del->id];
                        int count = j;
//                        cout<<"Delay"<<endl;
                        while (delay > 0) {
                            if (s.WaitingTime[prec_del->id] < 0) {
//                                cout<<"Waiting "<<  s.WaitingTime[prec_del->id]<<endl;
                                if (std::min(delay, Parameters::TIME_BTW_DELIVERY) - s.WaitingTime[prec_del->id] >
                                    Parameters::TIME_BTW_DELIVERY) {
//                                    backtrackOrder=false;
                                    s.UnassignDelivery({prec_del});
                                    if (prec_del->rank > 0) {
                                        prec_del = s.GetDelivery(cur_order, prec_del->rank - 1);
                                        count--;
                                        continue;
                                    } else
                                        break;
                                }
                            }
                            Sol::minDelay[prec_del->id] = std::min(delay, Parameters::TIME_BTW_DELIVERY);
                            Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;


                            prec_del->isdelayed = true;
//                            break;
                            s.UnassignDelivery({prec_del});
                            backtrackOrder = true;
                            count--;
                            delay = delay - Parameters::TIME_BTW_DELIVERY;
                            if (prec_del->rank > 0) {
                                prec_del = s.GetDelivery(cur_order, prec_del->rank - 1);
                            } else break;
                        }
//                        Sol::minDelay[prec_del->id] = Sol::pushVisit[del->id];
                        Sol::minDelay[del->id] = 0;
                        Sol::pushVisit[del->id] = 0;
                        Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                        listMoves[del->delID].Clear();
//                        cout<<" Push "<<prec_del->id << " by "<<Sol::minDelay[prec_del->id]<<endl;
//                        s.ShowSchedule(cur_order);
//                        cout<<"Uninstall "<<prec_del->id<< endl;
//                        j = j - 1;
                        if (backtrackOrder) {
                            j = count;
//                            if(j==0)cout<<"Retour au debut\n";
                            continue;
                        }
                    }
                    if (Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::LATETW) {
                        Sol::StartBefore[prec_del->id] = s.StartServiceTime[prec_del->id] - Sol::pullVisit[del->id];
                        Sol::StartBefore[del->id] = 0;
                        Sol::pullVisit[del->id] = 0;
                        Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                        Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
//                      Sol::FailureCount[del->id]=0;
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
                    if (Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::PUSH_PREC_CUSTOMER) {
//                        Prompt::print({to_string(c->custID), to_string(Sol::CustomerConflict[c->custID])});
//                        assert(Sol::CustomerConflict[c->custID] >= 0);
//
//                        s.UnassignOrder(cur_order);
//                        removedList.emplace_back(c);
//                        removedListId.emplace_back(c->custID);
//
//                        c = s.GetCustomer(Sol::CustomerConflict[c->custID]);
//                        custom_client = true;
//                        s.UnassignCustomer(c);
////                        s.ShowSchedule(c);
                        break;
                    }
                    backtrackOrder = false;
                    prec_del = s.GetDelivery(cur_order, del->rank - 1);
//                    cout<<"here\n";
                    while (prec_del != nullptr) {
                        s.UnassignDelivery({prec_del});
                        j = j - 1;
                        if (listMoves[prec_del->delID].Count() > 0) {
//                            Prompt::print({to_string(prec_del->id),"has moves"});
                            backtrackOrder = true;
                            break;
                        }
                        if (prec_del->rank > 0) {
                            prec_del = s.GetDelivery(cur_order, prec_del->rank - 1);
                        } else break;
                    }
                    if (backtrackOrder)
                        continue;
                }

//                s.ShowSchedule(cur_order);
                s.UnassignOrder(cur_order);
                removedList.emplace_back(c);
                removedListId.emplace_back(c->custID);
                break;
            }

            if (best.IsFeasible) {
                Sol::minDelay[best.n->id] = 0;
                _insrmv.ApplyInsertMove(s, best);
                s.Update(best.move.depot, best.move.dock, best.n);
                assert(Sol::FailureCount[del->id] == 0);
                Delivery *next_del = s.GetNextIdleDelivery(cur_order);
                if (next_del != nullptr) {
                    Sol::minDelay[next_del->id] = 0;
                    Sol::FailureCause[next_del->id] = Parameters::FAILURECAUSE::NONE;
                    listMoves[next_del->delID].Clear();
                }
                if (Parameters::SHOW) {
                    s.ShowSchedule(cur_order);
                }
                j++;
            }
            if (s.isOrderSatisfied(cur_order)) {
                if (s.OrderLateDelivery[cur_order->orderID] > 0) {
//                    s.ShowSchedule(cur_order);
                    _insrmv.repairSolution(s, cur_order);
//                    s.ShowSchedule(cur_order);
                    if (s.OrderLateDelivery[cur_order->orderID] > 0) {
                        s.UnassignOrder(cur_order);
                        removedList.emplace_back(c);
                        removedListId.emplace_back(c->custID);
                    }
                }
                break;
            }
        }
        if (!custom_client) {
            if (s.isClientSatisfied(c)) {
                if (Parameters::SHOW) {
                    s.ShowSchedule(c);
                }
//                cout << "Insert " << c->custID << endl;
            }
            i = i + 1;
            if (i == list.size()) break;
            c = list[i];
        }
//        s.ShowSchedule(cur_order);
        Parameters::SHOW = false;
    }
//    s.ShowCustomer();
//     cout<<s.GetCost()<<std::endl;
//     exit(1);
}

void CustInsertion::Insert2(Sol &s, std::vector<Customer *> &list,
                            std::vector<int> &list_ID) {
    Sort(s, list, list_ID, CustInsertion::_k);
//    Prompt::print(list);
    removedList.clear();
    removedListId.clear();
    listMoves.clear();
    listMoves.resize(s.GetDeliveryCount());
    std::shuffle(driversList.begin(), driversList.end(), Parameters::RANDOM_GEN);
    for (auto c: list) {
//        cout<<*c<<"--"<<std::endl;
        bool end = false;
        if (s.isClientSatisfied(c))
            continue;
        Order *cur_order = s.GetRandomOrder(c);
        std::vector<int> vec_cap(s.GetData()->driverCapacities.begin(), s.GetData()->driverCapacities.end());

        std::set < std::tuple<int, std::vector<int>> > seqDrivers = Combinatorial::findCombinationsWithLimit(vec_cap,
                                                                                                             cur_order->demand);
        for (auto [val, sizeDrivers]: seqDrivers) {
            cout << val << " ";
            Prompt::print(sizeDrivers);
            std::vector<Driver *> curDrivers;
            for (auto cap: sizeDrivers) {
                curDrivers = s.GetData()->GetDrivers(cap);
            }
            for (int j = 0; j < s.GetDeliveryCount(cur_order);) {
                Delivery *del = s.GetDelivery(cur_order, j);
//             cout << this->name << " Try to insert del " << del->id << "("<<del->rank<<") for "<<cur_order->orderID<< endl;
                Move<Delivery, Driver, MoveVrp> best;
                std::vector<int> listId{del->delID};
                if (listMoves[del->delID].Count() == 0) {
                    _insrmv.GetBestInsertion(s, listId, driversList, &listMoves[del->delID]);
                }
                if (listMoves[del->delID].Count() > 0) {
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
                            Sol::StartBefore[prec_del->id] =
                                    s.StartServiceTime[prec_del->id] - Sol::pullVisit[del->id];
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
                    cout << " Cant install " << c->custID << endl;
                    s.UnassignOrder(cur_order);
                    removedList.emplace_back(c);
                    removedListId.emplace_back(c->custID);
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
                s.ShowSchedule(cur_order);
                cout << "Insert " << c->custID << endl;
                end = true;
                break;
            }
        }
        if (end)
            break;
    }
//        s.ShowSchedule(cur_order);

//    s.ShowCustomer();
//     cout<<s.GetCost()<<std::endl;
//    exit(1);
}


void CustInsertion::Insert(Sol &s, std::vector<int> const &list) {
    Sol::InitStructure(s.GetData());
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
            }
            if (listMoves[del->delID].Count() > 0) {
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

void CustInsertion::Sort(Sol &s, std::vector<Customer *> &list,
                         std::vector<int> &list_ID, int k) {
    switch (k) {
        case -1: {
//            std::vector<int> vec{18, 19, 12, 16, 17, 1, 0, 4, 3, 2, 5, 6, 9, 10, 7, 8, 11, 14, 15, 13};
//            std::vector<int> vec{0,1,2,3,4,8,9};
            std::vector<int> vec{4,2};
            list.clear();
            for (auto id: vec) {
                list.emplace_back(s.GetCustomer(id));
            }
        }
            break;
        case 0:
            SortNode<Customer, Driver>::radixSortEarlyTW(list, s.GetData()->maxEarlyTW);
            break;
        case 1:
            SortNode<Customer, Driver>::radixSortGreatDemand(list, s.GetData()->MaxDemand);
            break;
        case 2:
            SortNode<Customer, Driver>::radixSortLateTW(list, s.GetData()->maxLateTW);
            break;
        case 3:
            SortNode<Customer, Driver>::radixSortMinWidthTW(list, s.GetData()->maxLateTW - s.GetData()->minEarlyTW);
            break;
        case 4:
            std::stable_sort(list_ID.begin(), list_ID.end(), [&s](int a, int b) {
                return -s.GetCustomer(a)->late_tw <
                       -s.GetCustomer(b)->late_tw;
            });
            break;
        case 5:
            std::stable_sort(list_ID.begin(), list_ID.end(), [&s](int a, int b) {
                return s.GetCustomer(a)->demand <
                       s.GetCustomer(b)->demand;
            });
            break;
        case 6:
            std::shuffle(list.begin(), list.end(), Parameters::RANDOM_GEN);
            break;

        case 7:
            SortNode<Customer, Driver>::radixSortKinable(list, this->_data.maxLateTW);
            break;
        default:
            std::stable_sort(list_ID.begin(), list_ID.end(), [this](int a, int b) {
                return this->_data.GetCustomer(a)->early_tw <
                       this->_data.GetCustomer(b)->early_tw;
            });
            break;
    }
}

CustInsertion::~CustInsertion() = default;
