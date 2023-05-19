#include "PriorityQueueInsertion.h"
#include <iostream>

using namespace std;
/**
 * @brief Insert following specified criterion
 *
 * @param s
 */
void PriorityQueueInsertion::Insert(Sol &s) {
    _data = *s.GetData();
    s.Update();

    if (not s.availableDrivers.empty())
    {
        driversList.clear();
        for (int i : s.availableDrivers)
        {
            driversList.push_back(s.GetDriver(i));
        }
    }
    else{
        driversList.clear();
        for (int i = 0; i < _data.GetDriverCount(); i++)
            driversList.push_back(_data.GetDriver(i));
    }
    customersList.clear();
    removedList.clear();
    if (!s.keyCustomers.empty()) {
        for (int i: s.keyCustomers) {
            Customer *c = s.GetCustomer(i);
            if (s.isClientSatisfied(c))
                continue;
            customersList.emplace_back(c);
        }
    } else {
        for (int i = 0; i < s.GetCustomerCount(); i++) {
            Customer *c = s.GetCustomer(i);
            if (s.isClientSatisfied(c))
                continue;
            customersList.emplace_back(c);
        }
    }

    InsertByTW(s);
}

void PriorityQueueInsertion::InsertByTW(Sol &s) {

    Insert(s, customersList);
    bool sortie = false;
    while (!removedList.empty() && !sortie) {
        double demand = s.updateCost.satisfiedCost;
        // cout << s.updateCost << endl;
        customersList = removedList;
        Insert(s, customersList);
        // cout << s.updateCost << endl;
        sortie = (s.updateCost.satisfiedCost == demand);
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
//         for(auto id:listId){
//           std::cout<<*s.GetDelivery(id)<<endl;
//         }
//         std::cout<<endl;

        std::shuffle(listId.begin(), listId.end(), Parameters::RANDOM_GEN);
        // std::shuffle(_ins_rmv_perators.begin(), _ins_rmv_perators.end(), Parameters::RANDOM_GEN);

        _insrmv.GetBestInsertion(s, listId, driversList, best);

        if (!best.IsFeasible) {
            for (int nodeID: listId) {
                priority_file.Supprimer(nodeID);
                Delivery *del = s.GetDelivery(nodeID);
                Order *o = s.GetOrder(del->orderID);

                if( del->rank>0 && Sol::FailureCount[del->id] <3 && Sol::FailureCause[del->id]==Parameters::FAILURECAUSE::DELAY)
                {
                    Delivery *prec_del = s.GetDelivery(o,del->rank-1);
                    Sol::minDelay[prec_del->id] = Sol::pushVisit[del->id];
                    Sol::pushVisit[del->id]=0;
                    Sol::FailureCause[del->id]=Parameters::FAILURECAUSE::NONE;
                    Sol::FailureCause[prec_del->id]=Parameters::FAILURECAUSE::NONE;
//                        cout<<" Push "<<prec_del->id << " by "<<Sol::minDelay[prec_del->id]<<endl;
//                        s.ShowSchedule(cur_order);
                    prec_del->isdelayed = true;
                }
                // s.ShowCustomerSchedule(s.GetCustomer(cur->custID));
                s.UnassignOrder(o);
                removedList.emplace_back(s.GetCustomer(del->custID));
//                 cout<<"unassigned "<<o->custID<<endl;
            }
            continue;
        }
        Delivery *del = s.GetDelivery(best.n->delID);
        Customer *c = s.GetCustomer(del->custID);
        priority_file.Supprimer(del->delID);
        if (best.IsFeasible) {
            _insrmv.ApplyInsertMove(s, best);
            s.Update(best.move.depot, best.move.dock, best.n);
//             s.ShowSchedule(del);
            Order *cur_order = s.GetOrder(del->orderID);
            if (not s.isOrderSatisfied(cur_order)) {
                Delivery *next_del = s.GetDelivery(cur_order, del->rank + 1);
                DecreaseQueue(s, del, next_del, c);
            } else if (c->nbOrder > 1 and !s.isClientSatisfied(c)) {
                Order *o = s.findIdleOrder(c);
                assert(o != nullptr);
                Delivery *next_del = s.GetNextIdleDelivery(o);
                assert(next_del != nullptr);
                DecreaseQueue(s, del, next_del, c);
            }
            else if(s.isClientSatisfied(c)){
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
    std::uniform_int_distribution<int> dis;
    assert(next_del != nullptr);
    switch (PriorityQueueInsertion::k) {
        case 4:
            dis = std::uniform_int_distribution<int>(-1, 1);
            priority_file.Decroitre(next_del->delID,
                                    -c->demand + dis(Parameters::RANDOM_GEN));
            break;
        case 5:
            dis = std::uniform_int_distribution<int>(0, 0);
            priority_file.Decroitre(next_del->delID,
                                    c->demand + dis(Parameters::RANDOM_GEN));
            break;
        case 6:
            dis = std::uniform_int_distribution<int>(-1, 1);
            priority_file.Decroitre(next_del->delID,
                                    c->late_tw - c->early_tw + dis(Parameters::RANDOM_GEN));
            break;
        case 7:
            dis = std::uniform_int_distribution<int>(-1, 1);
            priority_file.Decroitre(next_del->delID,
                                    -c->late_tw + c->early_tw + dis(Parameters::RANDOM_GEN));
            break;
        default:
            dis = std::uniform_int_distribution<int>(
                    s.EndServiceTime[del->id] - Parameters::TIME_BTW_DELIVERY,
                    s.EndServiceTime[del->id] + Parameters::TIME_BTW_DELIVERY);

            priority_file.Decroitre(next_del->delID, dis(Parameters::RANDOM_GEN));
            break;
    }
}
