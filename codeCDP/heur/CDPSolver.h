
#ifndef CDP_SOLVER_H
#define CDP_SOLVER_H

#include "../Data.h"
#include "../Solution.h"
#include "../Prompt.h"
#include "../InsertOperator.h"
#include "../BestSolutionList.h"
#include <vector>
#include <unordered_map>
#include "PriorityQueueInsertion.h"
#include "PriorityQueueOperator.h"
#include "DriverInsertionOperator.h"
#include <functional>

class CDPSolver {
public:
    explicit CDPSolver(Data *data):_data(data){
    }
    void run();
    static double ComputeCost(Sol &s, const std::set<int>& setClients);
    static void SolveInstance(Sol &s,Data &dat,int iter);
    static void SolveCDP(Sol &s,Data &dat,int iter,bool restart);
    static std::set<int> EliminateCustomer(Data &dat,const int iter);
    static void BuildOnSolution(Sol &s,Data &dat,int iter);
    static void findCorrelation(Data &dat, std::vector<TimeSlot> const & listInt, std::vector<std::set<int>> & linkedClientSlot,
    std::vector<std::set<int>> & linkedClientDemand, std::vector<std::set<int>>& linkedClients,std::vector<std::set<int>>& linkedClientInf,std::vector<std::set<int>>& linkedClientSup);
    static void findDisjointSet(Data &dat, std::vector<std::set<int>> const & linkedClients);
    static std::vector<std::set<int>> disjointClients;
    static std::vector<int> nbSatisfied;


    static void LearnParameters(Sol &s, std::set<int> const &customer);
    static void find_all_routes(Sol &s, Customer *c,std::unordered_map<std::string, Sol, MyHashFunction> * umap);

    static void PathRelinking(Sol &new_cur, std::vector<Customer *> &list_cust,
                              std::vector<BestSolutionList<Customer, Driver>>  & vectSolutions,
                              Cost &bestCout, Sol &best);
    static bool InsertDel(Sol &sol_cur, Sol *cur, Delivery *del);
    static void repairSolution(Sol &new_cur);
    static void repairSchedule(Sol &sol);
    static std::vector<InsertOperator<Node,Driver>*> listOperators;
    static void RecursiveSols(Sol &s, Delivery *del, std::unordered_map<std::string, Sol, MyHashFunction> * umap);
    static void deleteOperator(){

        while(listOperators.size()>0){
            delete listOperators[0]; // Explicitly delete the pointer
            listOperators.erase(listOperators.begin());
        }
    }

private:
    Data *_data;

};


#endif //CDP_SOLVER_H
