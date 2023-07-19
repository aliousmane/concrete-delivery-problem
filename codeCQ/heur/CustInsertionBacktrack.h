

#ifndef CODE_CUSTINSERTION_BACKTRACK_H
#define CODE_CUSTINSERTION_BACKTRACK_H

#include "../InsertOperator.h"
#include "ListMoveVrp.h"
#include "../Data.h"
#include "../SortRelated.h"
#include "InsRmvBuilder.h"

class CustInsertionBacktrack : public InsertOperator<Customer, Driver> {

public:
    CustInsertionBacktrack(Data &prob, InsRmvBuilder &insrmv)
            : _data(prob), _insrmv(insrmv),  customersListId(0),
              customersList(0),  _k(0),removedList(0),removedListId(0),
              name("CustInsertionBacktrack"),listMoves(0) {
        customersListId = std::vector<int>();
        removedListId = std::vector<int>();
        customersList = std::vector<Customer *>();
        removedList = std::vector<Customer *>();
        listMoves = std::vector<ListMoveVrp>();
        Init();
    }

    ~CustInsertionBacktrack() override;

    void Insert(Sol &s) override;

    void SetK(int k) { _k = k; }

    std::string name;
    std::vector<ListMoveVrp>  listMoves;
private:
    void Insert(Sol &s,std::vector<Customer *> &list, std::vector<int> &list_ID);
    void Sort(Sol &s,std::vector<Customer *> &list, std::vector<int> &list_ID, int k);
    void Init()
    {
        for (int i = 0; i < _data.GetDepotCount(); i++)
        {
            depotList.push_back(_data.GetDepot(i));
        }
        for (int i = 0; i < _data.GetDriverCount(); i++)
            driversList.push_back(_data.GetDriver(i));
    }
    InsRmvBuilder &_insrmv;
    int _k;
    Data &_data;
    std::vector<Depot *> depotList;
    std::vector<Driver *> driversList;
    std::vector<int> customersListId;
    std::vector<int> removedListId;
    std::vector<Customer*> customersList;
    std::vector<Customer *> removedList;
};


#endif //CODE_CUSTINSERTION_BACKTRACK_H
