/* INF3105 / TP4

   Vos noms et codes parmaments:
    1) Prénom Nom (NOMP01234567)
*/
#include <iomanip>
#include "cinema.h"

Salle::Salle() : id(0), nbPlaces(0) {}
Salle::Salle(int id, int nbPlaces) : id(id), nbPlaces(nbPlaces), projections() {}

Film::Film() : nom(""), duree(0) {}
Film::Film(std::string nom, int duree) : nom(nom), duree(duree), projections() {}

Projection::Projection(Salle salle, Film film, std::tm date) :
  nbReservations(0), salle(salle), film(film), date(date) {}

bool Projection::reserver(int nbPlaces) {
    if (salle.nbPlaces - nbReservations >= nbPlaces) {
        nbReservations += nbPlaces;
        return true;
    }
    return false;
}

bool Projection::operator<(const Projection& autre) const {
    // 1. Comparer les dates
    if (date.tm_year != autre.date.tm_year)
        return date.tm_year < autre.date.tm_year;

    if (date.tm_mon != autre.date.tm_mon)
        return date.tm_mon < autre.date.tm_mon;

    if (date.tm_mday != autre.date.tm_mday)
        return date.tm_mday < autre.date.tm_mday;

    if (date.tm_hour != autre.date.tm_hour)
        return date.tm_hour < autre.date.tm_hour;

    if (date.tm_min != autre.date.tm_min)
        return date.tm_min < autre.date.tm_min;

    return film.nom < autre.film.nom;
}


bool Cinema::ajouterSalle(int id, int nbPlaces){
    Salle salle = Salle(id, nbPlaces);

    salles[id] = salle;
    std::cout << "OK";
    return true;
}

bool Cinema::ajouterFilm(const std::string& nom, int duree){
    Film filmObj = Film(nom, duree);

    films[nom] = filmObj;

    return true;
}

bool Cinema::projeter(const std::string& nom, int salle, const std::tm& debut){
    Salle salleObj = salles[salle];
    Film filmObj = films[nom];

    Projection *projection = new Projection(salleObj, filmObj, debut);

    salleObj.projections.inserer(projection);
    filmObj.projections.inserer(projection);
    projections.inserer(projection);

    return true;
}


ArbreAVL<std::string> Cinema::disponible(const std::tm& debut, const std::tm& fin, int nbplaces) const{
    Projection *projection = new Projection(Salle(), Film(), debut);
    ArbreAVL<Projection*>::Iterateur iterateur = projections.rechercherEgalOuSuivant(projection);

    std::tm finCopie = fin;

    Projection *current;
    ArbreAVL<std::string> filmsDisponibles;

    while (iterateur) {
        current = projections[iterateur];

        if (std::mktime(&finCopie) < std::mktime(&current->date))
            break;

        if ((current->salle.nbPlaces + nbplaces) > current->nbReservations)
            break;

        filmsDisponibles.inserer(current->film.nom);
        ++iterateur;
    }

    delete projection;
    return filmsDisponibles;
}

int Cinema::reserver(const std::string& film, const std::tm& debutmin, int nbplaces, std::tm& datesortie){
    // Vérifier si le film existe
    if (!films.contient(film)) {
        return -1;
    }

    Film& filmObj = films[film];

    // Créer une projection factice pour la recherche
    Projection* projectionRecherche = new Projection(Salle(), Film(), debutmin);

    // Rechercher la première projection >= debutmin dans les projections du film
    ArbreAVL<Projection*>::Iterateur iterateur = filmObj.projections.rechercherEgalOuSuivant(projectionRecherche);

    Projection* current = nullptr;
    while (iterateur) {
        current = filmObj.projections[iterateur];

        // Vérifier s'il y a assez de places disponibles
        int placesDisponibles = current->salle.nbPlaces - current->nbReservations;
        if (placesDisponibles >= nbplaces) {
            // Réserver les places
            current->nbReservations += nbplaces;
            datesortie = current->date;
            int salleId = current->salle.id;
            delete projectionRecherche;
            return salleId;
        }

        ++iterateur;
    }

    delete projectionRecherche;
    return -1;
}

int Cinema::nbclients(const std::tm& debut, const std::tm& fin) const{
    int totalClients = 0;

    // Créer une projection factice pour la recherche à partir de debut
    Projection* projectionRecherche = new Projection(Salle(), Film(), debut);

    // Rechercher la première projection >= debut
    ArbreAVL<Projection*>::Iterateur iterateur = projections.rechercherEgalOuSuivant(projectionRecherche);

    delete projectionRecherche;

    Projection* current;
    std::time_t finTime = std::mktime(const_cast<std::tm*>(&fin));

    while (iterateur) {
        current = projections[iterateur];

        // Si la date de la projection est après fin, on arrête
        std::time_t currentTime = std::mktime(const_cast<std::tm*>(&current->date));
        if (currentTime > finTime)
            break;

        // Ajouter le nombre de réservations de cette projection
        totalClients += current->nbReservations;

        ++iterateur;
    }

    return totalClients;
}