
#ifndef CDP_SOLVER_H
#define CDP_SOLVER_H

#include "../Data.h"
#include "../Solution.h"
#include "../Prompt.h"
#include "../InsertOperator.h"
#include "../BestSolutionList.h"
#include <vector>


class CDPSolver {
public:
    explicit CDPSolver(Data *data):_data(data){
    }
    void run();
    static void SolveInstance(Sol &s,Data &dat,int iter);
    static void findCorrelation(Data &dat, std::vector<TimeSlot> const & listInt, std::vector<std::set<int>> & linkedClientSlot,
    std::vector<std::set<int>> & linkedClientDemand, std::vector<std::set<int>>& linkedClients);
    static void findDisjointSet(Data &dat, std::vector<std::set<int>> const & linkedClients);
    static std::vector<std::set<int>> disjointClients;

//    static std::vector<grasp_insert_operator<Node,Driver>> listOperators;
    static void fillOperatorList(Data &dat);
    static void PathRelinking(Sol &new_cur, std::vector<Customer *> &list_cust,
                              std::vector<BestSolutionList<Customer, Driver>>  & vectSolutions,
                              Cost &bestCout, Sol &best);
    static bool InsertDel(Sol &sol_cur, Sol *cur, Delivery *del);
    static void repairSolution(Sol &new_cur);
    static void repairSchedule(Sol &sol);
    static std::vector<InsertOperator<Node,Driver>*> listOperators;

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
