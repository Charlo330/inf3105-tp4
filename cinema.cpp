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
    return true;
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
    datesortie = std::tm{59, 59, 23, 8, 11, 123, 4, -1};
    return -1;
}

int Cinema::nbclients(const std::tm& debut, const std::tm& fin) const{
    return 0;
}