
#include "CustInsertion.h"
#include <iostream>

using namespace std;
void CustInsertion::Insert(Sol &s)
{
    s.Update();

    customersListId = std::vector<int>();
    removedListId = std::vector<int>();
    customersList = std::vector<Customer *>();
    removedList = std::vector<Customer *>();

    if (not s.keyCustomers.empty())
    {
        for (int i : s.keyCustomers)
        {
            Customer *c = s.GetCustomer(i);
            if(s.isClientSatisfied(c))
                continue;
            customersList.emplace_back(c);
            customersListId.emplace_back(c->custID);
        }
    }
    else
    {
        for (int i = 0; i < s.GetCustomerCount(); i++)
        {
            Customer *c = s.GetCustomer(i);
            customersList.emplace_back(c);
            customersListId.emplace_back(c->custID);
        }
    }
    Insert(s, customersList, customersListId);
    bool sortie = true;
    while (!removedList.empty() && !sortie)
    {
        double demand = s.updateCost.satisfiedCost;
        // cout << s.updateCost << endl;
        Insert(s, removedList, removedListId);
        // cout << s.updateCost << endl;
        sortie = (s.updateCost.satisfiedCost == demand);
    }
    // s.Update();
}

void CustInsertion::Sort(Sol &s, std::vector<Customer *> &list,
                         std::vector<int> &list_ID, int k)
{
    switch (k)
    {
        case 0:
            SortNode<Customer, Driver>::radixSortEarlyTW(list, this->_data.maxEarlyTW);
            break;
        case 1:
            SortNode<Customer, Driver>::radixSortGreatDemand(list, this->_data.MaxDemand);
            break;
        case 2:
            SortNode<Customer, Driver>::radixSortLateTW(list, this->_data.maxLateTW);
            break;
        case 3:
            std::stable_sort(list_ID.begin(), list_ID.end(), [this](int a, int b)
            { return -this->_data.GetCustomer(a)->early_tw <
                     -this->_data.GetCustomer(b)->early_tw; });
            break;
        case 4:
            std::stable_sort(list_ID.begin(), list_ID.end(), [this](int a, int b)
            { return -this->_data.GetCustomer(a)->late_tw <
                     -this->_data.GetCustomer(b)->late_tw; });
            break;
        case 5:
            std::stable_sort(list_ID.begin(), list_ID.end(), [this](int a, int b)
            { return this->_data.GetCustomer(a)->demand <
                     this->_data.GetCustomer(b)->demand; });
            break;
        case 6:
            std::shuffle(list.begin(), list.end(), Parameters::RANDOM_GEN);
            break;

        case 7:
            SortNode<Customer, Driver>::radixSortKinable(list, this->_data.maxLateTW);
            break;
        default:
            std::stable_sort(list_ID.begin(), list_ID.end(), [this](int a, int b)
            { return this->_data.GetCustomer(a)->early_tw <
                     this->_data.GetCustomer(b)->early_tw; });
            break;
    }
}

void CustInsertion::Insert(Sol &s, std::vector<Customer *> &list,
                           std::vector<int> &list_ID)
{
    Sort(s, list, list_ID, CustInsertion::_k);
    removedList.clear();
    removedListId.clear();
    std::shuffle(driversList.begin(), driversList.end(), Parameters::RANDOM_GEN);
    for (auto c : list)
    {
        Order *cur_order = nullptr;
//        cout<<*c<<endl;
        if (not s.isClientSatisfied(c))
        {
            std::vector<Order*> orders(s.GetOrders(c));
            std::shuffle(orders.begin(), orders.end(), Parameters::RANDOM_GEN);
            for (auto o1 : orders)
            {
                cur_order = o1;
                if (s.isOrderSatisfied(cur_order))
                    continue;
                break;
            }
        }
        else{
            continue;
        }
        // std::shuffle(_ins_rmv_perators.begin(), _ins_rmv_perators.end(), Parameters::RANDOM_GEN);
        assert(cur_order != nullptr);
        for (int j = 0; j < s.GetDeliveryCount(cur_order); )
        {
            Delivery *del = s.GetDelivery(cur_order, j);
//             cout << this->name << " Try to insert del " << del->id << "("<<del->rank<<") for "<<cur_order->orderID<< endl;

            // ! Sort list of drivers here
            Move<Delivery, Driver, MoveVrp> best;
            std::vector<int> listId{del->delID};


            for (auto op_ins_rmv : _ins_rmv_perators)
            {
                op_ins_rmv->GetBestInsertion(s, listId, driversList, best);
                if (best.IsFeasible)
                {
                    break;
                }
            }

            if (!best.IsFeasible)
            {
                if( del->rank>0 && Sol::FailureCause[del->id]==Parameters::FAILURECAUSE::DELAY)
                {

                        Delivery *prec_del = s.GetDelivery(cur_order,del->rank-1);
                        Sol::minDelay[prec_del->id] = Sol::minDelay[del->id];
                        Sol::minDelay[del->id]=0;
                        Sol::FailureCause[del->id]=Parameters::FAILURECAUSE::NONE;
                        Sol::FailureCause[prec_del->id]=Parameters::FAILURECAUSE::NONE;
//                        cout<<" Push "<<prec_del->id << " by "<<Sol::minDelay[prec_del->id]<<endl;
//                        s.ShowSchedule(cur_order);
                        s.UnassignDelivery({prec_del});
                        prec_del->isdelayed = true;
                        j=j-1;
                        continue;
                }
                else{
                    s.UnassignOrder(cur_order);
                    removedList.emplace_back(c);
                    removedListId.emplace_back(c->custID);
                    break;
                }

            }

            if (best.IsFeasible)
            {
                _insrmv.ApplyInsertMove(s, best);
//                cout<<*del<<endl;
                s.Update(best.move.depot, best.move.dock, best.n);
//                s.ShowSchedule(cur_order);
                j++;
            }
            if (s.isOrderSatisfied(cur_order))
            {
                break;
            }
        }
        if (s.isClientSatisfied(c))
        {
//            s.ShowSchedule();
//            cout<<"Insert "<<endl;
            continue;
        }
    }
//    s.ShowCustomer();
     s.Update();
//     cout<<s.GetCost()<<std::endl;
//     exit(1);
}

CustInsertion::~CustInsertion() {
}
