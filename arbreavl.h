/* Squelette pour classe générique ArbreAVL<T>.
 * UQAM / Département d'informatique
 * INF3105 - Structures de données et algorithmes
 * http://ericbeaudry.uqam.ca/INF3105/lab7/
 *
 * Rappel: il y a 4 blocs insérés dans le Lab7 par rapport au lab6
 *
 */
#if !defined(_ARBREAVL__H_E26_)
#define _ARBREAVL__H_E26_
#include <cassert>
#include <iostream>
#include "pile.h"

template <class T>
class ArbreAVL {
    public:
        ArbreAVL();
        ~ArbreAVL();

        bool vide() const;
        bool contient(const T&) const;
        void inserer(const T&);
        void vider();

        void enlever(const T&);

        ArbreAVL& operator=(const ArbreAVL&);
        ArbreAVL(const ArbreAVL&);

        int hauteur() const;
        void afficher() const;
        int compter() const;

        int compter(const T min, const T max) const;
        bool operator==(const ArbreAVL&) const;
        ArbreAVL<T> sous_ensemble(const T min, const T max) const;

        class Iterateur;

        Iterateur debut() const;
        Iterateur fin() const;
        Iterateur rechercher(const T&) const;
        Iterateur rechercherEgalOuSuivant(const T&) const;
        Iterateur rechercherEgalOuPrecedent(const T&) const;

        const T& operator[](const Iterateur&) const;
        T& operator[](const Iterateur&);
    private:
        struct Noeud {
            Noeud(const T&);
            T contenu;
            int equilibre;
            bool enleve;
            int nbEnfants;
            Noeud *gauche,
                  *droite;
        };
        Noeud* racine;

        // Fonctions internes
        bool inserer(Noeud*&, const T&);
        void rotationGaucheDroite(Noeud*&);
        void rotationDroiteGauche(Noeud*&);
        void vider(Noeud*&);
        int compter(const Noeud*) const;
        int compter(const Noeud*, const T&, const T&, bool, const T&, bool, const T&) const;

        void copier(const Noeud*, Noeud*&) const;

        int hauteur(const Noeud*) const;
        void preparer_afficher(const Noeud* n, int profondeur, int& rang, T* elements, int* profondeurs) const;
        int incrementerNbEnfants(const Noeud* n);
        int nbInferieurOuEgal(const Noeud* n, const T& valeur) const;

    public:
        class Iterateur{
            public:
                Iterateur(const ArbreAVL& a);
                Iterateur(const Iterateur& a);
                Iterateur(const ArbreAVL& a, Noeud* c);

                operator bool() const;
                bool operator!() const;
                bool operator==(const Iterateur&) const;
                bool operator!=(const Iterateur&) const;

                const T& operator*() const;

                Iterateur& operator++();
                Iterateur operator++(int);
                Iterateur& operator=(const Iterateur&);
            private:
                const ArbreAVL& arbre_associe;
                Noeud* courant;
                Pile<Noeud*> chemin;

            friend class ArbreAVL;
        };
};

template <class T>
ArbreAVL<T>::Noeud::Noeud(const T& c)
 : contenu(c), equilibre(0), enleve(false), nbEnfants(0), gauche(nullptr), droite(nullptr) {}

template <class T>
ArbreAVL<T>::ArbreAVL(): racine(nullptr) {}

template <class T>
ArbreAVL<T>::ArbreAVL(const ArbreAVL<T>& autre): racine(nullptr) {
    copier(autre.racine, racine);
}

template <class T>
ArbreAVL<T>::~ArbreAVL() {
    vider(racine);
}

template <class T>
bool ArbreAVL<T>::contient(const T& element) const {
    Noeud *current = racine;

    while(current)
        if(current->contenu < element)
            current = current->droite;
        else if (element < current->contenu)
            current = current->gauche;
        else
            return !current->enleve;

    return false;
}

// Fonctionnalite TP3

template <class T>
int ArbreAVL<T>::nbInferieurOuEgal(const Noeud* n, const T& x) const {
    if(n == nullptr) return 0;

    if(x < n->contenu) {
        return nbInferieurOuEgal(n->gauche, x);
    }

    int cpt = 0;
    if(!n->enleve) cpt = cpt + 1;

    if(n->gauche != nullptr)
        cpt += n->gauche->nbEnfants + 1;

    cpt += nbInferieurOuEgal(n->droite, x);

    return cpt;
}

template <class T>
int ArbreAVL<T>::compter(const T min, const T max) const {

    if(min > max) return 0;

    Iterateur it1 = rechercherEgalOuSuivant(min);
    Iterateur it2 = rechercherEgalOuPrecedent(max);

    if(!it1) return 0;
    if(!it2) return 0;
    if(max < *it1) return 0;

    int r1 = nbInferieurOuEgal(racine, *it1);
    int r2 = nbInferieurOuEgal(racine, *it2);

    return r2 - r1 + 1;
}

template <class T>
ArbreAVL<T> ArbreAVL<T>::sous_ensemble(const T min, const T max) const {
    if(min > max)
        return ArbreAVL<T>();

    ArbreAVL<T> sousArbre;
    for(Iterateur iter = rechercherEgalOuSuivant(min); iter && *iter <= max; ++iter) {
        if(max < *iter)
            break; 
        if (!iter.courant->enleve) {
            sousArbre.inserer(iter.courant->contenu);
        }
    }
    return sousArbre;
}

template <class T>
bool ArbreAVL<T>::operator==(const ArbreAVL& autre) const {
    if(this == &autre) return true;
    if(compter() != autre.compter()) return false;

    Iterateur iter1 = debut();
    Iterateur iter2 = autre.debut();

    while(iter1 && iter2) {
        if(*iter1 != *iter2)
            return false;
        ++iter1;
        ++iter2;
    }
    return !iter1 && !iter2;
}

template <class T>
void ArbreAVL<T>::inserer(const T& element) {
    inserer(racine, element);
}

template <class T>
bool ArbreAVL<T>::inserer(Noeud*& noeud, const T& element) {
    if(noeud == nullptr) {
        noeud = new Noeud(element);
        return true;
    }

    if(element < noeud->contenu) {
        bool grandi = inserer(noeud->gauche, element);

        noeud->nbEnfants = incrementerNbEnfants(noeud);
        if(grandi) {
            ++noeud->equilibre;

            if(noeud->equilibre == 0)
                return false;
            if(noeud->equilibre == 1)
                return true;
            assert(noeud->equilibre == 2);

            if(noeud->gauche->equilibre == -1)
                rotationDroiteGauche(noeud->gauche);
            rotationGaucheDroite(noeud);
        }
        return false;
    } else if(noeud->contenu < element) {
        bool grandi = inserer(noeud->droite, element);
        noeud->nbEnfants = incrementerNbEnfants(noeud);
        if(grandi) {
            --noeud->equilibre;
            if(noeud->equilibre == 0)
                return false;
            if(noeud->equilibre == -1)
                return true;
            assert(noeud->equilibre == -2);

            if(noeud->droite->equilibre == 1)
                rotationGaucheDroite(noeud->droite);
            rotationDroiteGauche(noeud);
        }
        return false;
    }

    noeud->enleve = false;
    noeud->contenu = element;
    return false;
}

template <class T>
int ArbreAVL<T>::incrementerNbEnfants(const Noeud* n) {
    if(!n) return 0;
    int nbEnfants = 0;
    if (n->gauche)
        nbEnfants += n->gauche->nbEnfants + 1;
    if (n->droite)
        nbEnfants += n->droite->nbEnfants + 1;
    return nbEnfants;
}

template <class T>
void ArbreAVL<T>::rotationGaucheDroite(Noeud*& racinesousarbre) {
    Noeud *temp = racinesousarbre->gauche;
    int  ea = temp->equilibre;
    int  eb = racinesousarbre->equilibre;
    int  neb = -(ea>0 ? ea : 0) - 1 + eb;
    int  nea = ea + (neb < 0 ? neb : 0) - 1;

    temp->equilibre = nea;
    racinesousarbre->equilibre = neb;
    racinesousarbre->gauche = temp->droite;

    racinesousarbre->nbEnfants = incrementerNbEnfants(racinesousarbre);
    temp->nbEnfants = incrementerNbEnfants(temp);

    temp->droite = racinesousarbre;
    racinesousarbre = temp;
}

template <class T>
void ArbreAVL<T>::rotationDroiteGauche(Noeud*& racinesousarbre) {
    Noeud *temp = racinesousarbre->droite;
    int  eb = temp->equilibre;
    int  ea = racinesousarbre->equilibre;

    int nea = ea - (eb < 0 ? eb : 0) + 1;
    int neb = eb + (nea > 0 ? nea : 0) + 1;

    temp->equilibre = neb;
    racinesousarbre->equilibre = nea;
    racinesousarbre->droite = temp->gauche;
    temp->gauche = racinesousarbre;
    racinesousarbre->nbEnfants = incrementerNbEnfants(racinesousarbre);
    temp->nbEnfants = incrementerNbEnfants(temp);
    racinesousarbre = temp;
}

template <class T>
bool ArbreAVL<T>::vide() const {
    return !racine;
}

template <class T>
void ArbreAVL<T>::vider() {
  vider(racine);
}

template <class T>
void ArbreAVL<T>::vider(Noeud*& noeud) {
    if (!noeud) {
        return;
    }
    // postordre
    vider(noeud->gauche);
    vider(noeud->droite);
    delete noeud;
    noeud = nullptr;
}

template <class T>
void ArbreAVL<T>::copier(const Noeud* source, Noeud*& noeud) const {
    // préordre
    if (source == nullptr) {
        return;
    }

    noeud = new Noeud(source->contenu);

    copier(source->gauche, noeud->gauche);
    copier(source->droite, noeud->droite);
}

template <class T>
int  ArbreAVL<T>::hauteur() const {
    return hauteur(racine);
}

template <class T>
int  ArbreAVL<T>::compter() const {
    return compter(racine);
}

template <class T>
inline T max(T i, T j) {
  return i > j ? i : j;
}

template <class T>
int  ArbreAVL<T>::hauteur(const Noeud* n) const {
    if(n == nullptr)
        return 0;

    return 1 + max(hauteur(n->gauche), hauteur(n->droite));
}

template <class T>
int ArbreAVL<T>::compter(const Noeud* n) const{
    if(!n) return 0;

    return compter(n->gauche) + compter(n->droite) + !n->enleve;
}

template <class T>
ArbreAVL<T>& ArbreAVL<T>::operator=(const ArbreAVL& autre) {
    if(this == &autre) return *this;

    vider();
    copier(autre.racine, racine);
    return *this;
}

template <class T>
void ArbreAVL<T>::enlever(const T& element) {
    Noeud *current = racine;
    while(current)
        if(current->contenu < element)
            current = current->droite;
        else if (element < current->contenu)
            current = current->gauche;
        else {
            assert(!current->enleve);
            current->enleve = true;
            return;
        }
}

template <class T>
void ArbreAVL<T>::afficher() const {
    std::cout << "Contenu de l'arbre AVL (";
    int n = compter();
    std::cout << n << " noeuds) :\n";
    T* elements = new T[n];
    int* profondeurs = new int[n];
    n = 0;
    preparer_afficher(racine, 0, n, elements, profondeurs);
    for(int p = 0; ; p++) {
        bool derniereprofondeur = true;
        for(int i = 0; i < n; i++) {
            if(profondeurs[i] == p) {
                std::cout << elements[i];
                derniereprofondeur = false;
            }
            std::cout << '\t';
        }
        std::cout << '\n';
        if(derniereprofondeur) break;
    }
    delete[] elements;
    delete[] profondeurs;
    std::cout << "--------------" << std::endl;
}

template <class T>
void ArbreAVL<T>::preparer_afficher(const Noeud* n, int profondeur, int& rang, T* elements, int* profondeurs) const {
    if(!n) return;
    preparer_afficher(n->gauche, profondeur + 1, rang, elements, profondeurs);
    elements[rang] = n->contenu;
    profondeurs[rang] = profondeur;
    rang++;
    preparer_afficher(n->droite, profondeur + 1, rang, elements, profondeurs);
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::debut() const {
    if (!racine) return fin();

    Iterateur iter(*this, racine);
    Noeud*& courant = iter.courant;

    for(; courant->gauche; courant = courant->gauche)
        iter.chemin.empiler(courant);

    if(courant->enleve)
        ++iter;
    
    return iter;
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::fin() const {
    return Iterateur(*this);
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercher(const T& e) const {
    Iterateur iter(*this, racine);

    Noeud*& n = iter.courant;

    while(n) {
        if (e < n->contenu) {
            iter.chemin.empiler(n);
            n = n->gauche;
        } else if (n->contenu < e) {
            n = n->droite;
        } else {
            return n->enleve ? fin() : iter;
        }
    }
    return fin();
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercherEgalOuSuivant(const T& e) const {
    Iterateur iter(*this);
    Noeud* courant = racine;

    while(courant)
        if(courant->contenu < e) {
            courant = courant->droite;
        } else if (e < courant->contenu) {
            iter.chemin.empiler(courant);
            courant = courant->gauche;
        } else {
            iter.courant = courant;
            break;
        }
    if (!iter.courant) {
        if(iter.chemin.vide())
            return fin();
        iter.courant = iter.chemin.depiler();
    }

    if(iter.courant->enleve)
        ++iter;
    return iter;
}

template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::rechercherEgalOuPrecedent(const T& e) const {
    Noeud *best = nullptr;
    Noeud* courant = racine;

    while(courant)
        if(e < courant->contenu) {
            courant = courant->gauche;
        } else if (courant->contenu < e) {
            best = courant;
            courant = courant->droite;
        } else {
            best = courant;
            break;
        }
    if (!best)
        return fin();
    if(!best->enleve)
        return rechercher(best->contenu);

    courant = racine;
    Pile<Noeud*> chemin;

    while(courant != best) {
        if(courant->contenu < best->contenu) {
            chemin.empiler(courant);
            courant = courant->droite;
        } else {
            courant = courant->gauche;
        }
    }

    while(courant->enleve)
        if(courant->gauche)
            for(courant = courant->gauche; courant->droite; courant = courant->droite)
                chemin.empiler(courant);
        else if (!chemin.vide())
            courant = chemin.depiler();
        else
            return fin();
    return rechercher(courant->contenu);
}

template <class T>
const T& ArbreAVL<T>::operator[](const Iterateur& iterateur) const {
    assert(&iterateur.arbre_associe == this);
    assert(iterateur.courant);
    return iterateur.courant->contenu;
}

template <class T>
T& ArbreAVL<T>::operator[](const Iterateur& iterateur) {
    assert(&iterateur.arbre_associe == this);
    assert(iterateur.courant);
    return iterateur.courant->contenu;
}

template <class T>
ArbreAVL<T>::Iterateur::Iterateur(const ArbreAVL& a)
: arbre_associe(a), courant(nullptr) {}

template <class T>
ArbreAVL<T>::Iterateur::Iterateur(const ArbreAVL<T>::Iterateur& a)
: arbre_associe(a.arbre_associe), courant(a.courant), chemin(a.chemin) {}

template <class T>
ArbreAVL<T>::Iterateur::Iterateur(const ArbreAVL& a, Noeud* c)
: arbre_associe(a), courant(c) {}

// Pré-incrément
template <class T>
typename ArbreAVL<T>::Iterateur& ArbreAVL<T>::Iterateur::operator++() {
    assert(courant);

    if(courant->droite)
        for(courant = courant->droite; courant->gauche; courant = courant->gauche)
            chemin.empiler(courant);
    else
        courant = chemin.vide() ? nullptr : chemin.depiler();

    if(courant && courant->enleve)
        operator++();

    return *this;
}

// Post-incrément
template <class T>
typename ArbreAVL<T>::Iterateur ArbreAVL<T>::Iterateur::operator++(int) {
    Iterateur copie(*this);
    operator++();
    return copie;
}

template <class T>
ArbreAVL<T>::Iterateur::operator bool() const {
    return courant;
}

template <class T>
bool ArbreAVL<T>::Iterateur::operator!() const {
    return !courant;
}

template <class T>
bool ArbreAVL<T>::Iterateur::operator==(const Iterateur& o) const {
    assert(&arbre_associe == &o.arbre_associe);
    return courant == o.courant;
}

template <class T>
bool ArbreAVL<T>::Iterateur::operator!=(const Iterateur& o) const {
    assert(&arbre_associe == &o.arbre_associe);
    return courant != o.courant;
}

template <class T>
const T& ArbreAVL<T>::Iterateur::operator*() const {
    assert(courant);
    return courant->contenu;
}

template <class T>
typename ArbreAVL<T>::Iterateur& ArbreAVL<T>::Iterateur::operator=(const Iterateur& autre) {
    assert(&arbre_associe == &autre.arbre_associe);
    courant = autre.courant;
    // Pile n'a pas d'operator= : on vide chemin puis on le reconstruit
    // à partir d'une copie de autre.chemin (constructeur par copie de Pile).
    chemin.vider();
    Pile<Noeud*> copie(autre.chemin);
    Pile<Noeud*> renverse;
    while(!copie.vide())
        renverse.empiler(copie.depiler());
    while(!renverse.vide())
        chemin.empiler(renverse.depiler());
    return *this;
}

#endif
