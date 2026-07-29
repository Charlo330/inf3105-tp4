/* INF3105 / TP4

   Vos noms et codes parmaments:
    1) Charles-Antoine Lanthier (LANC70040208)
*/
#include <iomanip>
#include "cinema.h"

Salle::Salle() : id(0), nbPlaces(0) {}
Salle::Salle(int id, int nbPlaces) : id(id), nbPlaces(nbPlaces) {}
bool Salle::operator==(const Salle& autre) const {
    return id == autre.id && nbPlaces == autre.nbPlaces;
}

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

static bool tmApres(const std::tm& a, const std::tm& b) {
    if (a.tm_year != b.tm_year) return a.tm_year > b.tm_year;
    if (a.tm_mon  != b.tm_mon)  return a.tm_mon  > b.tm_mon;
    if (a.tm_mday != b.tm_mday) return a.tm_mday > b.tm_mday;
    if (a.tm_hour != b.tm_hour) return a.tm_hour > b.tm_hour;
    return a.tm_min > b.tm_min;
}

static bool projectionConflit(const Projection& a, const Projection& b) {
    if (a.salle.id != b.salle.id) return false;
    std::time_t aDebT = std::mktime(const_cast<std::tm*>(&a.date));
    std::time_t bDebT = std::mktime(const_cast<std::tm*>(&b.date));
    std::time_t aFinT = aDebT + a.film->duree;
    std::time_t bFinT = bDebT + b.film->duree;
    return (aDebT < bFinT) && (bDebT < aFinT);
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
    if (film->nom != autre.film->nom) return film->nom < autre.film->nom;
    return salle.id < autre.salle.id;
}


bool Cinema::ajouterSalle(int id, int nbPlaces){
    Salle salle = Salle(id, nbPlaces);
    if (salles[id] == salle) return false;

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

    Projection nouveauProjection(salleObj, &filmObj, debut);

    // Vérifier les conflits avec les projections existantes
    for (auto it = projections.debut(); it; ++it) {
        if (projectionConflit(projections[it], nouveauProjection)) {
            return false;
        }
    }

    projections.inserer(nouveauProjection);
    filmObj.projections.inserer(nouveauProjection);

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

        if (tmApres(current.date, fin))
            break;

        std::time_t currentTime = std::mktime(const_cast<std::tm*>(&current.date));
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
    Film dummyFilm("", 0);
    Projection dummy(Salle(), &dummyFilm, debut);
    ArbreAVL<Projection>::Iterateur iterateur = projections.rechercherEgalOuSuivant(dummy);

    int totalClients = 0;

    while (iterateur) {
        const Projection& current = projections[iterateur];

        if (tmApres(current.date, fin))
            break;

        totalClients += current.nbReservations;
        ++iterateur;
    }

    return totalClients;
}