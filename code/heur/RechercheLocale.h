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
    RechercheLocale(std::set<int> const& feasibleClients):bestCost(false){
        customerIdList = std::vector<int>(feasibleClients.begin(), feasibleClients.end());
    }
    ~RechercheLocale(){}
    void Run(Sol &s, std::vector<std::set<int>> const &LinkedClients);
    void Run(Sol &s);
    void Run(Sol &s, std::vector<std::set<int>> &LinkedClientSlot,std::vector<std::set<int>> &LinkedClientDemand);
    bool Swap(Sol &s, Customer *c1,Customer *c2);
    bool Relocate(Sol &s, Customer *c1,std::set<int> &  _set);
    bool Relocate(Sol &s, Customer *c1,Customer *c2);
    bool found{true};
    std::vector<int> customerIdList;
    Cost bestCost;
    std::vector<std::set<int>> LinkedClientSlot;
};

#endif // !INCLUDE_RECHERCHELOCALE_H
