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
    bool SwapBuild(Sol &s, Customer *c1, Customer *c2);
    bool SwapReStart(Sol &s, Customer *c1, Customer *c2);
    bool RelocateBuild(Sol &s, Customer *c1, Customer *c2);
    bool RelocateReStart(Sol &s, Customer *c1, Customer *c2);
    bool found{true};
    std::vector<int> customerIdList;
    Cost bestCost;
    std::vector<std::set<int>> LinkedClientSlot;
};

#endif // !INCLUDE_RECHERCHELOCALE_H
