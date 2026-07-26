/*  INF3105 - Structures de données et algorithmes       *
 *  UQAM / Département d'informatique                    *
 *  Pile pour intérateurs ArbreAVL pour Lab7             *
 *  http://ericbeaudry.uqam.ca/INF3105/lab7/             */

#if !defined(__PILE__H__)
#define __PILE__H__

template <class T>
class Pile
{
  public:
    Pile();
    Pile(const Pile&);
    ~Pile();

    void empiler(const T&);
  
    void depiler(T& out);  // Depiler et copier le sommet dans out.
    T depiler();

    bool vide() const { return sommet==nullptr;}
    void vider();

    const Pile<T>&    operator = (const Pile<T>&);

  private:
    //Pile(const Pile&); // on pourrait préférer mettre le constructeur par copie privé.
    
    class Cellule
    {
      public:
        Cellule(const T& e, Cellule* c);
        T contenu;
        Cellule* suivante;
    };

    Cellule* sommet;
};

/*
 Puisque Pile<T> est une classe générique, toutes ses définitions doivent être
 inclues après les déclarations. C'est ce qui est fait plus bas.

 On pourrait quand même séparer les déclarations et les définitions en
 deux fichiers (ex.: pile.h et pile.hcc), à condition d'inclure le
 deuxième (pile.hcc) à la fin du premier (pile.h). Ainsi, un fichier source
 (.cc, .cpp, c++) qui inclut pile.h inclura indirectement aussi pile.hcc.
*/
//#include "pile.hcc"
/**** Début des définitions (qui auraient pues être mises dans un fichier source pile.hcc). ****/
// Les deux directives suivante sont utiles lorsqu'un fichier pile.hcc est utilisé.
#if !defined(__PILE_CPP__)
#define __PILE_CPP__
#include <cassert>

template <class T>
Pile<T>::Pile()
{
    sommet = nullptr;
}

template <class T>
Pile<T>::~Pile()
{
    vider();
}

template <class T>
Pile<T>::Pile(const Pile<T>& autre)
{
    sommet = nullptr;
    // version paresseuse : il suffit d'appeler l'opéreateur =
    *this = autre;
}


template <class T>
void Pile<T>::vider()
{
    while(!vide())
        depiler();  
}

template <class T>
Pile<T>::Cellule::Cellule(const T& e, Cellule* c)
 : suivante(c)
{
    contenu=e;
}

template <class T>
void Pile<T>::empiler(const T& e)
{
    sommet = new Cellule(e, sommet);
    assert(sommet);
}

template <class T>
T Pile<T>::depiler()
{
    assert(sommet);
    Cellule c(*sommet);
    delete sommet;
    sommet = c.suivante;
    return c.contenu;
}

template <class T>
void Pile<T>::depiler(T& e)
{
    assert(sommet!=nullptr);
    e = sommet->contenu;
    Cellule* c = sommet;
    sommet = sommet->suivante;
    delete c;
}

template <class T>
const Pile<T>& Pile<T>::operator = (const Pile<T>& autre)
{
    if(&autre == this) return *this;
 
    Cellule** t=&sommet;
    Cellule* a=autre.sommet;
    while(a){
        if((*t) == nullptr)
             *t = new Cellule(a->contenu, nullptr);
        else
             (*t)->contenu = a->contenu;
        t = &((*t)->suivante);
        a = a->suivante;
    }
    a = *t;
    *t = nullptr;
    while(a!=nullptr){
        Cellule* temp = a->suivante;
        delete a;
        a = temp;
    }

    return *this;
}

#endif
/**** Fin des définitions pouvant être mises dans un fichier pile.hcc. ****/

#endif
