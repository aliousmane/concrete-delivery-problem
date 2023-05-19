#include "GRASP.h"
#include "Driver.h"
#include "Node.h"
#include <cassert>

using namespace std;
template <class NodeT, class DriverT>
void GRASP<NodeT, DriverT>::Optimize(Sol &s,bool first_improvement)
{
	Optimize(s, nullptr,nullptr,nullptr, false);
}
template <class NodeT, class DriverT>
void GRASP<NodeT, DriverT>::Optimize(
	Sol &s, ISolutionList<NodeT, DriverT> *best_sol_list)
{
	cout << "Not implemented\n";
	exit(1);
}

template <class NodeT, class DriverT>
void GRASP<NodeT, DriverT>::Optimize(
	Sol &s, ISolutionList<NodeT, DriverT> *best_sol_list, unordered_map<string, Sol, MyHashFunction> *Mymap,
    RechercheLocale * loc_search,bool first_improvement)
{
	Sol best;
	std::vector<Node *> list_cust;

	std::vector<BestSolutionList<Node, Driver>> tempVectSolutions;

	list_cust.resize(s.GetCustomerCount());
// TODO	for (int i = 0; i < s.GetCustomerCount(); i++)
//	{
//		Node *c = s.GetCustomer(i);
//		list_cust[i] = c;
//		tempVectSolutions.emplace_back(BestSolutionList<Node, Driver>(s.GetData(), 5));
//		Sol::nbSatisfied[c->custID] = 0;
//	}
	//TODO SortNode<Node, Driver>::radixSortGreatDemand(list_cust, s.GetData()->GetMaxDemand());

	Cost bestCout(false);
	int iter_k = 0;
	_chrono.start();
	_chrono.setDuration(300);
	if (verbose)
	{
		printf("GRASP heuristic: %d iterations: %d Heuristics\n", _iterator_count, (int)grasp_insert_operators.size());
	}
	for (int iter = 0; iter < _iterator_count; iter++)
	{
		for (int i = 0; i < (int)grasp_insert_operators.size(); i++)
		{
            Sol cur(s.GetData());
            cur.keyCustomers = s.keyCustomers;
			auto f = grasp_insert_operators[i];
			f.opt->Insert(cur);
            if(loc_search!= nullptr){
                loc_search->Run(cur);
            }
			if (not cur.isFeasible)
			{
				iter_k++;
				continue;
			}
			if (Mymap != nullptr)
			{
				if (Mymap->find(cur.toString()) == Mymap->end())
				{
					(*Mymap)[cur.toString()] = cur;
				}
			}
			Cost curCout = cur.GetCost();

// TODO			for (Node *c : list_cust)
//			{
//				if (cur.isClientSatisfied(c))
//				{
//					Sol::nbSatisfied[c->custID]++;
//					// vectSolutions[c->custID].Add(cur);
//					tempVectSolutions[c->custID].Add(cur);
//				}
//			}
			if (iter_k > 0 && iter_k % 3 == 0)
			{
//TODO				Sol path_cur(s.GetData());
//				path_cur.keyCustomers = s.keyCustomers;
//
//				PathRelinking(path_cur, list_cust, tempVectSolutions, bestCout, best);
//
//				Cost pathCout = path_cur.GetCost();
//				if (pathCout < curCout)
//				{
//					curCout = pathCout;
//					cur = path_cur;
//				}
			}
			cur.iter = iter_k++;

			if (curCout < bestCout)
			{
				bestCout = curCout;
				best = cur;
				if (verbose)
                {
                    printf("Iter(%d-%d) %d %.2lf %2.1lf %2.lf %2.0lf real %2.0lf*\n",
                           iter, i + 1, iter_k, curCout.travelCost, curCout.getTotal(),
                           curCout.lateDeliveryCost, curCout.satisfiedCost,
                           bestCout.satisfiedCost);
                }

                if(first_improvement){
					if (std::includes(best.satisfiedCustomers.begin(), best.satisfiedCustomers.end(), 
					best.keyCustomers.begin(), best.keyCustomers.end()))
					{
						iter = _iterator_count;
						break;
					}
				}
			}
		}

		if (iter % 100 == 0)
		{
			// printf("Iter(%d) %2.0lf\n", iter, bestCout.satisfiedCost);
		}
		if (iter % _chrono_check_iter == 0 && _chrono.hasToEnd())
			break;
	}

	if (best.isFeasible)
	{
		s = best;
		s.Update();
		if (verbose)
		{
			s.ShowCustomer();
			// printf("Best iter %d %2.1lf lateness %2.1lf\n", best.iter, s.GetCost().satisfiedCost, s.GetCost().lateDeliveryCost);
			printf(" iter_k %d %2.1lf lateness %2.1lf\n", iter_k, s.GetCost().satisfiedCost, s.GetCost().lateDeliveryCost);
		}
	}
	s.duration_time = _chrono.getTime();
	// cout << "Time " << s.duration_time << endl;
	_chrono.stop();
}

template class GRASP<Customer, Driver>;