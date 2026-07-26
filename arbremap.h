/* Squelette pour classe générique ArbreMap<K, V>.
 * Lab8 -- Dictionnaires et Arbres binaires de recherche
 * UQAM / Département d'informatique
 * INF3105 - Structures de données et algorithmes
 * http://ericbeaudry.uqam.ca/INF3105/lab8/
 */
#if !defined(__ARBREMAP_H_E26_)
#define __ARBREMAP_H_E26_
#include "arbreavl.h"

template<class K, class V>
class ArbreMap {
    public:
        bool contient(const K&) const;

        void enlever(const K&);
        void vider();
        bool vide() const;

        const V& operator[](const K&) const;
        V& operator[](const K&);
    private:
        struct Node {
            K clé;
            V valeur;

            Node(K cle): clé(cle){};
            bool operator<(const Node& noeud) const {
                return clé < noeud.clé;
            }
        };
        ArbreAVL<Node> arbreAvl;

};

template<class K, class V>
void ArbreMap<K,V>::vider() {
    arbreAvl.vider();
}

template<class K, class V>
bool ArbreMap<K,V>::vide() const {
    return arbreAvl.vide();
}

template<class K, class V>
void ArbreMap<K,V>::enlever(const K& c) {
    Node n;
    n.clé = c;
    arbreAvl.enlever(n);
}

template<class K, class V>
bool ArbreMap<K,V>::contient(const K& c) const {
    Node n(c);
    return arbreAvl.contient(n);
}

template<class K, class V>
const V& ArbreMap<K,V>::operator[](const K& c) const {
    Node n;
    n.clé = c;
    typename ArbreAVL<Node>::Iterateur iter = arbreAvl.rechercher(n);
    return arbreAvl[iter].valeur;
}

template<class K, class V>
V& ArbreMap<K,V>::operator[](const K& c) {
    Node n(c);
    typename ArbreAVL<Node>::Iterateur iter = arbreAvl.rechercher(n);
    if (iter) {
        return arbreAvl[iter].valeur;
    }
    arbreAvl.inserer(n);
    iter = arbreAvl.rechercher(n);
    return arbreAvl[iter].valeur;
}

#endif
