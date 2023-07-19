
#include "CustInsertionBacktrack.h"
#include "../Prompt.h"
#include <iostream>
#include "../Combinatorial.h"

using namespace std;

void CustInsertionBacktrack::Insert(Sol &s) {
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

void CustInsertionBacktrack::Insert(Sol &s, std::vector<Customer *> &list,
                           std::vector<int> &list_ID) {
    if (list.size() > 1) {
        Sort(s, list, list_ID, CustInsertionBacktrack::_k);
//        Prompt::print(list,",");
//        Prompt::print({to_string(list.size())});
//        Parameters::SHOW = true;
    }
    removedList.clear();
    removedListId.clear();
    listMoves.clear();
    listMoves.resize(s.GetDeliveryCount());
    std::shuffle(driversList.begin(), driversList.end(), Parameters::RANDOM_GEN);
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

         int iter_count = 0;
//        if (cur_order->custID == 18)
//         Parameters::SHOW = true;
         for (int j = 0; j < s.GetDeliveryCount(cur_order);) {
             iter_count++;

             if (iter_count >= 100 and j == 0) {
//                 cout << iter_count << endl;
//                 s.ShowSchedule(c);
//                 cout << *c << endl;
//                 Parameters::SHOW = true;

//                exit(1);

             }
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
                    if (Parameters::SHOW) {
                        Prompt::print({"Push", to_string(del->id), "by", to_string(Sol::minDelay[del->id])});
                    }
                    ListMoveVrp temp_moves;
                    if(del->rank==0){
//                        cout<< "Push "<<del->id<<" "<< s.updateCost<<endl;
//                        s.ShowSchedule(c);

                    }
                    _insrmv.GetBestInsertion(s, listId, driversList, &temp_moves);
                    if (temp_moves.Count() > 0) {
                        listMoves[del->delID].Insert(temp_moves);
                        if(listMoves[del->delID].Count() >8){
                            Prompt::print({"Push", to_string(del->id), "by", to_string(Sol::minDelay[del->id])});
                        }
                    }
                    Sol::minDelay[del->id] = 0;
                }
            } else if (listMoves[del->delID].Count() == 0) {
                if(del->rank==0){
//                    cout<<"new "<<del->id<<" "<< s.updateCost<<endl;
//                    s.ShowSchedule(c);
                }
                _insrmv.GetBestInsertion(s, listId, driversList, &listMoves[del->delID]);
            }
             if(listMoves[del->delID].Count() >8){
//                 Parameters::SHOW=true;
             }
             if (Parameters::SHOW) {
                 Prompt::print({to_string(listMoves[del->delID].Count()), "moves for del", to_string(del->id)});
                 listMoves[del->delID].Show();
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
                                if (std::min(delay, Parameters::INTRA_ORDER_DELIVERY) - s.WaitingTime[prec_del->id] >
                                    Parameters::INTRA_ORDER_DELIVERY) {
//                                    backtrackOrder=false;
                                        count--;
                                    if (prec_del->rank > 0) {
//                                        s.UnassignDelivery({prec_del});
                                        prec_del = s.GetDelivery(cur_order, prec_del->rank - 1);
                                        continue;
                                    } else
                                    {
                                        break;
                                    }
                                }
                            }
                            Sol::minDelay[prec_del->id] = std::min(delay, Parameters::INTRA_ORDER_DELIVERY);
                            Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
                            prec_del->isdelayed = true;
//                            break;
//                            s.UnassignDelivery({prec_del});
                            backtrackOrder = true;
                            count--;
                            delay = delay - Parameters::INTRA_ORDER_DELIVERY;
                            if (prec_del->rank > 0) {
                                prec_del = s.GetDelivery(cur_order, prec_del->rank - 1);
                            } else break;
                        }
//                        Sol::minDelay[prec_del->id] = Sol::pushVisit[del->id];
                        Sol::minDelay[del->id] = 0;
                        Sol::pushVisit[del->id] = 0;
                        Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                        listMoves[del->delID].Clear();

                        if (backtrackOrder) {
                            int waste = s.updateCost.waste;
                            s.UnassignDelivery(s.GetDeliveries(cur_order,count,del->rank-1));
                            assert(waste > s.updateCost.waste);
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

void CustInsertionBacktrack::Sort(Sol &s, std::vector<Customer *> &list,
                         std::vector<int> &list_ID, int k) {
    switch (k) {
        case -1: {
//            std::vector<int> vec{18, 19, 12, 16, 17, 1, 0, 4, 3, 2, 5, 6, 9, 10, 7, 8, 11, 14, 15, 13};
//            std::vector<int> vec{0,1,2,3,4,8,9};
            std::vector<int> vec{18,19};
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

CustInsertionBacktrack::~CustInsertionBacktrack() = default;
