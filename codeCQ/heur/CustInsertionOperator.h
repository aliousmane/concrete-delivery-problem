

#ifndef _INSERT_CDP_OPERATOR_
#define _INSERT_CDP_OPERATOR_

#include "../InsertOperator.h"
#include "CustInsertion.h"
#include "CustInsertionBacktrack.h"

/**
 * k=0, sort by increasing early TW
 * k=1, sort by decreasing demand
 * k=2, sort by increasing late TW
 * k=3, sort by decreasing early TW
 * k=4, sort by decreasing late TW
 * k=5, sort by decreasing demand
 * k=5, shuffle customer
 */
class CustInsertionOperator : public InsertOperator<Customer, Driver> {
public:
    CustInsertionOperator(CustInsertion *insert_opt, int k)
            : _insert_opt(insert_opt), _k(k),
              InsertOperator<Customer, Driver>("CustInsertionOperator"), _insert_opt_bt(nullptr) {
        _insert_opt->SetK(_k);
    }

    CustInsertionOperator(CustInsertionBacktrack *insert_opt_bt, int k)
            : _insert_opt_bt(insert_opt_bt), _k(k),
              InsertOperator<Customer, Driver>("CustInsertionOperatorBacktrack"), _insert_opt(nullptr) {
        _insert_opt_bt->SetK(_k);
    }

    CustInsertionOperator(CustInsertion *insert_opt, int k, std::string const &name)
            : _insert_opt(insert_opt), _k(k), InsertOperator<Customer, Driver>(name) {
        _insert_opt->SetK(_k);
        _insert_opt->name = name;
    }

    CustInsertionOperator(CustInsertionBacktrack *insert_opt_bt, int k, std::string const &name)
            : _insert_opt_bt(insert_opt_bt), _k(k), _insert_opt(nullptr), InsertOperator<Customer, Driver>(name) {
        _insert_opt_bt->SetK(_k);
        _insert_opt_bt->name = name;
    }

    void Insert(Sol &s) override {
        if (_insert_opt != nullptr) {
            _insert_opt->SetK(_k);
            _insert_opt->Insert(s);
        } else if (_insert_opt_bt != nullptr) {
            _insert_opt_bt->SetK(_k);
            _insert_opt_bt->Insert(s);
        }
        else
        {
            printf("Invalid Operator\n");
            exit(1);
        }
    }

private:
    int _k;
    CustInsertion *_insert_opt;
    CustInsertionBacktrack *_insert_opt_bt;
};

#endif
