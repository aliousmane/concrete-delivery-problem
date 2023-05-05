

#include "TimeSlotPriorite.h"

TimeSlotPriorite::TimeSlotPriorite(Data *pr, int n)
    : _data(pr), tabuList(n, -1), tableauCount(n),
      tabuCount(0), count(0)
{
  MAX_INTERVAL = TimeSlot(1440, 1440);
  valeurs = std::vector<TimeSlot>(n, MAX_INTERVAL);
}
TimeSlotPriorite::TimeSlotPriorite(int n)
    : _data(nullptr), tabuList(n, -1), tableauCount(n),
      tabuCount(0), count(0)
{
  MAX_INTERVAL = TimeSlot(1440, 1440);
  valeurs = std::vector<TimeSlot>(n, MAX_INTERVAL);
}

// Ajouter un noeud ayant une certaine valeur
void TimeSlotPriorite::Ajouter(int cle, TimeSlot valeur)
{
  valeurs[cle] = valeur;
  count++;
}

void TimeSlotPriorite::Supprimer(int cle)
{
  valeurs[cle] = MAX_INTERVAL;
  count--;
}

// retourne le noeud de plus petite valeur
int TimeSlotPriorite::SupprimerMin()
{
  int index_min = -1;
  TimeSlot valeur_min = MAX_INTERVAL;
  for (int i = 0; i < valeurs.size(); i++)
  {
    if (valeur_min > valeurs[i])
    {
      valeur_min = valeurs[i];
      index_min = i;
    }
  }
  // tabuList[index_min] = -1;
  valeurs[index_min] = MAX_INTERVAL;
  count--;
  return index_min;
}
// retourne le noeud de plus petite valeur
int TimeSlotPriorite::GetMin()
{

  auto min_it = std::min_element(valeurs.begin(), valeurs.end());
  int index_min = std::distance(valeurs.begin(), min_it);

  if (index_min != -1)
  {
    tabuList[index_min] = index_min;
    tabuCount++;
  }
  return index_min;
}
std::vector<int> TimeSlotPriorite::GetMins()
{
  std::vector<int> list_min;
  std::vector<int> visited(valeurs.size(), -1);
  int index_min = -1;
  auto valeur_min = std::min_element(valeurs.begin(), valeurs.end());
  bool found = true;
  int count=0;
  while (found)
  {
    found = false;
    count++;
    for (int i = 0; i < valeurs.size(); i++)
    {
      if (visited[i] != -1)
        continue;
      auto val = valeurs[i];
      if (val == *valeur_min || *valeur_min == val)
      {

        valeur_min->lower = std::min(valeur_min->lower, val.lower);
        valeur_min->upper = std::max(valeur_min->upper, val.upper);
        // cout << "add " << i << endl;
        // cout<<"min"<<*valeur_min<<endl;
        // cout << valeurs[i] << endl;
        list_min.push_back(i);
        visited[i] = i;
        // found = true;
      }
      if(val==MAX_INTERVAL){
        visited[i] = i;
      }
    }
    // cout<<found<<endl;
  }
  // std::cout<<count<<std::endl;

  if (index_min != -1)
  {
    tabuList[index_min] = index_min;
    tabuCount++;
  }
  return list_min;
}
// Reduit la valeur associe au noeud
void TimeSlotPriorite::Decroitre(int noeud, TimeSlot valeur)
{
  valeurs[noeud] = std::min(valeurs[noeud], valeur);
  count++;
}

bool TimeSlotPriorite::EstVide()
{

  for (int i = 0; i < valeurs.size(); i++)
    if (valeurs[i] != MAX_INTERVAL)
      return false;
  return true;
}

void TimeSlotPriorite::Vider()
{
  std::fill(valeurs.begin(), valeurs.end(), MAX_INTERVAL);
}
void TimeSlotPriorite::ViderTabuList()
{
  std::fill(tabuList.begin(), tabuList.end(), -1);
  tabuCount = 0;
}
