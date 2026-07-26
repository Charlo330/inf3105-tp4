/* INF3105 / TP4

   Vos noms et codes parmaments:
    1) Charles-Antoine Lanthier (LANC70040208)
*/
#if !defined __CINEMA_H__
#define __CINEMA_H__

#include <iostream>
#include <string>
#include <ctime>

#include "arbreavl.h"
#include "arbremap.h"

// Forward declaration for Projection pointer usage in Salle and Film
class Projection;

class Salle {
  public:
    int id;
    int nbPlaces;
    Salle();
    Salle(int id, int nbPlaces);
    ArbreAVL<Projection*> projections;

};

Salle::Salle() : id(0), nbPlaces(0) {}
Salle::Salle(int id, int nbPlaces) : id(id), nbPlaces(nbPlaces), projections() {}

class Film {
  public:
    std::string nom;
    int duree;
    Film();
    Film(std::string nom, int duree);
    ArbreAVL<Projection*> projections;
};

Film::Film() : nom(""), duree(0) {}
Film::Film(std::string nom, int duree) : nom(nom), duree(duree), projections() {}

class Projection {
  public:
    Projection(Salle salle, Film film, std::tm date);
    int nbReservations;
    Salle salle;
    Film film;
    std::tm date;
    bool reserver(int nbPlaces);
    bool operator<(const Projection& autre) const;
};

Projection::Projection(Salle salle, Film film, std::tm date) : 
  nbReservations(0), salle(salle), film(film), date(date) {}

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


class Cinema{
  public:
    bool ajouterSalle(int id, int nbPlaces);
    bool ajouterFilm(const std::string& nom, int duree);
    bool projeter(const std::string& nom, int salle, const std::tm& debut);
    ArbreAVL<std::string> disponible(const std::tm& debut, const std::tm& fin, int nbplaces) const;
    int reserver(const std::string& film, const std::tm& debut, int nbPlaces, std::tm& datesortie);
    int nbclients(const std::tm& debut, const std::tm& fin) const;
  
  private:
    ArbreMap<int, Salle> salles;
    ArbreMap<std::string, Film> films;
    ArbreAVL<Projection*> projections;
};

#endif // __CINEMA_H__
