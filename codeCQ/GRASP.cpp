#include "GRASP.h"
#include "Driver.h"
#include "Node.h"
#include "heur/CDPSolver.h"

using namespace std;

template<class NodeT, class DriverT>
void GRASP<NodeT, DriverT>::Optimize(Sol &s, bool first_improvement) {
    Optimize(s, nullptr, nullptr, nullptr, first_improvement);
}

template<class NodeT, class DriverT>
void GRASP<NodeT, DriverT>::Optimize(
        Sol &s, ISolutionList<NodeT, DriverT> *best_sol_list) {
    Optimize(s, best_sol_list, nullptr, nullptr, true);
}

template<class NodeT, class DriverT>
void GRASP<NodeT, DriverT>::Optimize(
        Sol &s, ISolutionList<NodeT, DriverT> *best_sol_list, unordered_map<string, Sol, MyHashFunction> *Mymap,
        RechercheLocale *loc_search, bool first_improvement) {
    Sol best;
    std::vector<Customer *> list_cust;
    std::vector<BestSolutionList<Customer, Driver>> tempVectSolutions(s.GetCustomerCount(),
                                                                      BestSolutionList<Customer, Driver>(s.GetData(),
                                                                                                         3));
    unordered_map<string, Sol, MyHashFunction> customerMap;
    unordered_map<string, Sol, MyHashFunction> LocalSearchMap;

    list_cust.resize(s.GetCustomerCount());
    for (int i = 0; i < s.GetCustomerCount(); i++) {
        Customer *c = s.GetCustomer(i);
        list_cust[i] = c;
        CDPSolver::nbSatisfied[c->custID] = 0;
    }
    //TODO SortNode<Node, Driver>::radixSortGreatDemand(list_cust, s.GetData()->GetMaxDemand());
    std::vector<int> SORT_TYPE_VEC{
            Parameters::SORT::ONE,
                            Parameters::SORT::TWO,
//            Parameters::SORT::THREE,
//                            Parameters::SORT::FOUR,
//            Parameters::SORT::FIVE,
//            Parameters::SORT::SHUFFLE
    };
    std::vector<int> DRIVER_USE_VEC{
            Parameters::MINIMIZEDRIVER::SOLUTION,
//        Parameters::MINIMIZEDRIVER::CLIENT
    };
    Cost bestCout(false);
    _chrono.start();
    _chrono.setDuration(Parameters::RUNTIME);
    if (verbose) {
        printf("GRASP heuristic: %d iterations: %d Heuristic(s)\n", _iterator_count,
               (int) grasp_insert_operators.size());
    }
    int iter_k = 0;
    bool stop = false;
    for (int iter = 0; iter < _iterator_count; iter++) {
        if (stop) break;

        for (auto sort: SORT_TYPE_VEC) {
            if (stop) break;

            Parameters::SORT_TYPE = sort;
            for (auto drv_usage: DRIVER_USE_VEC) {
                if (stop) break;

                Parameters::DRIVER_USE = drv_usage;


                int op = std::min(grasp_insert_operators.size() - 1, iter % grasp_insert_operators.size());
//                for (int op = 0; op < (int) grasp_insert_operators.size(); op++)
                {
                    if (stop) break;

                    Sol cur(s.GetData(), s.keyCustomers);
                    auto f = grasp_insert_operators[op];
                    f.opt->Insert(cur);
                    if (not cur.isFeasible) {
                        iter_k++;
                        continue;
                    }
                    if (first_improvement) {
                        if (std::includes(cur.satisfiedCustomers.begin(), cur.satisfiedCustomers.end(),
                                          cur.keyCustomers.begin(), cur.keyCustomers.end())) {
                            iter = _iterator_count;
                            best = cur;
                            bestCout = cur.GetLastCost();
                            stop = true;
                            break;
                        }
                    }
                    if (loc_search != nullptr) {
                        if (LocalSearchMap.count(cur.toString()) == 0) {
                            loc_search->Run(cur);
                            LocalSearchMap[cur.toString()] = cur;
                        }
                    }

                    if (Mymap != nullptr) {
                        if (Mymap->find(cur.toString()) == Mymap->end()) {
                            (*Mymap)[cur.toString()] = cur;
                        }
                    }
                    Cost curCout = cur.GetCost();
//                    printf("Iter %d\n ",iter_k);
//                    cout<<curCout<<endl;
                    for (auto satisfiedId: cur.satisfiedCustomers) {
                        Customer *temp_c = cur.GetCustomer(satisfiedId);
                        if (customerMap.count(cur.toString(temp_c)) == 0) {
                            customerMap[cur.toString(temp_c)] = cur;
                            tempVectSolutions[temp_c->custID].Add(cur);
                            CDPSolver::nbSatisfied[temp_c->custID]++;
                        }
                    }

                    if (iter_k > 0 && iter_k % 4 == 0) {
                        Sol path_cur(s.GetData());
                        path_cur.keyCustomers = s.keyCustomers;
//                        CDPSolver::PathRelinking(path_cur, list_cust, tempVectSolutions, bestCout, best);
                        tempVectSolutions = vector<BestSolutionList<Customer, Driver>>(s.GetCustomerCount(),
                                                                                       BestSolutionList<Customer, Driver>(
                                                                                               s.GetData(), 3));
//                        cout<<"Path relinking"<< path_cur.GetLastCost()<<endl;
                        if (path_cur < cur) {
                            cur = path_cur;
                            curCout = path_cur.GetLastCost();
                            if (curCout < bestCout) {
                                cout << "best Iter " << iter_k << " Path relinking with cost" << curCout << endl;
                            }
                        }

                    }
                    cur.iter = iter_k++;

                    if (curCout < bestCout) {
                        bestCout = curCout;
                        best = cur;
                        if (verbose) {
                            printf("Iter(%d-%s-%s) %d ", iter, f.opt->name.c_str(), best.heurName.c_str(), iter_k);
                            cout << bestCout << endl;
                        }

                        if (first_improvement) {
                            if (std::includes(best.satisfiedCustomers.begin(), best.satisfiedCustomers.end(),
                                              best.keyCustomers.begin(), best.keyCustomers.end())) {
                                iter = _iterator_count;
                                stop = true;
                                break;
                            }
                        }
                        if (_chrono.hasToEnd()) {
                            stop = true;
                            break;
                        }

                    }
                }

                if (iter % 100 == 0) {
                    // printf("Iter(%d) %2.0lf\n", iter, bestCout.satisfiedCost);
                }
                if (iter % _chrono_check_iter == 0 && _chrono.hasToEnd())
                    break;
            }
        }
    }

    if (best.isFeasible) {
        s = best;
        s.Update();
        if (verbose) {
//            s.ShowCustomer();
            // printf("Best iter %d %2.1lf lateness %d\n", best.iter, s.GetCost().satisfiedCost, s.GetCost().lateDeliveryCost);
            printf("iter_k %d %2.1lf lateness %2.0lf\n", iter_k, s.GetCost().satisfiedCost,
                   s.GetCost().lateDeliveryCost);
        }
    }
    s.duration_time = _chrono.getTime();
    // cout << "Time " << s.duration_time << endl;
    _chrono.stop();
}

template
class GRASP<Customer, Driver>;