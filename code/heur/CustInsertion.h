

#ifndef CODE_CUSTINSERTION_H
#define CODE_CUSTINSERTION_H

#include "../InsertOperator.h"
//#include "../TempsPriorite.h"
//#include "../TimeSlotPriorite.h"
#include "../Data.h"
#include "../SortRelated.h"
#include "InsRmvBuilder.h"

class CustInsertion : public InsertOperator<Customer, Driver> {

public:
    CustInsertion(Data &prob, InsRmvBuilder &insrmv)
            : _data(prob), _insrmv(insrmv),  customersListId(0),
              customersList(0), _ins_rmv_perators(0), _k(0),removedList(0),removedListId(0),
              name("CustInsertion") {
        _ins_rmv_perators.emplace_back(&insrmv);
        Init();
    }

    ~CustInsertion() override;

    void Insert(Sol &s) override;

    void SetK(int k) { _k = k; }
    void SetOp(InsRmvBuilder *ins) {
        _ins_rmv_perators.emplace_back(ins);
    }
    std::string name;
private:
    void Insert(Sol &s,std::vector<Customer *> &list, std::vector<int> &list_ID);
    void Sort(Sol &s,std::vector<Customer *> &list, std::vector<int> &list_ID, int k);

    void Init()
    {
//        priority_file = TempsPriorite(&_data, _data.GetNodeCount());
        for (int i = 0; i < _data.GetDepotCount(); i++)
        {
            depotList.push_back(_data.GetDepot(i));
        }
        for (int i = 0; i < _data.GetDriverCount(); i++)
            driversList.push_back(_data.GetDriver(i));
    }
//    TempsPriorite priority_file;
    std::vector<InsRmvBuilder *> _ins_rmv_perators;

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


#endif //CODE_CUSTINSERTION_H
