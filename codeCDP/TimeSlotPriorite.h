
#ifndef _TimeSlot_PRIORITE_H
#define _TimeSlot_PRIORITE_H


#include<vector>
#include<algorithm>
#include<cmath>

#include "IFilePriorite.h"
#include "Data.h"
#include "Solution.h"
#include "TimeSlot.h"

class TimeSlotPriorite : public IFilePriorite<TimeSlot> {
public:
  TimeSlotPriorite()
      : _data(nullptr), valeurs(0), tabuList(0), tableauCount(0), tabuCount(0),
        count(0) {
    MAX_INTERVAL = TimeSlot(0, 1440);
  }
  TimeSlotPriorite(Data *pr)
      : _data(pr), valeurs(0), tabuList(0), tableauCount(0), tabuCount(0),
        count(0) {
    MAX_INTERVAL = TimeSlot(0, 1440);
  }
  TimeSlotPriorite(Data *pr, int n);
  TimeSlotPriorite(int n);

  // Ajouter un noeud ayant une certaine valeur
  void Ajouter(int noeud, TimeSlot valeur) override;

  // retourne le noeud de plus petite valeur
  int SupprimerMin()override;
  int GetMin()override;

  // Reduit la valeur associe au noeud
  void Decroitre(int noeud, TimeSlot valeur)override;

  bool EstVide()override;

  void Vider()override;
  void ViderTabuList();

  int TabuListCount() const { return tabuCount; }
  int Size() const { return (int)valeurs.size(); }
  TimeSlot Get(int i) { return valeurs[i]; }

  std::vector<int> GetMins();

  void Supprimer(int cle)override;

private:
  TimeSlot MAX_INTERVAL;
  std::vector<TimeSlot> valeurs;
  std::vector<int> tabuList;
  int tableauCount;
  int tabuCount;
  Data *_data;
  int count;
};

#endif // _INTERVAL_PRIORITE_H
