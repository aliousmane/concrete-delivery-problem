

#include "TempsPriorite.h"
#include "lib/mathfunc.h"

TempsPriorite::TempsPriorite(Data *pr, int n)
        : _data(pr), valeurs(n, MAX_VALUE), tabuList(n, -1), tableauCount(n),
          tabuCount(0), count(0) {}

TempsPriorite::TempsPriorite(int n)
        : _data(nullptr), valeurs(n, MAX_VALUE), tabuList(n, -1), tableauCount(n),
          tabuCount(0), count(0) {}

// Ajouter un noeud ayant une certaine valeur
void TempsPriorite::Ajouter(int cle, double valeur) {
    valeurs[cle] = std::min(valeurs[cle], valeur);
    count++;
}

void TempsPriorite::Supprimer(int cle) {
    valeurs[cle] = MAX_VALUE;
    count--;
}

// retourne le noeud de plus petite valeur
int TempsPriorite::SupprimerMin() {

    int index_min = -1;
    double valeur_min = MAX_VALUE;
    for (int i = 0; i < valeurs.size(); i++) {
        if (valeur_min > valeurs[i]) {
            valeur_min = valeurs[i];
            index_min = i;
        } else if (valeur_min != MAX_VALUE && valeur_min == valeurs[i]) {
            if (_data != nullptr) {
                const int custID1 = _data->GetDelivery(index_min)->custID;
                const int custID2 = _data->GetDelivery(i)->custID;

                double dmd =
                        _data->GetCustomer(custID1)->demand + _data->GetCustomer(custID2)->demand;
                double val = mat_func_get_rand_double();
                // if (_data->GetNode(index_min)->demand/dmd > val) {
                if (val < 0.5) {
                    valeur_min = valeurs[i];
                    index_min = i;
                }
            }
        }
    }
    // tabuList[index_min] = -1;
    valeurs[index_min] = MAX_VALUE;
    count--;
    return index_min;
}

// retourne le noeud de plus petite valeur
int TempsPriorite::GetMin() {

    auto min_it = std::min_element(valeurs.begin(), valeurs.end());
    int index_min = (int)std::distance(valeurs.begin(), min_it);

    if (index_min != -1) {
        tabuList[index_min] = index_min;
        tabuCount++;
    }
    return index_min;

}

std::vector<int> TempsPriorite::GetMins() {
    std::vector<int> list_min;
    auto valeur_min = std::min_element(valeurs.begin(), valeurs.end());
    for (int i = 0; i < valeurs.size(); i++) {
        if (*valeur_min == valeurs[i]) {
            list_min.push_back(i);
        }
    }
    return list_min;
}

// Reduit la valeur associe au noeud
void TempsPriorite::Decroitre(int noeud, double valeur) {
    valeurs[noeud] = std::min(valeurs[noeud], valeur);
    count++;
}

bool TempsPriorite::EstVide() {
    for (auto valeur : valeurs)
        if (valeur < MAX_VALUE)
            return false;
    return true;
}

void TempsPriorite::Vider() {
    std::fill(valeurs.begin(), valeurs.end(), MAX_VALUE);
}

void TempsPriorite::ViderTabuList() {
    std::fill(tabuList.begin(), tabuList.end(), -1);
    tabuCount = 0;
}
