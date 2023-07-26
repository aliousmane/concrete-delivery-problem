/*
 * Copyright Jean-Francois Cote 2012
 * 
 * The code may be used for academic, non-commercial purposes only.
 * 
 * Please contact me at cotejean@iro.umontreal.ca for questions
 *
 * If you have improvements, please contact me!
 *
*/

#ifndef INSERT_OPERATOR_VRP
#define INSERT_OPERATOR_VRP

#include "Solution.h"
#include <string>

template <class NodeT, class DriverT>
class InsertOperator
{
public:
	InsertOperator():name("Insertion") {}
	InsertOperator(std::string _name):name(_name) {}
	virtual ~InsertOperator() {}

	//Try to insert all unassigned customers
	virtual void Insert(Sol &s) = 0;
    std::string name;
};


#endif
