#include "PriorityQueueInsertion.h"
#include "CustInsertion.h"
#include "../Prompt.h"
#include <iostream>

using namespace std;

/**
 * @brief Insert following specified criterion
 *
 * @param s
 */
void PriorityQueueInsertion::Insert(Sol &s) {
    _data = *s.GetData();
    Sol::InitStructure(s.GetData());
    s.Update();
    removedList.clear();
    _insrmv.FillStructures(s, customersList, driversList);
    InsertByRules(s);
}

void PriorityQueueInsertion::InsertByRules(Sol &s) {

    Insert(s, customersList);
    bool beginLoop = true;
    while (!removedList.empty() && beginLoop) {
        double demand = s.updateCost.satisfiedCost;
        customersList = removedList;
        Insert(s, customersList);
        beginLoop = (s.updateCost.satisfiedCost > demand);
    }
    s.Update();
}

void PriorityQueueInsertion::Insert(Sol &s, std::vector<Customer *> &list) {
    priority_file.Vider();
    FillQueue(s, list);
    removedList.clear();
    std::shuffle(driversList.begin(), driversList.end(), Parameters::RANDOM_GEN);

    while (!priority_file.EstVide()) {
        Move<Delivery, Driver, MoveVrp> best;
        std::vector<int> listId = priority_file.GetMins();
        std::vector<int> unUsedId;
        for (auto id: listId) {
            Delivery *cur_del = s.GetDelivery(id);
            if (!s.isClientSatisfied(cur_del->custID)) {
//                cout<<"Cust "<<cur_del->custID<<"-";
//                cout<<*s.GetCustomer(cur_del->custID)<<endl;
                unUsedId.push_back(id);
            } else if (!s.isOrderSatisfied(cur_del->orderID)) {
                unUsedId.push_back(id);
//                cout<<"Cust "<<cur_del->custID<<"-";
//                cout<<*s.GetCustomer(cur_del->custID)<<endl;
            } else {
                priority_file.Supprimer(id);
            }
        }
        if (unUsedId.empty()) {
            continue;
        }
//        cout<<endl;
        _insrmv.GetBestInsertion(s, unUsedId, driversList, best);

        if (!best.IsFeasible) {
            Sol cur = s;
            cur.keyCustomers.clear();
            for (int nodeID: listId) {
                priority_file.Supprimer(nodeID);
                Delivery *del = s.GetDelivery(nodeID);
                Order *o = s.GetOrder(del->orderID);
                cur.UnassignCustomer(del->custID);
                cur.keyCustomers.insert(del->custID);
                if (del->rank > 0) {
//                    if( Sol::FailureCount[del->id] <3 && Sol::FailureCause[del->id]==Parameters::FAILURECAUSE::DELAY)
                    if (Sol::FailureCause[del->id] == Parameters::FAILURECAUSE::DELAY) {
                        Delivery *prec_del = s.GetDelivery(o, del->rank - 1);
                        Sol::minDelay[prec_del->id] = Sol::pushVisit[del->id];
                        Sol::pushVisit[del->id] = 0;
                        Sol::FailureCause[del->id] = Parameters::FAILURECAUSE::NONE;
                        Sol::FailureCause[prec_del->id] = Parameters::FAILURECAUSE::NONE;
                        //                        cout<<" Push "<<prec_del->id << " by "<<Sol::minDelay[prec_del->id]<<endl;
                        //                        s.ShowSchedule(cur_order);
                        prec_del->isdelayed = true;
                    }
                }
                //TODO remove next line
                s.UnassignOrder(o);
                removedList.emplace_back(s.GetCustomer(del->custID));
            }
            continue;
        }
        Delivery *del = s.GetDelivery(best.n->delID);
        Customer *c = s.GetCustomer(del->custID);
        priority_file.Supprimer(del->delID);
        if (best.IsFeasible) {
            _insrmv.ApplyInsertMove(s, best);
            s.Update(best.move.depot, best.move.dock, best.n);
            assert(s.DeliveryLoad[best.n->delID] > 0);
//            s.ShowSchedule(del);
            Order *cur_order = s.GetOrder(del->orderID);
            if (not s.isOrderSatisfied(cur_order)) {
                Delivery *next_del = s.GetDelivery(cur_order, del->rank + 1);
                if (next_del == nullptr) {
                    s.UnassignCustomer(del->custID);
                } else {
                    DecreaseQueue(s, del, next_del, c);
                }
            } else if (c->nbOrder > 1 and !s.isClientSatisfied(c)) {
                Order *o = s.findIdleOrder(c);
                if (o != nullptr) {
                    Delivery *next_del = s.GetNextIdleDelivery(o);
                    assert(next_del != nullptr);
                    DecreaseQueue(s, del, next_del, c);
                }
            } else if (s.isClientSatisfied(c)) {
//                s.ShowSchedule(c);
//                cout<<"Insert "<<c->constID<<endl;
            }
        }
    }
}

void PriorityQueueInsertion::FillQueue(Sol &s, std::vector<Customer *> &list) {

    priority_file = TempsPriorite(s.GetData(), s.GetData()->GetDeliveryCount());
    std::shuffle(list.begin(), list.end(), Parameters::RANDOM_GEN);

    for (Customer *c: list) {
        Order *o = s.findIdleOrder(c);
        assert(o != nullptr);
        Delivery *del = _data.GetDelivery(o, 0);
        assert(del->id > 0);

        switch (PriorityQueueInsertion::k) {
            case 0:
                priority_file.Ajouter(del->delID, c->early_tw);
                break;
            case 1:
                priority_file.Ajouter(del->delID, c->late_tw);
                break;
            case 2:
                priority_file.Ajouter(del->delID, -c->early_tw);
                break;
            case 3:
                priority_file.Ajouter(del->delID, -c->late_tw);
                break;
            case 4:
                priority_file.Ajouter(del->delID, -c->demand);
                break;
            case 5:
                priority_file.Ajouter(del->delID, c->demand);
                break;
            case 6:
                priority_file.Ajouter(del->delID, c->late_tw - c->early_tw);
                break;
            case 7:
                priority_file.Ajouter(del->delID, -c->late_tw + c->early_tw);
                break;
            default:
                priority_file.Ajouter(del->delID, c->early_tw);
                break;
        }
    }
}

void PriorityQueueInsertion::DecreaseQueue(Sol &s, Delivery *del, Delivery *next_del, Customer *c) {
    std::uniform_real_distribution<double> dis;
    assert(next_del != nullptr);
    switch (PriorityQueueInsertion::k) {
        case 4:
            dis = std::uniform_real_distribution<double>(-1, 1);
            priority_file.Decroitre(next_del->delID,
                                    -c->demand + dis(Parameters::RANDOM_GEN));
            break;
        case 5:
            dis = std::uniform_real_distribution<double>(-1, 1);
            priority_file.Decroitre(next_del->delID,
                                    c->demand + dis(Parameters::RANDOM_GEN));
            break;
        case 6:
            dis = std::uniform_real_distribution<double>(-1, 1);
            priority_file.Decroitre(next_del->delID,
                                    c->late_tw - c->early_tw + dis(Parameters::RANDOM_GEN));
            break;
        case 7:
            dis = std::uniform_real_distribution<double>(-1, 1);
            priority_file.Decroitre(next_del->delID,
                                    -c->late_tw + c->early_tw + dis(Parameters::RANDOM_GEN));
            break;
        default:
            dis = std::uniform_real_distribution<double>(
                    s.EndServiceTime[del->id] - s.GetTimeBtwDel(del),
                    s.EndServiceTime[del->id] + s.GetTimeBtwDel(del));

            priority_file.Decroitre(next_del->delID, dis(Parameters::RANDOM_GEN));
            break;
    }
}
