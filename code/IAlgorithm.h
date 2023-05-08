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

#ifndef IALGORITHM
#define IALGORITHM

#include "ISolutionList.h"
#include <vector>
#include <map>

class Sol;
template <class NodeT, class DriverT>
class IAlgorithm {
public:
  IAlgorithm() {}
  virtual ~IAlgorithm() {}

  virtual void Optimize(Sol& sol) = 0;
  virtual void Optimize(Sol& sol,bool first_improvement) {};

  virtual void Optimize(Sol& sol, ISolutionList<NodeT, DriverT>* list) {};

  virtual void SetMaxTime(int maxtime) {}
};
#endif
