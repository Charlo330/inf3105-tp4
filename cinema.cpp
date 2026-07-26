/* INF3105 / TP4

   Vos noms et codes parmaments:
    1) Charles-Antoine Lanthier (LANC70040208)
*/
#include <iomanip>
#include "cinema.h"

Salle::Salle() : id(0), nbPlaces(0) {}
Salle::Salle(int id, int nbPlaces) : id(id), nbPlaces(nbPlaces) {}

Film::Film() : nom(""), duree(0) {}
Film::Film(std::string nom, int duree) : nom(nom), duree(duree) {}

Projection::Projection(Salle salle, Film* film, std::tm date) :
  nbReservations(0), salle(salle), film(film), date(date) {}

bool Projection::reserver(int nbPlaces) {
    if (salle.nbPlaces - nbReservations >= nbPlaces) {
        nbReservations += nbPlaces;
        return true;
    }
    return false;
}

bool Projection::operator<(const Projection& autre) const {
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
    if (film == nullptr && autre.film == nullptr) return false;
    if (film == nullptr) return true;
    if (autre.film == nullptr) return false;
    return film->nom < autre.film->nom;
}


bool Cinema::ajouterSalle(int id, int nbPlaces){
    Salle salle = Salle(id, nbPlaces);
    salles[id] = salle;
    return true;
}

bool Cinema::ajouterFilm(const std::string& nom, int duree){
    Film filmObj = Film(nom, duree);
    films[nom] = filmObj;
    return true;
}

bool Cinema::projeter(const std::string& nom, int salle, const std::tm& debut){
    Salle& salleObj = salles[salle];
    Film& filmObj = films[nom];

    Projection projection(salleObj, &filmObj, debut);

    projections.inserer(projection);
    filmObj.projections.inserer(projection);

    return true;
}


ArbreAVL<std::string> Cinema::disponible(const std::tm& debut, const std::tm& fin, int nbplaces) const{
    Film dummyFilm("", 0);
    Projection dummy(Salle(), &dummyFilm, debut);
    ArbreAVL<Projection>::Iterateur iterateur = projections.rechercherEgalOuSuivant(dummy);

    std::time_t finTime = std::mktime(const_cast<std::tm*>(&fin));

    ArbreAVL<std::string> filmsDisponibles;

    while (iterateur) {
        const Projection& current = projections[iterateur];
        std::time_t currentTime = std::mktime(const_cast<std::tm*>(&current.date));

        if (currentTime > finTime)
            break;

        std::time_t finProjTime = currentTime + current.film->duree;
        if (finProjTime <= finTime) {
            int placesLibres = current.salle.nbPlaces - current.nbReservations;
            if (placesLibres >= nbplaces)
                filmsDisponibles.inserer(current.film->nom);
        }

        ++iterateur;
    }

    return filmsDisponibles;
}

int Cinema::reserver(const std::string& film, const std::tm& debutmin, int nbplaces, std::tm& datesortie){
    if (!films.contient(film)) {
        return -1;
    }

    Film& filmObj = films[film];

    Projection dummy(Salle(), &filmObj, debutmin);
    ArbreAVL<Projection>::Iterateur it = filmObj.projections.rechercherEgalOuSuivant(dummy);

    Projection* trouve = nullptr;
    while (it) {
        Projection& current = filmObj.projections[it];
        int placesDisponibles = current.salle.nbPlaces - current.nbReservations;
        if (placesDisponibles >= nbplaces) {
            trouve = &current;
            break;
        }
        ++it;
    }

    if (trouve == nullptr) {
        return -1;
    }

    trouve->nbReservations += nbplaces;
    datesortie = trouve->date;
    int salleId = trouve->salle.id;

    Projection dummyGlobal(Salle(), &filmObj, trouve->date);
    ArbreAVL<Projection>::Iterateur itGlobal = projections.rechercherEgalOuSuivant(dummyGlobal);
    if (itGlobal) {
        Projection& g = projections[itGlobal];
        if (!(dummyGlobal < g) && !(g < dummyGlobal)) {
            g.nbReservations += nbplaces;
        }
    }

    return salleId;
}

int Cinema::nbclients(const std::tm& debut, const std::tm& fin) const{
    std::time_t finTime = std::mktime(const_cast<std::tm*>(&fin));

    Film dummyFilm("", 0);
    Projection dummy(Salle(), &dummyFilm, debut);
    ArbreAVL<Projection>::Iterateur iterateur = projections.rechercherEgalOuSuivant(dummy);

    int totalClients = 0;

    while (iterateur) {
        const Projection& current = projections[iterateur];
        std::time_t currentTime = std::mktime(const_cast<std::tm*>(&current.date));

        if (currentTime > finTime)
            break;

        totalClients += current.nbReservations;
        ++iterateur;
    }

    return totalClients;
}
