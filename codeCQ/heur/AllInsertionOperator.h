

#ifndef _ALL_INSERT_CDP_OPERATOR_
#define _ALL_INSERT_CDP_OPERATOR_

#include "../InsertOperator.h"
#include "CustInsertion.h"
#include "CustInsertion2.h"
#include "CustInsertionBacktrack.h"
#include "PriorityQueueInsertion.h"
#include "DriverInsertion.h"

/**
 * k=0, sort by increasing early TW
 */
class AllInsertionOperator : public InsertOperator<Customer, Driver> {
public:

    explicit AllInsertionOperator(int k,std::string const &name):cust_insert_opt(nullptr), _k(k),InsertOperator<Customer, Driver>(name),
                                          cust_insert_bt_opt(nullptr),
                                         prio_insert_opt(nullptr),cust2_insert_opt(nullptr),drv_insert_opt(nullptr),name(name){

    }

    AllInsertionOperator(CustInsertion *insert_opt, int k, std::string const &name)
            :  AllInsertionOperator(k,name) {
        cust_insert_opt=insert_opt;
        cust_insert_opt->SetK(_k);
        cust_insert_opt->name = name;
    }
    AllInsertionOperator(CustInsertion2 *insert2_opt, int k, std::string const &name)
            : AllInsertionOperator(k,name){
        cust2_insert_opt=insert2_opt;
        cust2_insert_opt->SetK(_k);
        cust2_insert_opt->name = name;
    }

    AllInsertionOperator(CustInsertionBacktrack *insert_opt_bt, int k, std::string const &name)
            : AllInsertionOperator(k,name){
        cust_insert_bt_opt=insert_opt_bt;
        cust_insert_bt_opt->SetK(_k);
        cust_insert_bt_opt->name = name;
    }
    AllInsertionOperator(PriorityQueueInsertion *insert_opt_prio, int k, std::string const &name)
            : AllInsertionOperator(k,name)   {
        prio_insert_opt=insert_opt_prio;
        prio_insert_opt->SetK(_k);
        prio_insert_opt->name = name;
    }
    AllInsertionOperator(DriverInsertion *insert_opt_drv, int k, std::string const &name)
            : AllInsertionOperator(k,name)   {
        drv_insert_opt=insert_opt_drv;
        drv_insert_opt->SetK(_k);
        drv_insert_opt->name = name;
    }

    void Insert(Sol &s) override {
        s.heurName=name;
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
        else if (cust2_insert_opt != nullptr) {
            cust2_insert_opt->SetK(_k);
            cust2_insert_opt->Insert(s);
        } else if (drv_insert_opt != nullptr) {
            drv_insert_opt->SetK(_k);
            drv_insert_opt->Insert(s);
        }
        else
        {
            printf("Invalid Operator\n");
            exit(1);
        }
    }
std::string name;
private:
    int _k;
    CustInsertion *cust_insert_opt;
    CustInsertion2 *cust2_insert_opt;
    CustInsertionBacktrack *cust_insert_bt_opt;
    PriorityQueueInsertion *prio_insert_opt;
    DriverInsertion *drv_insert_opt;
};

#endif
