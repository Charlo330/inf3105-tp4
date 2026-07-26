/* INF3105 / TP4
   
   Vos noms et codes parmaments:
    1) Prénom Nom (NOMP01234567) 
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "cinema.h"

int tp4(std::istream& entree) {
    Cinema cinema;
    // Lire entree commande par commmande (normalement, chacune est sur une ligne terminée par point-virgule).
    while(!entree.eof()) {
        std::string commande;
        entree >> commande;
        if(entree.fail() || commande.empty())
            break;
        if(commande=="AjouterSalle") {
            int id=0, nbPlaces=0;
            // Format: "AjouterSalle" ID NOMBRE_PLACE ";"
            entree >> id >> nbPlaces;
            bool ok = cinema.ajouterSalle(id, nbPlaces);
            std::cout << (ok ? "OK" : "Erreur") << std::endl;
        } else if(commande=="AjouterFilm") {
            std::string nom;
            int duree;
            // Format: "AjouterFilm" NOM DUREE ";"
            entree >> nom >> duree;
            bool ok = cinema.ajouterFilm(nom, duree*60);
            std::cout << (ok ? "OK" : "Erreur") << std::endl;
        } else if(commande=="Projeter") {
            std::string nomFilm;
            std::tm debut = {0, 0, 0, 0, 0, 0, 0, 0, -1};
            int idSalle;
            // Format : "Projeter" NOM_FILM ID_SALLE DATE_DEBUT HEURE_DEBUT ";"
            entree >> nomFilm >> idSalle >> std::get_time(&debut, "%Y-%m-%d %H:%M");
            bool ok = cinema.projeter(nomFilm, idSalle, debut);
            std::cout << (ok ? "OK" : "Erreur") << std::endl;
        } else if(commande=="Disponible") {
            std::tm debut = {0, 0, 0, 0, 0, 0, 0, 0, -1};
            std::tm fin = {0, 0, 0, 0, 0, 0, 0, 0, -1};
            int nbPlaces;
            // Format : "disponible" HEURE_DEBUT HEURE_FIN NOMBRE_DE_PLACE ";"
            entree >> std::get_time(&debut, "%Y-%m-%d %H:%M") >> std::get_time(&fin, "%Y-%m-%d %H:%M") >> nbPlaces;
            ArbreAVL<std::string> films = cinema.disponible(debut, fin, nbPlaces);
            for(ArbreAVL<std::string>::Iterateur i=films.debut();i;++i)
                std::cout << *i << ' ';
            std::cout << std::endl;
        } else if(commande=="Reserver") {
            std::string nomFilm;
            std::tm debutmin = {0, 0, 0, 0, 0, 0, 0, 0, -1};
            int nbPlaces;
            // Format : "reserver" NOM_FILM DATE_MIN HEURE_MIN NOMBRE_DE_PLACE ";"
            entree >> nomFilm >> std::get_time(&debutmin, "%Y-%m-%d %H:%M") >> nbPlaces;
            std::tm debut;
            int salle = cinema.reserver(nomFilm, debutmin, nbPlaces, debut);
            if(salle>=0)
                std::cout << '#' << salle << " @ " << std::put_time(&debut, "%Y-%m-%d %H:%M") << std::endl;
            else
                std::cout << "Erreur" << std::endl;
        } else if(commande=="NbClients") {
            // Commande : "nbclients" DATE_DEBUT HEURE_DEBUT DATE_FIN HEURE_FIN ";"
            std::tm debut = {0, 0, 0, 0, 0, 0, 0, 0, -1};
            std::tm fin = {0, 0, 0, 0, 0, 0, 0, 0, -1};
            entree >> std::get_time(&debut, "%Y-%m-%d %H:%M") >> std::get_time(&fin, "%Y-%m-%d %H:%M");
            int n = cinema.nbclients(debut, fin);
            std::cout << n << std::endl;
        } else {
            std::cout << "Commande \"" << commande << "\" invalide! Arrêt du programme." << std::endl;
            return 2;
        }
        
        // Lire le point-virgule de validation.
        char pointvirgule=0;
        entree >> pointvirgule;
        if(pointvirgule!=';'){
            std::cout << "Erreur : un point-virgule est attendu à la place de '" << pointvirgule << "' !" << std::endl;
            return 3;
        }
        // Forcer la lecture d'espaces blancs, au cas d'atteindre la fin (pour le test entree.eof()).
        entree >> std::ws;
    }
    return 0;
}

int main(int argc, char const *argv[]) {
    if(argc>=2){
        std::ifstream ifichier(argv[1]);
        if(ifichier.fail()) {
            std::cerr << "Erreur lors de l'ouverture du fichier " << argv[1] << " !" << std::endl;
            return 1;
        }
        return tp4(ifichier);
    }else{
        return tp4(std::cin);
    }
    return 0;
}
