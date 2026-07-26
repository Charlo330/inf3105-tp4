# Makefile pour TP4.
# Adaptez ce fichier au besoin.

# Options standard.
#OPTIONS = -Wall

# L'option -g permet de générer les infos de débogage.
# Décommentez la ligne suivante si vous désirez utiliser un débogueur.
#OPTIONS = -g -O0 -Wall

# Les options -O, -O1, -O2, -O3 permetent d'optimiser le code binaire produit.
# Décommentez la ligne suivante si avant la remise finale
OPTIONS = -O2 -Wall


# Syntaxe : cible : dépendance1 dépendance2 ...
# Ensuite, la ou les ligne(s) débutant par une tabulation (\t) donne les commandes pour construire une cible
tp4: tp4.cpp cinema.o
	g++ $(OPTIONS) -o tp4 tp4.cpp cinema.o

cinema.o: cinema.h cinema.cpp
	g++ $(OPTIONS) -c -o cinema.o cinema.cpp
	
clean :
	rm -f *.o
	rm -f tp4
	rm -f *~

