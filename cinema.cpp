/* INF3105 / TP4
   
   Vos noms et codes parmaments:
    1) Prénom Nom (NOMP01234567) 
*/
#include <iomanip>
#include "cinema.h"

bool Cinema::ajouterSalle(int id, int nbPlaces){
    Salle salle = Salle(id, nbPlaces);

    salles[id] = salle;
    std::cout << "OK";
    //std::cerr << "Ajout de la salle " << id << " avec " << nbPlaces << " places" << std::endl;
    return true;
}

bool Cinema::ajouterFilm(const std::string& film, int duree){
    Film filmObj = Film(film, duree);

    films[film] = filmObj;

    return true;
    //std::cerr << "Ajout du film " << film << " d'une durée de " << duree << " minutes" << std::endl;
}

bool Cinema::projeter(const std::string& film, int salle, const std::tm& debut){
    Salle salleObj = salles[salle];
    Film filmObj = films[film];

    Projection *projection = new Projection(salleObj, filmObj, debut);

    salleObj.projections.inserer(projection);
    filmObj.projections.inserer(projection);
    projections.inserer(projection);

    return true;
    //std::cerr << "Projection du film " << film << " dans la salle " << salle << " à " << std::put_time(&debut, "%Y-%m-%d %H:%M") << std::endl;
}


ArbreAVL<std::string> Cinema::disponible(const std::tm& debut, const std::tm& fin, int nbplaces) const{
    Projection *projection = new Projection(Salle(), Film(), debut);
    ArbreAVL<Projection*>::Iterateur iterateur = projections.rechercherEgalOuSuivant(projection);

    std::tm finCopie = fin;

    Projection *current;
    ArbreAVL<std::string> filmsDisponibles;
    
    while (iterateur++) {
        current = projections[iterateur];

        if (std::mktime(&finCopie) < std::mktime(&current->date))
        break;
        
        if ((current->salle.nbPlaces + nbplaces) > current->nbReservations)
        break;

        filmsDisponibles.inserer(current->film.nom);
    }

    delete projection;
    return filmsDisponibles;
    //std::cerr << "Disponibilité de " << nbplaces << " places entre " << std::put_time(&debut, "%Y-%m-%d %H:%M") << " et " << std::put_time(&fin, "%Y-%m-%d %H:%M") << std::endl;
}

int Cinema::reserver(const std::string& film, const std::tm& debutmin, int nbplaces, std::tm& datesortie){
    //std::cerr << "Réservation de " << nbplaces << " places pour le film " << film << " à " << std::put_time(&debutmin, "%Y-%m-%d %H:%M") << std::endl;
    datesortie = std::tm{59, 59, 23, 8, 11, 123, 4, -1};
    return -1;
}

int Cinema::nbclients(const std::tm& debut, const std::tm& fin) const{
    //std::cerr << "Nombre de clients entre " << std::put_time(&debut, "%Y-%m-%d %H:%M") << " et " << std::put_time(&fin, "%Y-%m-%d %H:%M") << std::endl;
    return 0;
}
