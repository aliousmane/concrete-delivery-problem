/* Copyright Jean-Francois Cote 2017
 * The code may be used for academic, non-commercial purposes only.
 * Please contact me at jean-francois.cote@fsa.ulaval.ca for questions
 * If you have improvements, please contact me!
*/

#ifndef MoveVRP_H_
#define MoveVRP_H_

#include "../Node.h"
#include "../Driver.h"


class MoveVrp
{
public:
	Delivery* del;
    Dock * dock;
	Node* prev;
	Node* prevDepot;
    Depot * depot;
	Driver* to;
	Driver* from;
	double DeltaCost;
	double DeltaDistance;
	double arrival_time;
	bool IsFeasible;
};

#endif
