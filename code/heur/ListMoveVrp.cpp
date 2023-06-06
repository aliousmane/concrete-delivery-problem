#include <algorithm>
#include <iostream>
#include "ListMoveVrp.h"

using namespace std;

bool ListMoveVrpSorter::operator()(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2) {

    if (Parameters::SORT_TYPE == Parameters::SORT::ONE) {
        return Sort1(m1, m2);
    } else if (Parameters::SORT_TYPE == Parameters::SORT::TWO) {
        return Sort2(m1, m2);
    }
    else if (Parameters::SORT_TYPE == Parameters::SORT::THREE) {
        return Sort3(m1, m2);
    }
    else if (Parameters::SORT_TYPE == Parameters::SORT::FOUR) {
        return Sort4(m1, m2);
    }else if (Parameters::SORT_TYPE == Parameters::SORT::FIVE) {
        return Sort5(m1, m2);
    }
    else {
        return Sort2(m1, m2);
    }
}
bool ListMoveVrpSorter::Sort5(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2){
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
    assert(m1.n!= nullptr);
    assert(m1.to!= nullptr);
    assert(m2.n!= nullptr);
    assert(m2.to!= nullptr);
    if (m1.DeltaCost.lateDeliveryCost < m2.DeltaCost.lateDeliveryCost) {
        return true;
    } else if (m1.DeltaCost.lateDeliveryCost == m2.DeltaCost.lateDeliveryCost) {
        if (m1.waste == 0)
            return true;
        else if (m2.waste == 0)
            return false;
        if (m1.arrival_del < m2.arrival_del) {
            return true;
        } else {
            if (m1.waste == m2.waste) {
                return (m1.to->capacity > m2.to->capacity);
            } else
                return (std::abs(m1.waste) < std::abs(m2.waste));
        }
    }
    return false;
}

bool ListMoveVrpSorter::Sort1(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2) {
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
        return true;
    if (m1.n == m2.n) {
        if (m1.allDriver < m2.allDriver)
            return true;
        else if (m1.allDriver == m2.allDriver) {

            if (m1.waste == 0) return true;
            else if (m2.waste == 0) return false;

            if (m1.arrival_del < m2.arrival_del) {
                return true;
            } else if (m1.arrival_del == m2.arrival_del) {

                if(m1.DeltaCost.travelCost < m2.DeltaCost.travelCost)
                    return  true;

                if (m1.waste * m2.waste == 0) {
                    if (m1.waste == 0)
                        return true;
                    return false;
                } else if (m1.waste * m2.waste < 0) {
                    if (m1.waste > 0) return true;
                } else {
                    if (std::abs(m1.waste) < std::abs(m2.waste)) return true;
                    else if (m1.waste == m2.waste) {
                        return (m1.to->capacity > m2.to->capacity);
                    }
                    return false;
                }
            }

        }
//          if(m1.waste < m2.waste)
//              return true;
        return false;
    }
    return (m1 < m2);
}

bool ListMoveVrpSorter::Sort2(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2) {

    if(m1.FailureCause!=-1) return false;
    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
        return true;
    if (m1.n == m2.n) {
        if (m1.waste == 0) return true;
        else if (m2.waste == 0) return false;

        if (m1.arrival_del < m2.arrival_del) {
            return true;
        } else if (m1.arrival_del == m2.arrival_del) {

            if (m1.allDriver < m2.allDriver)
                return true;

//            if(m1.DeltaCost.travelCost < m2.DeltaCost.travelCost) return true;

            if (m1.waste * m2.waste == 0) {
                if (m1.waste == 0)
                    return true;
                return false;
            } else if (m1.waste * m2.waste < 0) {
                if (m1.waste > 0) return true;
            } else {
                if (std::abs(m1.waste) < std::abs(m2.waste)) return true;
                else if (m1.waste == m2.waste) {
                    return (m1.to->capacity > m2.to->capacity);
                }
                return false;
            }
        } else {
            if (m1.allDriver < m2.allDriver)
                return true;
        }


//          if(m1.waste < m2.waste)
//              return true;
        return false;
    }
    return (m1 < m2);
}

bool ListMoveVrpSorter::Sort3(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2) {
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
    if(m2.FailureCause!=Parameters::FAILURECAUSE::NONE) return true;
    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
        return true;
    if (m1.n == m2.n) {
        if (m1.waste == 0) return true;
        else if (m2.waste == 0) return false;
        if (m1.DeltaCost.travelCost < m2.DeltaCost.travelCost) {
            return true;
        }
        if (m1.arrival_del < m2.arrival_del) {
            return true;
        } else if (m1.arrival_del == m2.arrival_del) {

            if (m1.allDriver < m2.allDriver)
                return true;

//            if(m1.DeltaCost.travelCost < m2.DeltaCost.travelCost) return true;

            if (m1.waste * m2.waste == 0) {
                if (m1.waste == 0)
                    return true;
                return false;
            } else if (m1.waste * m2.waste < 0) {
                if (m1.waste > 0) return true;
            } else {
                if (std::abs(m1.waste) < std::abs(m2.waste)) return true;
                else if (m1.waste == m2.waste) {
                    return (m1.to->capacity > m2.to->capacity);
                }
                return false;
            }
        } else {
            if (m1.allDriver < m2.allDriver)
                return true;
        }


//          if(m1.waste < m2.waste)
//              return true;
        return false;
    }
    return (m1 < m2);
}

bool ListMoveVrpSorter::Sort4(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2){
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
//    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
//        return true;

    if(m1.DeltaCost.undeliveredCost<m2.DeltaCost.undeliveredCost) return true;
    if(m1.DeltaCost.undeliveredCost>m2.DeltaCost.undeliveredCost) return false;
    if(m1.allDriver<m2.allDriver) return true;
    else{
        if(m1.clientDriver < m2.clientDriver)
            return true;
//        else if(m1.clientDriver==m2.clientDriver){
//            return(m1.waste<m2.waste);
//        }
    }
//    if (m1.waste == 0) return true;
//    else if (m2.waste == 0) return false;
    return (m1 < m2);
}


bool ListMoveVrp::Sort1(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2) {
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
        return true;
    if (m1.n == m2.n) {
        if (m1.allDriver < m2.allDriver)
            return true;
        else if (m1.allDriver == m2.allDriver) {

            if (m1.waste == 0) return true;
            else if (m2.waste == 0) return false;

            if (m1.arrival_del < m2.arrival_del) {
                return true;
            } else if (m1.arrival_del == m2.arrival_del) {

                if(m1.DeltaCost.travelCost < m2.DeltaCost.travelCost)
                    return  true;

                if (m1.waste * m2.waste == 0) {
                    if (m1.waste == 0)
                        return true;
                    return false;
                } else if (m1.waste * m2.waste < 0) {
                    if (m1.waste > 0) return true;
                } else {
                    if (std::abs(m1.waste) < std::abs(m2.waste)) return true;
                    else if (m1.waste == m2.waste) {
                        return (m1.to->capacity > m2.to->capacity);
                    }
                    return false;
                }
            }

        }
//          if(m1.waste < m2.waste)
//              return true;
        return false;
    }
    return (m1 < m2);
}

bool ListMoveVrp::Sort2(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2) {

    if(m1.FailureCause!=-1) return false;
    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
        return true;
    if (m1.n == m2.n) {
        if (m1.waste == 0) return true;
        else if (m2.waste == 0) return false;

        if (m1.arrival_del < m2.arrival_del) {
            return true;
        } else if (m1.arrival_del == m2.arrival_del) {

            if (m1.allDriver < m2.allDriver)
                return true;

//            if(m1.DeltaCost.travelCost < m2.DeltaCost.travelCost) return true;

            if (m1.waste * m2.waste == 0) {
                if (m1.waste == 0)
                    return true;
                return false;
            } else if (m1.waste * m2.waste < 0) {
                if (m1.waste > 0) return true;
            } else {
                if (std::abs(m1.waste) < std::abs(m2.waste)) return true;
                else if (m1.waste == m2.waste) {
                    return (m1.to->capacity > m2.to->capacity);
                }
                return false;
            }
        } else {
            if (m1.allDriver < m2.allDriver)
                return true;
        }


//          if(m1.waste < m2.waste)
//              return true;
        return false;
    }
    return (m1 < m2);
}

bool ListMoveVrp::Sort3(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2) {
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
    if(m2.FailureCause!=Parameters::FAILURECAUSE::NONE) return true;
    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
        return true;
    if (m1.n == m2.n) {
        if (m1.waste == 0) return true;
        else if (m2.waste == 0) return false;
        if (m1.DeltaCost.travelCost < m2.DeltaCost.travelCost) {
            return true;
        }
        if (m1.arrival_del < m2.arrival_del) {
            return true;
        } else if (m1.arrival_del == m2.arrival_del) {

            if (m1.allDriver < m2.allDriver)
                return true;

//            if(m1.DeltaCost.travelCost < m2.DeltaCost.travelCost) return true;

            if (m1.waste * m2.waste == 0) {
                if (m1.waste == 0)
                    return true;
                return false;
            } else if (m1.waste * m2.waste < 0) {
                if (m1.waste > 0) return true;
            } else {
                if (std::abs(m1.waste) < std::abs(m2.waste)) return true;
                else if (m1.waste == m2.waste) {
                    return (m1.to->capacity > m2.to->capacity);
                }
                return false;
            }
        } else {
            if (m1.allDriver < m2.allDriver)
                return true;
        }


//          if(m1.waste < m2.waste)
//              return true;
        return false;
    }
    return (m1 < m2);
}

bool ListMoveVrp::Sort4(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2){
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
//    if(m1.DeltaCost.lateDeliveryCost<m2.DeltaCost.lateDeliveryCost)
//        return true;

    if(m1.DeltaCost.undeliveredCost<m2.DeltaCost.undeliveredCost) return true;
    if(m1.DeltaCost.undeliveredCost>m2.DeltaCost.undeliveredCost) return false;
    if(m1.allDriver<m2.allDriver) return true;
    else{
        if(m1.clientDriver < m2.clientDriver)
            return true;
//        else if(m1.clientDriver==m2.clientDriver){
//            return(m1.waste<m2.waste);
//        }
    }
//    if (m1.waste == 0) return true;
//    else if (m2.waste == 0) return false;
    return (m1 < m2);
}
bool ListMoveVrp::Sort5(Move<Delivery, Driver, MoveVrp> &m1, Move<Delivery, Driver, MoveVrp> &m2){
    if(m1.FailureCause!=Parameters::FAILURECAUSE::NONE) return false;
    assert(m1.n!= nullptr);
    assert(m1.to!= nullptr);
    assert(m2.n!= nullptr);
    assert(m2.to!= nullptr);
    if (m1.DeltaCost.lateDeliveryCost < m2.DeltaCost.lateDeliveryCost) {
        return true;
    } else if (m1.DeltaCost.lateDeliveryCost == m2.DeltaCost.lateDeliveryCost) {
        if (m1.waste == 0)
            return true;
        else if (m2.waste == 0)
            return false;
        if (m1.arrival_del < m2.arrival_del) {
            return true;
        } else {
            if (m1.waste == m2.waste) {
                return (m1.to->capacity > m2.to->capacity);
            } else
                return (std::abs(m1.waste) < std::abs(m2.waste));
        }
    }
    return false;
}


ListMoveVrp::ListMoveVrp() : _moves(0) {
    moveMap.clear();
}

void ListMoveVrp::Clear() { _moves.clear(); moveMap.clear();}

void ListMoveVrp::Add(Move<Delivery, Driver, MoveVrp> &m) {

    assert(m.n!= nullptr);
    assert(m.to!= nullptr);

    const string str=m.toString();
    if(moveMap.contains(str))
        return;
    moveMap[str]=m;
    /*for(size_t i=0;i<_moves.size();i++)
            if(_moves[i].DeltaCost < m.DeltaCost && _moves[i].move.AddedTime <
       m.move.AddedTime) return;*/
//    cout<<"cap1 "<< _moves.capacity();
    _moves.push_back(m);
//    cout<<" cap2 "<< _moves.capacity()<<endl;
}
void ListMoveVrp::Insert( ListMoveVrp &to_insert){

    for(int i=to_insert.Count()-1;i>=0;i--){
        auto mv = to_insert.Get(i);
        if(moveMap.contains(mv.toString()))
            continue;
        moveMap[mv.toString()]=mv;
        _moves.insert( _moves.begin(), mv);
    }
}

Move<Delivery, Driver, MoveVrp> &ListMoveVrp::Get(int i) { return _moves[i]; }

int ListMoveVrp::Count() { return (int) _moves.size(); }

Move<Delivery, Driver, MoveVrp> ListMoveVrp::Extract() {
    Move<Delivery, Driver, MoveVrp> temp = _moves[0];
    _moves.erase(_moves.begin());
    return temp;
}

void ListMoveVrp::Sort() {

    if(Parameters::SORT_TYPE == Parameters::SORT::SHUFFLE){
        std::shuffle(_moves.begin(), _moves.end(),Parameters::RANDOM_GEN);
        return;
    }
    partial_Sort(Count());
    return;

//    Show();
    sort(_moves.begin(), _moves.end(), [this](Move<Delivery, Driver, MoveVrp> &m1,
                                              Move<Delivery, Driver, MoveVrp> &m2) -> bool {
        if (Parameters::SORT_TYPE == Parameters::SORT::ONE) {
            return Sort1(m1, m2);
        } else if (Parameters::SORT_TYPE == Parameters::SORT::TWO) {
            return this->Sort2(m1, m2);
        }
        else if (Parameters::SORT_TYPE == Parameters::SORT::THREE) {
            return this->Sort3(m1, m2);
        }
        else if (Parameters::SORT_TYPE == Parameters::SORT::FOUR) {
            return this->Sort4(m1, m2);
        }else if (Parameters::SORT_TYPE == Parameters::SORT::FIVE) {
            assert(m1.n!= nullptr);
            assert(m2.n!= nullptr);
            return this->Sort5(m1, m2);
        }
        else {
            return this->Sort2(m1, m2);
        }
    });

}

void ListMoveVrp::Insert(int i, Move<Delivery, Driver, MoveVrp> &m) {
    _moves.at(i) = m;
}

void ListMoveVrp::Resize(int n) { _moves.resize(n); }

void ListMoveVrp::partial_Sort(int k) {
    partial_sort(_moves.begin(), _moves.begin() + k, _moves.end(), move_sorter);
}

void ListMoveVrp::Show() {

    cout << "Count " << _moves.size() << endl;
    for (auto m: _moves) {
        cout << m.move.prev->id << "->" << m.to->id << "--" << m.n->id << "(" << m.n->orderID << ")--" << "("
             << m.n->rank << ")|"
             << m.waste << " --" << m.arrival_del << "--(" << m.clientDriver << "-"<<m.allDriver<< ")-- F:"
             << m.FailureCause <<"->" ;
        cout << m.DeltaCost << endl;
    }
}

Move<Delivery, Driver, MoveVrp> &ListMoveVrp::GetRandom() {
    uniform_int_distribution<int> dis2(0, 10);
    uniform_int_distribution<int> dis(0, std::min(Count() - 1, 3));
    const int k = dis(Parameters::RANDOM_GEN);
    Sort();
//    partial_Sort(Count() - 1);
    const int choice = dis2(Parameters::RANDOM_GEN);
    if (choice < 4) {
        return _moves[0];
    } else if (choice < 8) {
        return _moves[k];
    } else {
        int i = k;
        while (i >= 1) {
            if (i - 1 >= 0) {
                if (_moves[i - 1].DeltaCost.lateDeliveryCost <
                    _moves[i].DeltaCost.lateDeliveryCost) {
                    i--;
                    continue;
                } else
                    return _moves[i];
            }
            break;
        }
        return _moves[i];
    }
}

class ListMoveVrp;
