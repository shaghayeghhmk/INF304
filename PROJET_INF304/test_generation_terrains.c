#include "generation_terrains.h"
#include "terrain.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int nb_cases_occupees(Terrain *T) {
  int occupee = 0;
  int x, y;
  for (x = 0; x < largeur(T); x++) {
    for (y = 0; y < hauteur(T); y++) {
      if (T->tab[x][y] != LIBRE)
        occupee++;
    }
  }
  return occupee;
}

// Test de generation aléatoire de terrains
// Le programme génère n terrains de largeur et hauteur fixes
// avec largeur et hauteur impaires et inférieures a dimension_max de terrain.h
// avec densité d'obstacle dObst
// autre que la case centrale soit occupee
// l'appel du programme se fait avec 5 arguments :
// generation_terrains N largeur hauteur dObstacle fichier_res
// la sortie se fait dans le fichier resultat

int main(int argc, char **argv) {
  int N, l, h;
  float dObst;
  FILE *resFile;
  Terrain T;

  if (argc < 6) {
    printf(
        "Usage: %s <N> <largeur> <hauteur> <densite_obstacle> <fichier_res> \n",
        argv[0]);
    return 1;
  }

  N = strtol(argv[1], NULL, 10);
  l = strtol(argv[2], NULL, 10);
  h = strtol(argv[3], NULL, 10);
  dObst = strtof(argv[4], NULL);

  // test de l et h
  if (l > DIM_MAX || l % 2 == 0) {
    printf("Largeur incorrecte : doit être impaire et <= %d\n", DIM_MAX);
    return 1;
  }
  if (h > DIM_MAX || h % 2 == 0) {
    printf("Hauteur incorrecte : doit être impaire et <= %d\n", DIM_MAX);
    return 1;
  }
  if ((dObst > 1) || (dObst < 0)) {
    printf("Densité incorrecte : doit être comprise entre 0 et 1\n");
    return 1;
  }

  // Ouverture du fichier résultat
  resFile = fopen(argv[5], "w");
  
  // Écriture du nombre de terrains générés dans le fichier de résultat
  fprintf(resFile, "Nombre de terrains générés : %d\n", N);

  // Initialisation de la fonction de génération de nombres aléatoires
  srand(time(NULL));

  // Génération aléatoire des terrains
  int terrainTotal = 0;            // Nombre total de terrains générés
  float densiteMoyenne = 0.0;      // Densité moyenne des obstacles dans les terrains obtenus
  float sommeDensites = 0.0;       // Somme des densités d'obstacles obtenues
  float densiteObstacle;           // Densité d'obstacles pour chaque terrain généré
  int terrainIndex = 1;            // Indice du terrain actuel

  while (terrainIndex <= N) {
      // Génération aléatoire d'un terrain
      generation_aleatoire(&T, l, h, dObst);
      terrainTotal++;

      // Vérification de l'existence d'un chemin vers la sortie
      while (!existe_chemin_vers_sortie(&T)) {
          generation_aleatoire(&T, l, h, dObst);
          terrainTotal++;
      }

      // Coordonnées du centre du terrain
      int cx = l / 2;
      int cy = h / 2;
      int surface = l * h;

      // Écriture des détails du terrain généré dans le fichier de résultat
      fprintf(resFile, "\n");
      fprintf(resFile, "*************************      Terrain numéro: %d      ******************** : \n", terrainIndex);
      ecrire_terrain(resFile, &T, cx, cy);

      // Calcul et affichage des statistiques du terrain
      densiteObstacle = (float)nb_cases_occupees(&T) / surface;
      printf("\n");
      printf("*******      Terrain numéro: %d      ******* : \n", terrainIndex);
      afficher_terrain(&T);
      printf("Nombre d'obstacles dans le terrain :  %d\n", nb_cases_occupees(&T));
      printf("Densité d'obstacles dans le terrain : %f\n", densiteObstacle);
      printf("Densité d'obstacles attendue dans le terrain : %f\n", dObst);

      // Écriture des statistiques dans le fichier de résultat
      fprintf(resFile, "\n");
      fprintf(resFile, "Nombre d'Obstacles :  %d\n", nb_cases_occupees(&T));
      fprintf(resFile, "Densité d'obstacles dans le terrain : %f\n", densiteObstacle);
      fprintf(resFile, "Densité d'obstacles attendue dans le terrain : %f\n", dObst);
      fprintf(resFile, "\n\n\n");

      // Mise à jour de la somme des densités d'obstacles
      sommeDensites += densiteObstacle;

      // Passage au terrain suivant
      terrainIndex++;
      sleep(1); // Pour éviter la génération de terrains identiques
  }

  // Calcul et affichage des statistiques globales
  densiteMoyenne = sommeDensites / N;
  float prcTerrainsValides = (float)N / terrainTotal * 100;   // Pourcentage de terrains validés
  printf("\n\n");
  printf("Nombre total de terrains générés : %d\n", terrainTotal);
  printf("Pourcentage de terrains valides en total : %f\n", prcTerrainsValides);
  printf("Densité moyenne des terrains obtenus : %f\n", densiteMoyenne);

  // Écriture des statistiques globales dans le fichier de résultat
  fprintf(resFile, "Pourcentage de terrains valides en total : %f\n", prcTerrainsValides);
  fprintf(resFile, "Densité moyenne des terrains obtenus : %f\n", densiteMoyenne);

  // Fermeture du fichier de résultat
  fclose(resFile);

  // Fin du programme
  return 0;
}