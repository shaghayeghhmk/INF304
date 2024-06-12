#include "type_pile.h"
#include "environnement.h"
#include "interprete.h"
#include "programme.h"
#include "terrain.h"
#include "robot.h"
#include "observateur.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void gestion_erreur_terrain(erreur_terrain e) {
  switch (e) {
  case OK:
    break;
  case ERREUR_FICHIER:
    printf("Erreur lecture du terrain : erreur d'ouverture du fichier\n");
    exit(1);
  case ERREUR_LECTURE_LARGEUR:
    printf("Erreur lecture du terrain : erreur de lecture de la largeur\n");
    exit(1);
  case ERREUR_LECTURE_HAUTEUR:
    printf("Erreur lecture du terrain : erreur de lecture de la hauteur\n");
    exit(1);
  case ERREUR_LARGEUR_INCORRECTE:
    printf("Erreur lecture du terrain : largeur incorrecte\n");
    exit(1);
  case ERREUR_HAUTEUR_INCORRECTE:
    printf("Erreur lecture du terrain : hauteur incorrecte\n");
    exit(1);
  case ERREUR_CARACTERE_INCORRECT:
    printf("Erreur lecture du terrain : caractère incorrect\n");
    exit(1);
  case ERREUR_LIGNE_TROP_LONGUE:
    printf("Erreur lecture du terrain : ligne trop longue\n");
    exit(1);
  case ERREUR_LIGNE_TROP_COURTE:
    printf("Erreur lecture du terrain : ligne trop courte\n");
    exit(1);
  case ERREUR_LIGNES_MANQUANTES:
    printf("Erreur lecture du terrain : lignes manquantes\n");
    exit(1);
  case ERREUR_POSITION_ROBOT_MANQUANTE:
    printf("Erreur lecture du terrain : position initiale du robot manquante\n");
    exit(1);
  }
}

void affichage_position_programme(erreur_programme e) {
  /* Fonction qui affiche la position exacte de l'erreur dans un programme */
  int i;
  printf("Ligne %d, colonne %d :\n", e.num_ligne, e.num_colonne);
  printf("%s\n", e.ligne);
  /* Impression de e.num_colonne-1 espaces */
  for (i = 1; i < e.num_colonne; i++) {
    printf(" ");
  }
  /* Impression d'un curseur de position */
  printf("^\n");
}

void gestion_erreur_programme(erreur_programme e) {
  switch (e.type_err) {
  case OK_PROGRAMME:
    break;
  case ERREUR_FICHIER_PROGRAMME:
    printf("Erreur lecture du programme : erreur d'ouverture du fichier\n");
    exit(2);
  case ERREUR_BLOC_NON_FERME:
    printf("Erreur lecture du programme : bloc non fermé\n");
    exit(2);
  case ERREUR_FERMETURE_BLOC_EXCEDENTAIRE:
    printf("Erreur lecture du programme : fermeture de bloc excédentaire\n");
    affichage_position_programme(e);
    exit(2);
  case ERREUR_COMMANDE_INCORRECTE:
    printf("Erreur lecture du programme : commande incorrecte\n");
    affichage_position_programme(e);
    exit(2);
  }
}

int main(int argc, char **argv) {
    Environnement envt;
    Programme prog;
    erreur_terrain errt;
    erreur_programme errp;
    etat_inter etat;
    resultat_inter res;

    // Vérifie le nombre d'arguments
    if (argc < 3) {
        printf("Usage: %s <terrain> <programme>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Initialisation de l'environnement
    errt = initialise_environnement(&envt, argv[1]);
    gestion_erreur_terrain(errt);

    // Lecture du programme
    errp = lire_programme(&prog, argv[2]);
    gestion_erreur_programme(errp);

    // Initialisation de l'état
    envt.e = INITIAL;

    // Affichage du terrain et du robot
    afficher_envt(&envt);

    do {
        // Exécution d'une étape du programme
        res = exec_pas(&prog, &envt, &etat);

        // Affichage du terrain et du robot
        afficher_envt(&envt);

        // Affichage de l'état courant
        printf("L'état courant est : %d\n", envt.e);
        printf("Le résultat est: %d\n", res);
    } while (res == OK_ROBOT);

    // Vérification de la propriété
    if (est_accepteur(envt.e)) {
        printf("La propriété est valide!\n");
    } else {
        printf("La propriété est non valide!\n");
    }

    return 0;
}
