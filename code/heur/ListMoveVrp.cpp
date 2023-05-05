#include <algorithm>
#include <iostream>
#include "ListMoveVrp.h"

using namespace std;

ListMoveVrp::ListMoveVrp() = default;

void ListMoveVrp::Clear() { _moves.clear(); }
void ListMoveVrp::Add(Move<Delivery, Driver, MoveVrp> &m)
{
  /*for(size_t i=0;i<_moves.size();i++)
          if(_moves[i].DeltaCost < m.DeltaCost && _moves[i].move.AddedTime <
     m.move.AddedTime) return;*/
  _moves.push_back(m);
}
Move<Delivery, Driver, MoveVrp> &ListMoveVrp::Get(int i) { return _moves[i]; }

int ListMoveVrp::Count() { return (int)_moves.size(); }

void ListMoveVrp::Sort() { sort(_moves.begin(), _moves.end()); }
void ListMoveVrp::Insert(int i, Move<Delivery, Driver, MoveVrp> &m)
{
  _moves.at(i) = m;
}
void ListMoveVrp::Resize(int n) { _moves.resize(n); }
void ListMoveVrp::partial_Sort(int k)
{
  partial_sort(_moves.begin(), _moves.begin() + k, _moves.end(), move_sorter);
}
void ListMoveVrp::Show()
{

  cout << "Count " << _moves.size() << endl;
  for (auto m : _moves)
  {
    cout << m.to->id << "--" << m.n->id << "(" << m.n->orderID << ")-- "
         << m.waste << " --" << m.arrival_del << "--";
    cout<<m.DeltaCost<<endl;
  }
}
Move<Delivery, Driver, MoveVrp> &ListMoveVrp::GetRandom()
{
  uniform_int_distribution<int> dis2(0, 10);
  uniform_int_distribution<int> dis(0, std::min(Count() - 1, 3));
  const int k = dis(Parameters::RANDOM_GEN);
  partial_Sort(Count() - 1);
  const int choice = dis2(Parameters::RANDOM_GEN);
  if (choice < 11)
  {
    return _moves[0];
  }
  else if (choice < 8)
  {
    return _moves[k];
  }
  else
  {
    int i = k;
    while (i >= 1)
    {
      if (i - 1 >= 0)
      {
        if (_moves[i - 1].DeltaCost.lateDeliveryCost <
            _moves[i].DeltaCost.lateDeliveryCost)
        {
          i--;
          continue;
        }
        else
          return _moves[i];
      }
      break;
    }
    return _moves[i];
  }
}

class ListMoveVrp;
