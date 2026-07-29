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

class Film;

class Salle {
  public:
    int id;
    int nbPlaces;
    Salle();
    Salle(int id, int nbPlaces);
    bool operator==(const Salle& autre) const;
};

class Projection {
  public:
    Projection(Salle salle, Film* film, std::tm date);
    int nbReservations;
    Salle salle;
    Film* film;
    std::tm date;
    bool reserver(int nbPlaces);
    bool operator<(const Projection& autre) const;
    bool operator==(const Projection& autre) const;
};

class Film {
  public:
    std::string nom;
    int duree;
    Film();
    Film(std::string nom, int duree);
    ArbreAVL<Projection> projections;
};

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
    ArbreAVL<Projection> projections;
};

#endif // __CINEMA_H__
