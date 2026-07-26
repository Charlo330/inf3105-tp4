#!/bin/bash
###############################################################################
# UQAM - Département d'informatique
# INF3105 - Structures de données et algorithmes
# TP4 - Gestion d'un complexe cinématographique
# http://ericbeaudry.uqam.ca/INF3105/tp4/
#
# Script d'évaluation
#
# Instructions:
# 1. Déposer ce script avec les fichiers tests dans un répertoire
#    distinct (ex: tests).
# 2. Se placer dans le répertoire contenant votre programme `tp4`.
#    Le correcteur se placera dans le répertoire contenant les soumissions.
# 3. Lancer ce script (ex: ../tests/evaluer.sh).
##########################################################################

# Obtenir le chemin du répertoire contenant le présent script et les fichiers tests
pushd `dirname $0` > /dev/null
repertoire_tests=`pwd`
# Lister les fichier test disponibles
# tests=`ls exemple[0-9].txt`
tests=`ls exemple[0-9].txt test[0-9][0-9].txt`
popd  > /dev/null

echo "UQAM | Département d'informatique"
echo "INF3105 | Structures de données et algorithmes"
echo "Évaluation du TP4"
echo

if [ `pwd` -ef $repertoire_tests ];
then
    echo "Ce script doit être dans un répertoire différent de celui contenant votre programme tp4."
    echo "Ce script a été arrêté afin de ne pas écraser les fichiers test*+.txt !"
    exit -2;
fi

########### Détection du valideur de résultats #######
valideur="${repertoire_tests}/valideur"
if [ ${repertoire_tests}/valideur.cpp -nt ${valideur} ]
then
    echo "Compilation du valideur..."
    g++ -O3 -o ${valideur} ${repertoire_tests}/valideur.cpp
fi
######################################################

########## Détection si l'utilitaire time sous Linux est disponible (peut afficher 0.00)
if [ -x /usr/bin/time ]; then
  #echo "Détection de /usr/bin/time..."
  /usr/bin/time -f %U echo 2> /dev/null > /dev/null
  utilitaire_time=$?
else
  utilitaire_time="-1"
fi

# Fixer les limites : 300s, 4096Mo mémoire et 100Mo fichier
ulimit -t 300 -v 4194304 -f 102400
#echo "ulimit (limites courantes des ressources) :"
#ulimit -t -v -f
#echo "-----"

# Détection du CPU
if [ -e /proc/cpuinfo ] ; then
    cpuinfo=`grep "model name" /proc/cpuinfo | sort -u | cut -d ":" -f 2`
else
    cpuinfo="?"
fi

function Nettoyer
{
    echo "Nettoyage"
    # Au cas où le Makefile des étudiants ne ferait pas un «make clean» correctement.
    #make clean

    rm -f *.o* *.gch tp[1-5] *+.txt
    # Au besoin, nettoyer les précédents fichiers logs
    rm -f log*.txt
}


##### Fonction d'évaluation d'un TP ######
function EvaluerTP
{
    date2=`date +%Y%m%d_%H%M%S`
    datesoumis=`grep Date lisezmoi.txt| cut -c 7-`
    coequipier=`grep Coéquipier lisezmoi.txt| cut -d : -f 2`
    heures=`grep Heures lisezmoi.txt| cut -d : -f 2`
    autoeval=`grep Auto-Évaluation lisezmoi.txt| cut -d : -f 2`

    sommaire="$datesoumis\t$coequipier\t$autoeval\t$heures"

    #log="log-${date2}.txt"
    log="log.txt"
    echo "Les résultats seront déposés dans ${log}..."

    echo "Rapport TP4" | tee $log
    echo "Date: $date2" | tee -a $log
    echo "Date remise: $datesoumis" | tee -a $log
    echo "Soumission: $tp" | tee -a $log
    echo "Coéquipier: $coequipier" | tee -a $log
    echo "#Machine : " `hostname`  | tee -a $log
    echo "#CPU :$cpuinfo"  | tee -a $log
    echo "#Limite de `ulimit -t` secondes par test"  | tee -a $log
    #echo "Fichiers:" | tee -a $log
    #ls -l 2>&1 | tee -a $log
    #echo "Taille des fichiers source:" | tee -a ${log}
    #wc *.{c*,h*}  | tee -a ${log}
    #echo "" | tee -a $log



    ## ZIP
    #if [ -f sources.zip ]; then
    #    echo "Unzip: sources.zip"
    #    unzip -n sources.zip
    #    rm sources.zip
    #fi
    
    ## Forcer la recompilation.
    #rm -f tp4 *.o *.gch
    #make clean
    #echo "Compilation ..." | tee -a $log
    #make tp4 2>&1 | tee -a $log

    # Vérification de l'existance du programme.
    if [ ! -e tp4 ]
        then
        echo "  Erreur : le fichier tp4 est inexistant!" | tee -a $log
        return
    fi
    if [ ! -x tp4 ]
        then
        echo "  Erreur : le fichier tp4 n'est pas exécutable!" | tee -a $log
        return
    fi

    echo | tee -a $log

    echo -e "Fichier test\tTotal\tMém.(k)\tProje\tReser\tDispo\tNbCli\ttProje\tReser\tDispo\tNbCli" | tee -a $log

    for test in $tests;
    do
      fichier=$repertoire_tests/$test

      if [ $utilitaire_time -eq 0 ]; then
        tTout=$( (/usr/bin/time -f "%U\t%Mk" ./tp4 < $fichier > ${test%.txt}+.txt) 2>&1 )
        tp4_exit_code="$?"
        tTout=$(echo "${tTout}" | tail -n 1)
        if [ ${tp4_exit_code} -eq 137 ]; then tTout=">${tTout}"; fi
        tProj=$( egrep "^Ajouter|^Projeter" $fichier | (/usr/bin/time -f "%U" ./tp4 > /dev/null) 2>&1)
        tp4_exit_code="$?"
        tProj=$(echo "${tProj}" | tail -n 1)
        if [ ${tp4_exit_code} -eq 137 ]; then tProj=">${tProj}"; fi
        tRese=$( egrep "^Ajouter|^Projeter|^Reserver" $fichier | (/usr/bin/time -f "%U" ./tp4 > /dev/null) 2>&1)
        tp4_exit_code="$?"
        tRese=$(echo "${tRese}" | tail -n 1)
        if [ ${tp4_exit_code} -eq 137 ]; then tRese=">${tRese}"; fi
        tDisp=$( egrep "^Ajouter|^Projeter|^Reserver|^Disponible" $fichier | (/usr/bin/time -f "%U" ./tp4 > /dev/null) 2>&1)
        tp4_exit_code="$?"
        tDisp=$(echo "${tDisp}" | tail -n 1)
        if [ ${tp4_exit_code} -eq 137 ]; then tDisp=">${tDisp}"; fi
        tNbCl=$( egrep "^Ajouter|^Projeter|^Reserver|^NbClients" $fichier | (/usr/bin/time -f "%U" ./tp4 > /dev/null) 2>&1)
        tp4_exit_code="$?"
        tNbCl=$(echo "${tNbCl}" | tail -n 1)
        if [ ${tp4_exit_code} -eq 137 ]; then tNbCl=">${tNbCl}"; fi
      else
        # À écrire...
        tTout=`                                          (time -p ./tp4 < $fichier > ${test%.txt}+.txt) 2>&1 | egrep user | cut -f 2 -d " "`
        tProj=`egrep "^Ajouter|^Projeter"                       $fichier | (time -p ./tp4 > /dev/null) 2>&1 | egrep user | cut -f 2 -d " "`
        tRese=`egrep "^Ajouter|^Projeter|^Reserver"             $fichier | (time -p ./tp4 > /dev/null) 2>&1 | egrep user | cut -f 2 -d " "`
        tDisp=`egrep "^Ajouter|^Projeter|^Reserver|^Disponible" $fichier | (time -p ./tp4 > /dev/null) 2>&1 | egrep user | cut -f 2 -d " "`
        tNbCl=`egrep "^Ajouter|^Projeter|^Reserver|^NbClients"  $fichier | (time -p ./tp4 > /dev/null) 2>&1 | egrep user | cut -f 2 -d " "`
      fi

      if ( [ -x ${valideur} ] && [ -e "${fichier%.txt}+.txt" ] )
      then
          verif=`$valideur -q -c $fichier "${fichier%.txt}+.txt" ${test%.txt}+.txt`
      else
          verif="??"
      fi

      echo -e "$test\t${tTout}\t${tProj}\t${tRese}\t${tDisp}\t${tNbCl}\t${verif}" | tee -a $log
      sommaire="${sommaire}\t${tTout}\t${tProj}\t${tRese}\t${tDisp}\t${tNbCl}\t${verif}\t"

    done

    #rm -f tp4 *.o *.gch
    #make clean

}


if [ -f tp4 ] || [ -f tp4.cpp ] || [ -f Makefile ]
then
    echo "Mode étudiant"
    EvaluerTP
    exit
else
    echo "Mode correcteur"
    lestps=`ls -1`
    tps=`for tp in ${lestps}; do echo $x; if [ -f ${tp}/tp4.cpp ]; then echo ${tp}; fi; done`
fi

#Suite pour correction seulement

date=`date +%Y%m%d_%H%M%S`
echo "#Rapport de correction INF3105 / TP4" > "rapport-${date}.txt"
echo -e "#Date:\t${date}" >> "rapport-${date}.txt"
echo -e "#Machine :\t" `hostname` >> "rapport-${date}.txt"
echo -e "#CPU :\t$cpuinfo" >> "rapport-${date}.txt"
echo >> "rapport-${date}.txt"

echo -e -n "\t\t\t\t" >> "rapport-${date}.txt"
for test in ${tests}; do
    echo -e -n "\t${test}\t\t\t\t\t\t\t\t\t\t" >> "rapport-${date}.txt"
done
echo >> "rapport-${date}.txt"
echo -e -n "Soumission\tDate\tCoéquipier\tAuto-Éval\tHeures\t" >> "rapport-${date}.txt"
for test in ${tests}; do
    echo -e -n "CPU\tMém.(k)\tProje\tReser\tDispo\tNbCli\ttProje\tReser\tDispo\tNbCli\t\t" >> "rapport-${date}.txt"
done
echo >> "rapport-${date}.txt"
echo

for tp in $tps;
do
    sommaire=""
    echo "------------------------------------------"
    echo "Correction de: $tp ..."

    # changer de répertoire pour celui de la soumission
    pushd $tp

    EvaluerTP

    # revenir dans le répertoire précédent.
    echo
    popd

    # Ajout d'une ligne au rapport...
    echo -e "${tp}\t${sommaire}" >> "rapport-${date}.txt"
done
