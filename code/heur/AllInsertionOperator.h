

#ifndef _ALL_INSERT_CDP_OPERATOR_
#define _ALL_INSERT_CDP_OPERATOR_

#include "../InsertOperator.h"
#include "CustInsertion.h"
#include "CustInsertionBacktrack.h"
#include "PriorityQueueInsertion.h"

/**
 * k=0, sort by increasing early TW
 */
class AllInsertionOperator : public InsertOperator<Customer, Driver> {
public:
    AllInsertionOperator(CustInsertion *insert_opt, int k)
            : cust_insert_opt(insert_opt), _k(k),
              InsertOperator<Customer, Driver>("AllInsertionOperator"), cust_insert_bt_opt(nullptr),prio_insert_opt(
                    nullptr) {
        cust_insert_opt->SetK(_k);
    }

    AllInsertionOperator(CustInsertionBacktrack *insert_opt_bt, int k)
            : cust_insert_bt_opt(insert_opt_bt), _k(k),
              InsertOperator<Customer, Driver>("AllInsertionOperatorBacktrack"), cust_insert_opt(nullptr),prio_insert_opt(
                    nullptr) {
        cust_insert_bt_opt->SetK(_k);
    }
    AllInsertionOperator(PriorityQueueInsertion *insert_opt_prio, int k)
            : prio_insert_opt(insert_opt_prio), _k(k),
              InsertOperator<Customer, Driver>("AllInsertionOperatorPrioQueue"), cust_insert_opt(nullptr),cust_insert_bt_opt(
                    nullptr) {
        prio_insert_opt->SetK(_k);
    }

    AllInsertionOperator(CustInsertion *insert_opt, int k, std::string const &name)
            : cust_insert_opt(insert_opt), _k(k), InsertOperator<Customer, Driver>(name),prio_insert_opt(nullptr) ,cust_insert_bt_opt(nullptr){
        cust_insert_opt->SetK(_k);
        cust_insert_opt->name = name;
    }

    AllInsertionOperator(CustInsertionBacktrack *insert_opt_bt, int k, std::string const &name)
            : cust_insert_bt_opt(insert_opt_bt), _k(k), cust_insert_opt(nullptr), InsertOperator<Customer, Driver>(name),prio_insert_opt(nullptr)  {
        cust_insert_bt_opt->SetK(_k);
        cust_insert_bt_opt->name = name;
    }
    AllInsertionOperator(PriorityQueueInsertion *insert_opt_prio, int k, std::string const &name)
            : prio_insert_opt(insert_opt_prio), _k(k), cust_insert_opt(nullptr), InsertOperator<Customer, Driver>(name),
                    cust_insert_bt_opt(nullptr)  {
        prio_insert_opt->SetK(_k);
        prio_insert_opt->name = name;
    }

    void Insert(Sol &s) override {
        if (cust_insert_opt != nullptr) {
            cust_insert_opt->SetK(_k);
            cust_insert_opt->Insert(s);
        } else if (cust_insert_bt_opt != nullptr) {
            cust_insert_bt_opt->SetK(_k);
            cust_insert_bt_opt->Insert(s);
        }
        else if (prio_insert_opt != nullptr) {
            prio_insert_opt->SetK(_k);
            prio_insert_opt->Insert(s);
        }
        else
        {
            printf("Invalid Operator\n");
            exit(1);
        }
    }

private:
    int _k;
    CustInsertion *cust_insert_opt;
    CustInsertionBacktrack *cust_insert_bt_opt;
    PriorityQueueInsertion *prio_insert_opt;
};

#endif
