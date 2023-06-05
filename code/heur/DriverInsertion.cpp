
#include "DriverInsertion.h"
#include "../Prompt.h"
#include <iostream>

using namespace std;

void DriverInsertion::Insert(Sol &s) {
    Sol::InitStructure(s.GetData());
    removedList.clear();
    s.heurName=this->name;
    s.Update();
    _insrmv.FillStructures(s, customersList, driversList);

    Insert(s, customersList);
    bool sortie = false;
    while (!removedList.empty() && !sortie) {
        double demand = s.updateCost.satisfiedCost;
        customersList = vector<Customer *>(removedList.begin(), removedList.end());
        Insert(s, customersList);
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
        default:
            std::shuffle(list_ID.begin(), list_ID.end(), Parameters::RANDOM_GEN);
            break;
    }
}

void DriverInsertion::Insert(Sol &s, std::vector<Customer *> const &listCust) {
    Sort(s, driversList, driverListId, DriverInsertion::_k);
    std::shuffle(customersList.begin(), customersList.end(), Parameters::RANDOM_GEN);
//    Prompt::print(custListID);
    listMoves.clear();
    listMoves.resize(s.GetDriverCount());
//    listMoves.shrink_to_fit();
    std::vector<Delivery *> unScheduledDel;
    removedList.clear();
    for (int k = 0; k < driverListId.size();) {
        Driver *d = s.GetDriver(k);
        listMoves[d->id].Clear();
        std::vector<Driver *> candidateDriver{d};
        for (auto c: listCust) {
//            cout << *c << "--" << std::endl;
            if (s.isClientSatisfied(c))
                continue;
            Order *cur_order = s.GetRandomOrder(c);

            for (int j = 0; j < s.GetDeliveryCount(cur_order); j++) {
                Delivery *del = s.GetDelivery(cur_order, j);
                if (s.GetDriverAssignedTo(del) != nullptr)
                    continue;

                if (del->rank > 0) {
                    Delivery *prec_del = s.GetDelivery(cur_order, del->rank - 1);
                    if (s.GetDriverAssignedTo(prec_del) == nullptr)
                        break;
                }
//                cout << this->name << " Try to insert del " << del->id << "(" << del->rank << ") for "<< cur_order->orderID << endl;
                std::vector<int> listId{del->delID};
                Move<Delivery, Driver, MoveVrp> mo;
                _insrmv.GetBestInsertion(s, listId, candidateDriver, mo);
                if (mo.IsFeasible) {
//                    cout<<listMoves.size()<<" "<<d->id<<" "<<listMoves[d->id].Count()<< " "<<  endl;
                    listMoves[d->id].Add(mo);
                } else {
                    unScheduledDel.push_back(del);
                    break;
                }
            }
        }
        Move<Delivery, Driver, MoveVrp> best;
        if (listMoves[d->id].Count() > 0) {
            listMoves[d->id].Sort();
            best = listMoves[d->id].Extract();
        }
        if (!best.IsFeasible) {
            k = k + 1;
        } else {
            assert(s.GetDriverAssignedTo(best.n) == nullptr);
            _insrmv.ApplyInsertMove(s, best);
            s.Update(best.move.depot, best.move.dock, best.n);
        }
    }

    for (auto del: unScheduledDel) {
        if (del->rank > 0) {
            Order *cur_order = s.GetOrder(del->orderID);
            Delivery *prec_del = s.GetDelivery(cur_order, del->rank - 1);

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
        removedList.insert(s.GetCustomer(del->custID));
    }
}

