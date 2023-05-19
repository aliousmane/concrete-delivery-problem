

#ifndef CODE_DRIVERINSERTION_H
#define CODE_DRIVERINSERTION_H

#include "../InsertOperator.h"
#include "ListMoveVrp.h"
#include "../Data.h"
#include "../SortRelated.h"
#include "InsRmvBuilder.h"

class DriverInsertion : public InsertOperator<Customer, Driver> {

public:
    DriverInsertion(Data &prob, InsRmvBuilder &insrmv)
            : _data(prob), _insrmv(insrmv),
              customersList(0), _k(0),removedList(0),
              name("DriverInsertion"),listMoves(0),driverListId(0) {
        Init();
    }

    ~DriverInsertion() override;

    void Insert(Sol &s) override;

    void SetK(int k) { _k = k; }

    std::string name;
    void Insert(Sol &s, std::vector<int> const &list);
private:
    void Insert(Sol &s, std::vector<int> &list_ID);
    static void Sort(Sol &s,std::vector<Driver *> &list, std::vector<int> &list_ID, int k);

    void Init()
    {
        for (int i = 0; i < _data.GetDepotCount(); i++)
        {
            depotList.push_back(_data.GetDepot(i));
        }
        for (int i = 0; i < _data.GetDriverCount(); i++){
            driversList.push_back(_data.GetDriver(i));
            driverListId.push_back(_data.GetDriver(i)->id);
        }
    }
    std::vector<ListMoveVrp>  listMoves;
    InsRmvBuilder &_insrmv;
    int _k;
    Data &_data;

    std::vector<Depot *> depotList;
    std::vector<Driver *> driversList;
    std::set<int> customersListId;
    std::vector<int> driverListId;
    std::set<int> removedListId;
    std::vector<Customer*> customersList;
    std::vector<Customer *> removedList;

};


#endif //CODE_DRIVERINSERTION_H
