
#include "CustInsertion.h"
#include "../Prompt.h"
#include <iostream>
#include "../Combinatorial.h"

using namespace std;

void CustInsertion::Insert(Sol &s) {
    Sol::InitStructure(s.GetData());
    removedList.clear();
    s.Update();
    _insrmv.FillStructures(s, customersList, driversList);
    assert(!customersList.empty());
    const int insertion_type = Parameters::LOAD_INSERTION;
    InsertWithBactrack(s, customersList);
    bool beginLoop = false;
    while (!removedList.empty() && beginLoop) {
        double demand = s.updateCost.satisfiedCost;
        customersList = removedList;
        Sol cur(s);
        Parameters::LOAD_INSERTION = Parameters::DEPOTINSERTION::BACKWARD;

        InsertWithBactrack(cur, customersList);
        if (cur < s) {
            s = cur;
        }
        beginLoop = (cur.updateCost.satisfiedCost > demand);
    }
    Parameters::LOAD_INSERTION = insertion_type;
    s.Update();
}

void CustInsertion::InsertWithBactrack(Sol &s, std::vector<Customer *> &list) {
    if (list.size() > 1) {Sort(s, list, CustInsertion::_k);}
    removedList.clear();
    removedList.shrink_to_fit();
    listMoves.clear();
    listMoves.shrink_to_fit();
    listMoves.resize(s.GetDeliveryCount());
    std::shuffle(driversList.begin(), driversList.end(), Parameters::RANDOM_GEN);
    Customer *c = list[0];
    set<int> UsedOrder;
    bool nextCustomer = false;
    for (int i = 0; i < list.size();) {
        if (nextCustomer) {
            if (i == list.size() - 1) break;
            i++;
            c = list[i];
            UsedOrder.clear();
            nextCustomer = false;
        }
        Order *cur_order = s.GetRandomOrder(c, UsedOrder);
        if (cur_order == nullptr) {
            nextCustomer = true;
            s.UnassignCustomer(c);
            continue;
        }
        UsedOrder.insert(cur_order->orderID);
        nextCustomer = Insert(s, c, cur_order);
    }
}

bool CustInsertion::Insert(Sol &s, Customer *c, Order *cur_order) {
    bool nextCustomer = false;
    int depth = 0;
//    if (c->custID == 5)
//        Parameters::SHOW = true;
    for (int j = 0; j < s.GetDeliveryCount(cur_order);) {
        Delivery *del = s.GetDelivery(cur_order, j);
        if (del == nullptr) break;
        if (s.DriverAssignTo[del->id] != nullptr) {
            j++;
            continue;
        }
        if (Parameters::SHOW) {
            Prompt::print({this->name, " Try to insert del ", to_string(del->id), "(", to_string(del->rank + 1), "of ",
                           to_string(s.GetDeliveryCount(cur_order)), " for", to_string(cur_order->orderID)});
        }

        if (depth++ > Parameters::BACKTRACK_DEPTH * s.GetDeliveryCount(cur_order)) {
            nextCustomer = true;
            s.UnassignCustomer(c);
            break;
        }
        std::vector<int> listId{del->delID};
        if (listMoves[del->delID].Count() > 0) {
            if (Sol::minDelay[del->id] > 0) {
                if (Parameters::SHOW) {
                    Prompt::print({"Push", to_string(del->id), "by", to_string(Sol::minDelay[del->id])});
                }
                ListMoveVrp temp_moves;
                _insrmv.GetBestInsertion(s, listId, driversList, &temp_moves);
                if (temp_moves.Count() > 0) {listMoves[del->delID].Insert(temp_moves);}
                Sol::minDelay[del->id] = 0;
            }
        } else if (listMoves[del->delID].Count() == 0) {
            if (Parameters::SHOW) {
                Prompt::print({"Find insertions for", to_string(del->id), "with delay of ",
                               to_string(Sol::minDelay[del->id])});
            }
            _insrmv.GetBestInsertion(s, listId, driversList, &listMoves[del->delID]);
        }
        if (Parameters::SHOW) {
            Prompt::print({to_string(listMoves[del->delID].Count()), "moves for del", to_string(del->id)});
        }
//            listMoves[del->delID].Show();
        Move<Delivery, Driver, MoveVrp> best;
        if (listMoves[del->delID].Count() > 0) { best = listMoves[del->delID].Extract(); }
        if (!best.IsFeasible) {
            if (del->rank > 0) {
                Delivery *prec_del = s.GetDelivery(cur_order, del->rank - 1);
                bool backtrackOrder = false;
                if (Sol::FailureCount[del->id] < 4 &&
                    Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::DELAY) {
                    double delay = Sol::pushVisit[del->id];
                    int count = j;
                    while (delay > 0) {
                        if (s.WaitingTime[prec_del->id] < 0) {
                            if (std::min(delay, (double) s.GetTimeBtwDel(prec_del)) - s.WaitingTime[prec_del->id] >
                                s.GetTimeBtwDel(prec_del)) {
                                count--;
                                if (prec_del->rank > 0) {
                                    prec_del = s.GetDelivery(cur_order, prec_del->rank - 1);
                                    continue;
                                } else {
                                    break;
                                }
                            }
                        }
                        Sol::minDelay[prec_del->id] = std::min(delay, (double) s.GetTimeBtwDel(prec_del));
                        Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
                        if (Parameters::SHOW) {
                            Prompt::print({"Return to and delay", to_string(prec_del->id), "by", to_string(delay)});
                        }
                        backtrackOrder = true;
                        count--;
                        delay = delay - s.GetTimeBtwDel(prec_del);
                        if (prec_del->rank > 0) {
                            prec_del = s.GetDelivery(cur_order, prec_del->rank - 1);
                        } else break;
                    }
                    Sol::minDelay[del->id] = 0;
                    Sol::pushVisit[del->id] = 0;
                    Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                    listMoves[del->delID].Clear();
                    if (backtrackOrder) {
                        s.UnassignDelivery(s.GetDeliveries(cur_order, count, del->rank - 1));
                        j = count;
                        if (Parameters::SHOW) {
//                                s.ShowSchedule(c);
                            Prompt::print({"return to del rank", to_string(j)});
                        }
                        continue;
                    }
                }
                backtrackOrder = false;
                prec_del = s.GetDelivery(cur_order, del->rank - 1);
                while (prec_del != nullptr) {
                    s.UnassignDelivery({prec_del});
                    j = j - 1;
                    if (listMoves[prec_del->delID].Count() > 0) {
                        if (Parameters::SHOW) {
                            Prompt::print({"return to", to_string(prec_del->id),
                                           "with remaining moves", to_string(listMoves[prec_del->delID].Count())});

                        }
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
            if (Parameters::SHOW) {
                cout << cur_order->orderID << " is not scheduled\n";
                Prompt::print(Sol::CustomerConflict[c->custID]);
//                s.ShowSchedule(c);
            }
//            s.UnassignOrder(cur_order);
            s.UnassignCustomer(c);
            removedList.emplace_back(c);
            break;
        }
        if (best.IsFeasible) {
            Sol::minDelay[best.n->id] = 0;
            _insrmv.ApplyInsertMove(s, best);
            s.Update(best.move.depot, best.move.dock, best.n);
            Delivery *next_del = s.GetNextIdleDelivery(cur_order);
            if (next_del != nullptr) {
                Sol::minDelay[next_del->id] = 0;
                Sol::FailureCause[next_del->id] = Parameters::FAILURECAUSE::NONE;
                listMoves[next_del->delID].Clear();
            }
            if (Parameters::SHOW) {
                s.ShowSchedule(best.n);
            }
            j++;
        }
        if (s.isOrderSatisfied(cur_order)) {
            if (Parameters::SHOW) {
                cout << "order " << cur_order->orderID << " is scheduled\n";
            }
            break;
        }
    }
    if (s.isClientSatisfied(c)) {
        if (Parameters::SHOW) {
            cout << "Customer " << c->custID << " is scheduled\n";
        }
        nextCustomer = true;
    }
    Parameters::SHOW = false;
    return nextCustomer;
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

        for (int j = 0; j < s.GetDeliveryCount(cur_order);) {
            Delivery *del = s.GetDelivery(cur_order, j);
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
//                s.ShowSchedule(cur_order);
                j++;
            }
            if (s.isOrderSatisfied(cur_order)) {
                break;
            }
        }
        if (s.isClientSatisfied(c)) {
        }
    }
    s.Update();
}

void CustInsertion::Sort(Sol &s, std::vector<Customer *> &list, int k) {
    switch (k) {
        case -1: {
            std::vector<int> vec{9, 8, 7, 3, 10, 0, 1, 2, 5, 6, 4};
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
            std::shuffle(list.begin(), list.end(), Parameters::RANDOM_GEN);
            break;
        case 5:
            SortNode<Customer, Driver>::radixSortKinable(list, s.GetData()->maxLateTW);
            break;
        default:
            std::shuffle(list.begin(), list.end(), Parameters::RANDOM_GEN);
            break;
    }
}

void CustInsertion::Sort(Sol &s, std::vector<int> &list_ID, int k) {
    std::vector<Customer *> custList(list_ID.size());
    int id = 0;
    for (auto val: list_ID) {
        custList[id++] = s.GetCustomer(val);
    }
    Sort(s, custList, k);
}

