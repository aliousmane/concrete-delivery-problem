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

#ifndef REMOVE_OPERATOR_H
#define REMOVE_OPERATOR_H


class Sol;
template <class NodeT, class DriverT>
class RemoveOperator
{
public:
	RemoveOperator() {}
	virtual ~RemoveOperator() {}
	virtual void Remove(Sol& s, int count) = 0;
};

#endif
