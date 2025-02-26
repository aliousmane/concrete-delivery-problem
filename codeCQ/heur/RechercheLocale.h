#ifndef INCLUDE_RECHERCHELOCALE_H
#define INCLUDE_RECHERCHELOCALE_H

#include "../Solution.h"
#include "CDPSolver.h"
#include <vector>
#include <iostream>
#include <iterator>
#include<set>

class RechercheLocale {
public:
    RechercheLocale() : customerIdList(0), bestCost(false) {}

    explicit RechercheLocale(std::set<int> const &feasibleClients) : bestCost(false) {
        customerIdList = std::vector<int>(feasibleClients.begin(), feasibleClients.end());
    }

    ~RechercheLocale() = default;

    void Run(Sol &s);

    void RunAllFeasible(Sol &s);

    bool RemoveAndReschedule(Sol &s);

    bool SwapLoad(Sol &s, Order *o);
    bool RelocateStartLoad(Sol &s);
	bool SwapDriver(Sol &s);
    bool UseSingleDriver(Sol &s, Order *o);

    bool Swap1(Sol &s, Customer *c1, Customer *c2);

    bool Swap2(Sol &s, Customer *c1, Customer *c2);

    bool Relocate1(Sol &s, Customer *c1, Customer *c2);

    bool Relocate2(Sol &s, Customer *c1, Customer *c2);

    bool RelocateDriver(Sol &s);

    bool ShiftLoading(Sol &s);

    bool UnscheduledFirst(Sol &s);

    bool LoadBackward(Sol &s);

    bool found{true};
    std::vector<int> customerIdList;
    Cost bestCost;
    std::vector<std::set<int>> LinkedClientSlot;
private:
    long runtime{0};
    std::set<int> keyCustomer;
    std::set<int> availableDriver;

};

#endif // !INCLUDE_RECHERCHELOCALE_H
