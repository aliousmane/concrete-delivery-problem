

#ifndef CODE_INSRMVMETHODFAST_H
#define CODE_INSRMVMETHODFAST_H
#define WITH_OUTPUT 0

#include "../InsRmvMethod.h"
#include "../Driver.h"
#include "../Node.h"
#include "MoveVrp.h"
#include "ListMoveVrp.h"
#include "../lib/mathfunc.h"
#include <vector>
#include <set>

class InsRmvMethodFast : public InsRmvMethod<Delivery, Driver, MoveVrp> {
public:
    explicit InsRmvMethodFast(Data &data);

    InsRmvMethodFast() = default;

    void InsertCost(Sol &s, Delivery *n, Driver *d, Move<Delivery, Driver, MoveVrp> &m) override;
    void InsertCost(Sol &s, Delivery *n, Driver *d, ListMove<Delivery, Driver, MoveVrp> *temp_moves);

    void RemoveCost(Sol &s, Delivery *n, Move<Delivery, Driver, MoveVrp> &m) override {}

    void FillInsertionList(Sol &s, std::vector<Delivery *> &list) override {}

    void CancelMove(Sol &s, Move<Delivery, Driver, MoveVrp> &m) override {}


    void GetBestInsertion(Sol &s, const std::vector<int> &listId,
                          const std::vector<Driver *> &driversList,
                          Move<Delivery, Driver, MoveVrp> &best) override {}

    void GetBestInsertion(Sol &s, const std::vector<int> &listId,
                          const std::vector<Driver *> &driversList,
                          ListMove<Delivery, Driver, MoveVrp> *list_moves) override;

    void SetServiceParams(Sol &s, Delivery *n, Driver *d, double demand) override;

    void repairSolution(Sol &s) override {}

    void repairSolution(Sol &s, Order *o);

    void repairSolution(Sol &s, Customer *c);


    Move<Delivery, Driver, MoveVrp> GetCost(
            Sol &s, Delivery *n, Driver *d, Cost &solcost, double demand,
            ListMove<Delivery, Driver, MoveVrp> *temp_moves) override;

    void ApplyInsertMove(Sol &s, Move<Delivery, Driver, MoveVrp>
    &m) override;

    bool cancel{false};
    double UNLOADING_DURATION{0};
    double LOAD_DURATION{0};
    int ADJUSTMENT_DURATION{0};
    double expected_del_time{0};
    double real_del_time{0};
    double max_arrival_Time{0};

    void FillStructures(Sol &s, std::vector<Customer *> &customersList,
                        std::vector<Driver *> &driversList);

private:
    Data *_data;

};


#endif //CODE_INSRMVMETHODFAST_H
