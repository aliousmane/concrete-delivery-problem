

#ifndef _INSERT_CDP_OPERATOR_
#define _INSERT_CDP_OPERATOR_

#include "../InsertOperator.h"
#include "CustInsertion.h"

/**
 * k=0, sort by increasing early TW
 * k=1, sort by decreasing demand
 * k=2, sort by increasing late TW
 * k=3, sort by decreasing early TW
 * k=4, sort by decreasing late TW
 * k=5, sort by decreasing demand
 * k=5, shuffle customer
 */
class CustInsertionOperator: public InsertOperator<Node, Driver>
{
	public:	
    CustInsertionOperator(CustInsertion * insert_opt, int k)
            : _insert_opt(insert_opt),_k(k),
            InsertOperator<Node, Driver>("CustInsertionOperator"){
        _insert_opt->SetK(_k);
            }
    CustInsertionOperator(CustInsertion * insert_opt, int k,std::string name)
            : _insert_opt(insert_opt),_k(k),InsertOperator<Node, Driver>(name){
        _insert_opt->SetK(_k);
        _insert_opt->name=name;
    }
		void Insert(Sol & s)
		{
			_insert_opt->SetK(_k);
			_insert_opt->Insert(s);
		}
	
	private:
		int _k;
		CustInsertion *_insert_opt;
};

#endif
