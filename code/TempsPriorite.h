
#ifndef _TABLEAU_PRIORITE_H
#define _TABLEAU_PRIORITE_H

#define MAX_VALUE 10000

#include<vector>
#include<algorithm>
#include<cmath>

#include "IFilePriorite.h"
#include "Data.h"

class TempsPriorite : public IFilePriorite<double> {
public:
    TempsPriorite():_data(nullptr),valeurs(0), tabuList(0), tableauCount(0), tabuCount(0),count(0) {}
    TempsPriorite(Data *pdata): _data(pdata), valeurs(0), tabuList(0), tableauCount(0), tabuCount(0),
          count(0) {}
    TempsPriorite(Data *data,int n);
    TempsPriorite(int n);

    //Ajouter un noeud ayant une certaine valeur
    void Ajouter(int noeud, double valeur) override;

    //retourne le noeud de plus petite valeur
    int SupprimerMin()override;
    int GetMin()override;


    //Reduit la valeur associe au noeud
    void Decroitre(int noeud, double valeur)override;

    bool EstVide() override;

    void Vider()override;
    void ViderTabuList();

    int TabuListCount() const { return tabuCount; }
    int Size() const { return (int)valeurs.size(); }
    double Get(int i) { return valeurs[i]; }

    std::vector<int> GetMins();

    void Supprimer(int cle)override;

  private:
    std::vector<double> valeurs;
    std::vector<int> tabuList;
    int tableauCount;
    int tabuCount;
    Data *_data;
    int count;
};

#endif // _TABLEAU_PRIORITE_H


