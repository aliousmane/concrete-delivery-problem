

#ifndef _INSERT_DRIVER_OPERATOR_
#define _INSERT_DRIVER_OPERATOR_

#include "../InsertOperator.h"
#include "DriverInsertion.h"

/**
 * k=0, sort by increasing early TW
 * k=1, sort by decreasing demand
 * k=2, sort by increasing late TW
 */
class DriverInsertionOperator: public InsertOperator<Customer, Driver>
{
	public:	
    DriverInsertionOperator(DriverInsertion * insert_opt, int k)
            : _insert_opt(insert_opt),_k(k),
            InsertOperator<Customer, Driver>("DriverInsertionOperator"){
        _insert_opt->SetK(_k);
            }
    DriverInsertionOperator(DriverInsertion * insert_opt, int k,std::string const & name)
            : _insert_opt(insert_opt),_k(k),InsertOperator<Customer, Driver>(name){
        _insert_opt->SetK(_k);
        _insert_opt->name=name;
    }
		void Insert(Sol & s) override
		{
			_insert_opt->SetK(_k);
			_insert_opt->Insert(s);
		}
	
	private:
		int _k;
		DriverInsertion *_insert_opt;
};

#endif
