#include "environnement.h"
#include "interprete.h"
#include "programme.h"
#include "terrain.h"
#include "robot.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Gestion des erreurs liées au terrain
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
  case ERREUR_POSITION_ROBOT_MANQUANTE :
    printf("Erreur lecture du terrain : position initiale du robot manquante\n");
    exit(1);
  }
}

// Affichage de la position d'erreur dans le programme
void affichage_position_programme(erreur_programme e) {
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

// Gestion des erreurs liées au programme
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

// Vérification de l'événement attendu
int verif_evenement(char e, resultat_inter res){
  switch (e)
  {
  case 'S':
    if (res != SORTIE_ROBOT) {
      printf("Erreur : le robot n'est pas sorti du terrain!!\n");
      return 1;
    }else{
      printf("le robot est sorti du terain!!\n");}
    break;
  case 'O':
    if (res != CRASH_ROBOT) {
      printf("Erreur : le robot n'a pas rencontré un obstacle!!\n");
      return 1;
    }else{
      printf("le robot a rencontré un obstacle!!\n");}
    break;
  case 'P':
    if (res != PLOUF_ROBOT) {
      printf("Erreur : le robot n'est pas tombé dans l'eau!!\n");
      return 1;
    }else{
      printf("le robot est tombé dans l'eau!!\n");}
    break;
  case 'F':
    if (res != ARRET_ROBOT) {
      printf("Erreur : le programme n'est pas encore termine!!\n");
      return 1;
    }else{
      printf("le programme est termine!!\n");}
    break;
  }
  return 0;
}

// Conversion de l'orientation enum en caractère
char orientation_char(Orientation orientation) {
    switch (orientation) {
        case Nord: return 'N';
        case Sud: return 'S';
        case Est: return 'E';
        case Ouest: return 'O';
        default: return '?'; // Gérer une orientation inattendue
    }
}

// Vérification de la position et de l'orientation du robot
int verif_position_orientation(Environnement envt, char orientation_attendue, char evenement, int x_attendu, int y_attendu) {
    if (evenement == 'N' || evenement == 'F') {
        if (envt.r.x != x_attendu || envt.r.y != y_attendu) {
            printf("Erreur : Position incorrecte du robot. Attendue : (%d, %d), Obtenu : (%d, %d)\n",
                   x_attendu, y_attendu, envt.r.x, envt.r.y);
            return 1; // Position incorrecte
        }

        Orientation orientation_attendue_enum;
        switch (orientation_attendue) {
            case 'N': orientation_attendue_enum = Nord; break;
            case 'S': orientation_attendue_enum = Sud; break;
            case 'E': orientation_attendue_enum = Est; break;
            case 'O': orientation_attendue_enum = Ouest; break;
            default: orientation_attendue_enum = Nord; // Par défaut, on considère le Nord si l'orientation est inattendue
        }

        if (envt.r.o != orientation_attendue_enum) {
            printf("Erreur : Orientation incorrecte du robot. Attendue : %c, Obtenu : %c\n",
                   orientation_attendue, orientation_char(envt.r.o));
            return 1; // Orientation incorrecte
        }
    }
    return 0; // Position et orientation correctes
}



int main(int argc, char **argv) {
  Environnement envt;
  Programme prog;
  erreur_terrain errt;
  erreur_programme errp;
  etat_inter etat;
  resultat_inter res;

  FILE *fich_test;
  char terr[100];
  unsigned int timeout;
  char pro[100];
  char e;
  int x, y;
  char o; // Orientation courante du robot

  if (argc != 2) {
    printf("Usage : %s <nom_fichier>\n", argv[0]);
    return -1;
  }

  
  fich_test = fopen(argv[1], "r");
  if (fich_test==NULL){
    printf("Erreur : Impossible d'ouvrir le fichier %s\n", argv[1]);
    return -1;
  }
  
  fscanf(fich_test, "%s\n", terr); //lecture du fichier contenant le terrain
  fscanf(fich_test, "%s\n", pro); //lecture du fichier contenant le programme
  fscanf(fich_test, "%d\n", &timeout); //lecture de timeout
  fscanf(fich_test, "%c\n", &e); //lecture de l'evenement final S N F O P
  if (e == 'N' || e == 'F'){
    fscanf(fich_test, "%d %d\n", &x, &y); //lecture des coordonnées x y attendus du robot
    fscanf(fich_test, "%c", &o); //lecture de l'orientation de robot N S E O
  }
  fclose(fich_test);

  //Initialisation de l'environnement : lecture du terrain, initialisation de la position du robot
  errt = initialise_environnement(&envt, terr);
  gestion_erreur_terrain(errt);

  //Lecture du programme
  errp = lire_programme(&prog, pro);
  gestion_erreur_programme(errp);

  //Initialisation de l'état
  init_etat(&etat);
  int i=0;
  do {
    i++;
    res = exec_pas(&prog, &envt, &etat);
    //Affichage du terrain et du robot
    afficher_envt(&envt);
  } while (res == OK_ROBOT && i < timeout);

  printf("l'abssice est: %d\n", envt.r.x);
  printf("l'ordonnée est: %d\n", envt.r.y);
  printf("l'orientation est: %c\n", orientation_char(envt.r.o));

  //verification de l'evenement final attendu du robot
  verif_evenement(e, res);

  //verification de la position et l'orientation du robot
  if (e == 'N' || e == 'F'){
    verif_position_orientation(envt, o, e, x, y);
  }
  
  //Si tout fonctionne bien, afficher "OK"
  if (  (0 == verif_evenement(e, res)) && (0 == verif_position_orientation(envt, o, e, x, y))   ) {
    printf("OK.\n");
  }
 
 return 0;
}