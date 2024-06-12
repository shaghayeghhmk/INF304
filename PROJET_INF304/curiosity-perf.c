#include "environnement.h"
#include "interprete.h"
#include "programme.h"
#include "generation_terrains.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Fonction pour gérer les erreurs liées au terrain
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

// Fonction pour afficher la position d'une erreur dans le programme
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

// Fonction pour gérer les erreurs liées au programme
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
  erreur_programme errp;
  etat_inter etat;
  resultat_inter res;
  FILE *resFile;
  Terrain T;

  int N, L, H, graine, nb_step_max;
  float d; 
  char *fichier_programme, *fichier_res;

  int sorti = 0, s, bloque = 0, tombe = 0, ecrase = 0, sum_pas = 0;

  if (argc < 9) {
    printf("Usage: %s <fichier_programme> <N> <L> <H> <d> <graine> <nb_step_max> <fichier_res>\n", argv[0]);
    return 1;
  }

  fichier_programme = argv[1];
  N = strtol(argv[2], NULL, 10);
  L = strtol(argv[3], NULL, 10);
  H = strtol(argv[4], NULL, 10);
  d = strtof(argv[5], NULL);
  graine = strtol(argv[6], NULL, 10);
  nb_step_max = strtol(argv[7], NULL, 10);
  fichier_res = argv[8];

  // test de l et h
  if (L > DIM_MAX || L % 2 == 0) {
    printf("Largeur incorrecte : doit être impaire et <= %d\n", DIM_MAX);
    return 1;
  }
  if (H > DIM_MAX || H % 2 == 0) {
    printf("Hauteur incorrecte : doit être impaire et <= %d\n", DIM_MAX);
    return 1;
  }
  if ((d> 1) || (d < 0)) {
    printf("Densité incorrecte : doit être comprise entre 0 et 1\n");
    return 1;
  }

  // Initialisation de la fonction random
  srand(graine);

  // Ouverture du fichier résultat
  resFile = fopen(fichier_res,"w");

  // Écriture du nombre de terrains
  fprintf(resFile, "%d\n", N);


  for (int i = 0; i < N; i++)
  {
    envt.r.o = Est;
    envt.r.x = L / 2;
    envt.r.y = H / 2;
    generation_aleatoire(&T, L, H, d);
    while (!existe_chemin_vers_sortie(&T)){
      generation_aleatoire(&T, L, H, d);}
    envt.t = T;


    /* Lecture du programme */
    errp = lire_programme(&prog, fichier_programme);
    gestion_erreur_programme(errp);

    /* Initialisation de l'état */
    init_etat(&etat);
    s=0;
    do {
      s++;
      res = exec_pas(&prog, &envt, &etat);
      /* Affichage du terrain et du robot */
      afficher_envt(&envt);
    } while (res == OK_ROBOT && s < nb_step_max);

    printf("nb de pas fait : %d\n", s);

    /* Affichage du résultat */
    switch (res) {
    case OK_ROBOT:
      printf("Robot sur une case libre, programme non terminé (ne devrait pas "
             "arriver)\n");
      fprintf(resFile, "-1\n");
      bloque++;
      break;
    case SORTIE_ROBOT:
      printf("Le robot est sorti :-)\n");
      fprintf(resFile, "%d\n", s);
      sorti++;
      sum_pas = sum_pas + s;
      break;
    case ARRET_ROBOT:
      printf("Robot sur une case libre, programme terminé :-/\n");
      fprintf(resFile, "-1\n");
      bloque++;
      break;
    case PLOUF_ROBOT:
      printf("Le robot est tombé dans l'eau :-(\n");
      fprintf(resFile, "-2\n");
      tombe++;
      break;
    case CRASH_ROBOT:
      printf("Le robot s'est écrasé sur un rocher X-(\n");
      fprintf(resFile, "-3\n");
      ecrase++;
      break;
    case ERREUR_PILE_VIDE:
      printf("ERREUR : pile vide\n");
      fprintf(resFile, "none\n");
      break;
    case ERREUR_ADRESSAGE:
      printf("ERREUR : erreur d'adressage\n");
      fprintf(resFile, "none\n");
      break;
    case ERREUR_DIVISION_PAR_ZERO:
      printf("ERREUR : division par 0\n");
      fprintf(resFile, "none\n");
      break;
    }
    sleep(1);
  }
  printf("\n\n");
  printf("            RÉSULTATS STATISTIQUES            \n");
  printf("Nombre total de terrains générés et évalués : %d\n\n", N);
  printf("Nombre de terrains d'où le robot a réussi à sortir : %d\n", sorti);
  printf("Pourcentage de succès de sortie : %.2f%%\n\n", (float) sorti / N * 100);
  printf("Nombre de terrains où le robot a rencontré des obstacles (eau + rocher) : %d\n", ecrase + tombe);
  printf("Pourcentage de terrains avec obstacles rencontrés : %.2f%%\n\n", (float)(ecrase + tombe) / N * 100);
  printf("Nombre de terrains où le robot est resté bloqué : %d\n", bloque);
  printf("Pourcentage de blocages : %.2f%%\n\n", (float)bloque / N * 100);
  printf("Nombre moyen de pas effectués pour les sorties réussies : %.2f\n", (float)sum_pas / sorti);
  printf("Somme totale des pas effectués sur tous les terrains : %d\n", sum_pas);

  fclose(resFile);
  return 0;
}
