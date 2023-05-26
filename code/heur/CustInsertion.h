

#ifndef CODE_CUSTINSERTION_H
#define CODE_CUSTINSERTION_H

#include "../InsertOperator.h"
#include "ListMoveVrp.h"
#include "../Data.h"
#include "../SortRelated.h"
#include "InsRmvBuilder.h"

class CustInsertion : public InsertOperator<Customer, Driver> {

public:
    CustInsertion(Data &prob, InsRmvBuilder &insrmv)
            : _data(prob), _insrmv(insrmv),
              customersList(0),  _k(0),removedList(0),
              name("CustInsertion"),listMoves(0) {
        customersList = std::vector<Customer *>();
        removedList = std::vector<Customer *>();
        listMoves = std::vector<ListMoveVrp>();
        Init();
    }

    ~CustInsertion() override;

    void Insert(Sol &s) override;

    void SetK(int k) { _k = k; }

    std::string name;
    std::vector<ListMoveVrp>  listMoves;
    void Insert(Sol &s, std::vector<int> const &list);
    void InsertWithBactrack(Sol &s,std::vector<Customer *> &list);
private:
    static void Sort(Sol &s,std::vector<Customer *> &list, int k);
    static void Sort(Sol &s,std::vector<int> &list_ID, int k);
    void Insert2(Sol &s, std::vector<Customer *> &list,std::vector<int> &list_ID);
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
    std::vector<Customer*> customersList;
    std::vector<Customer *> removedList;
};


#endif //CODE_CUSTINSERTION_H
