/*
  INF3105 - Structures de données et algorithmes
  UQAM / Département d'informatique
  Éric Beaudry -- http://ericbeaudry.uqam.ca/INF3105 -- beaudry.eric@uqam.ca

  Valideur pour le TP4

  Pour compiler :
    g++ -o valideur valideur.cpp

  Pour utiliser :
    ./valideur testXX.txt testXX+.txt votrefichier.txt
*/

#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

using namespace std;

string enleverEspaces(const string& chaine) {
  string resultat = chaine;
  int j = 0;
  for (unsigned int i = 0; i < chaine.size(); i++) {
    if (chaine[i] != ' ' && chaine[i] != '\t') resultat[j++] = chaine[i];
  }
  resultat[j++] = 0;
  return resultat;
}

int main(int argc, const char** argv) {
  string nomfichier_test, nomfichier_resultat1, nomfichier_resultat2;
  bool silence = false;
  bool correction = false;
  for (int i = 1, j = 0; i < argc; i++) {
    if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "-quiet") == 0
        || strcmp(argv[i], "-silence") == 0)
      silence = true;
    else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "-correction") == 0)
      correction = true;
    else
      switch (j++) {
        case 0:
          nomfichier_test = argv[i];
          break;
        case 1:
          nomfichier_resultat1 = argv[i];
          break;
        case 2:
          nomfichier_resultat2 = argv[i];
          break;
      }
  }
  if (nomfichier_resultat2 == "") {
    cerr << "Valideur pour INF3105/2023A/TP4" << endl;
    cerr << "Syntaxe : ./valideur testXX.txt testXX+.txt votrefichierXX+.txt"
         << endl;
    return 1;
  }

  ifstream ifs_test(nomfichier_test.c_str());
  ifstream ifs_solution(nomfichier_resultat1.c_str());
  ifstream ifs_resultat(nomfichier_resultat2.c_str());

  if (ifs_test.fail()) {
    cerr << "Erreur ouverture " << nomfichier_test << endl;
    return 1;
  }
  if (ifs_solution.fail()) {
    cerr << "Erreur ouverture " << nomfichier_resultat1 << endl;
    return 1;
  }
  if (ifs_resultat.fail()) {
    cerr << "Erreur ouverture " << nomfichier_resultat2 << endl;
    return 1;
  }

  map<string, int> nbtests;
  map<string, int> nbreussi;

  int ligne = 0;
  while (ifs_test) {
    ligne++;
    string lignetest, lignesolution, ligneresultat;

    getline(ifs_test, lignetest);
    getline(ifs_solution, lignesolution);
    getline(ifs_resultat, ligneresultat);

    stringstream ss(lignetest);
    string commande;
    ss >> commande;

    if (commande.empty()) break;

    if (enleverEspaces(lignesolution) == enleverEspaces(ligneresultat))
      nbreussi[commande]++;
    else {
      if (!silence) {
        cerr << "Ligne #" << ligne << " (" << commande << ") : ÉCHEC" << endl;
        cerr << " V1:" << lignesolution << endl;
        cerr << " V2:" << ligneresultat << endl;
      }
    }

    nbtests[commande]++;
  }

  cout << nbreussi["Projeter"];
  if (!correction) cout << '/' << nbtests["Projeter"];
  cout << '\t' << nbreussi["Disponible"];
  if (!correction) cout << '/' << nbtests["Disponible"];
  cout << '\t' << nbreussi["Reserver"];
  if (!correction) cout << '/' << nbtests["Reserver"];
  cout << '\t' << nbreussi["NbClients"];
  if (!correction) cout << '/' << nbtests["NbClients"];
  /* if (ok) cout << "\tOK"; */
  cout << endl;

  return 0;
}
