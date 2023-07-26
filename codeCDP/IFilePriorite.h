
#ifndef _IFILE_PRIORITE_H
#define _IFILE_PRIORITE_H

template <class T>
 class IFilePriorite {
public:
    IFilePriorite() {}
    virtual ~IFilePriorite() {}
    //Ajouter un noeud ayant une certaine valeur
virtual void Ajouter(int noeud, T valeur) = 0;

    //retourne le noeud de plus petite valeur
    virtual int SupprimerMin() = 0;

    virtual void Supprimer(int k) = 0;

    //retourne le noeud de plus petite valeur sans supprimer
    virtual int GetMin() = 0;

    //Reduit la valeur associe au noeud
    virtual void Decroitre(int noeud, T valeur) = 0;

    virtual bool EstVide() = 0;

    virtual void Vider() = 0;
};

#endif // _IFILE_PRIORITE_H


