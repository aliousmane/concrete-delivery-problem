#ifndef INCLUDE_RECHERCHELOCALE_H
#define INCLUDE_RECHERCHELOCALE_H

#include "../Solution.h"
#include "CDPSolver.h"
#include <vector>
#include <iostream>
#include<set>

class RechercheLocale
{
public:
    RechercheLocale():customerIdList(0), bestCost(false){}
    explicit RechercheLocale(std::set<int> const& feasibleClients):bestCost(false){
        customerIdList = std::vector<int>(feasibleClients.begin(), feasibleClients.end());
    }
    ~RechercheLocale()=default;
    void Run(Sol &s);
    void RunAllFeasible(Sol &s);
    void RemoveAndReschedule(Sol &s);
    bool SwapLoad(Sol &s, Order *o);
    bool UseSingleDriver(Sol &s, Order *o);
    bool Swap1(Sol &s, Customer *c1,Customer *c2);
    bool Swap2(Sol &s, Customer *c1,Customer *c2);
    bool Relocate1(Sol &s, Customer *c1,Customer *c2);
    bool Relocate2(Sol &s, Customer *c1,Customer *c2);
    bool RelocateDriver(Sol &s);
    bool found{true};
    std::vector<int> customerIdList;
    Cost bestCost;
    std::vector<std::set<int>> LinkedClientSlot;
private:
    long runtime{0};
};

#endif // !INCLUDE_RECHERCHELOCALE_H
