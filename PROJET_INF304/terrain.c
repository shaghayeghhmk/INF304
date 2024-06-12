#include "terrain.h"
#include "robot.h"
#include <stdio.h>
#include <string.h>

erreur_terrain lire_terrain(FILE *f, Terrain *t, int *x, int *y) {
    int l, h;   // Dimensions du terrain
    int rx, ry; // Coordonnées initiales du robot

    if (f == NULL) {
        return ERREUR_FICHIER;
    }

    // Lecture de la largeur
    if (fscanf(f, "%d", &l) != 1) {
        return ERREUR_LECTURE_LARGEUR;
    }

    // Vérification de la largeur
    if (l <= 0 || l > DIM_MAX) {
        return ERREUR_LARGEUR_INCORRECTE;
    }

    // Lecture de la hauteur
    if (fscanf(f, "%d", &h) != 1) {
        return ERREUR_LECTURE_HAUTEUR;
    }

    // Vérification de la hauteur
    if (h <= 0 || h > DIM_MAX) {
        return ERREUR_HAUTEUR_INCORRECTE;
    }

    // Initialisation des dimensions du terrain
    t->largeur = l;
    t->hauteur = h;

    // Lecture du terrain
    for (int i = 0; i < h; i++) {
        // Initialisation de la longueur de la ligne
        int longueur_ligne = 0;

        for (int j = 0; j < l; j++) {
            char c;
            if (fscanf(f, " %c", &c) != 1) {
                return ERREUR_FICHIER;
            }

            // Vérification de la validité du caractère lu
            switch (c) {
                case '.':
                    t->tab[j][i] = LIBRE;
                    break;
                case '#':
                    t->tab[j][i] = ROCHER;
                    break;
                case '~':
                    t->tab[j][i] = EAU;
                    break;
                case 'C':
                    t->tab[j][i] = LIBRE;
                    rx = j;
                    ry = i;
                    break;
                default:
                  return ERREUR_CARACTERE_INCORRECT; //(*)
            }

            // Augmentation de la longueur de la ligne
            longueur_ligne++;
        }

        // Vérifier la longueur de la ligne
        if (longueur_ligne != l) {
            if (longueur_ligne < l) {
                return ERREUR_LIGNE_TROP_COURTE;//(*)
            } else {
                return ERREUR_LIGNE_TROP_LONGUE;
            }
        }
    }

    // // Vérifier si la position du robot a été trouvée (*)
    // if (!est_case_libre(t, rx, ry)) {
    //     return ERREUR_POSITION_ROBOT_MANQUANTE;
    // }

    // Vérification du nombre de lignes

    char newline;
    if (fscanf(f, " %c", &newline) == 1 && newline != '\n') {
        return ERREUR_LIGNE_TROP_LONGUE;
    }

    // Initialisation de la position du robot
    *x = rx;
    *y = ry;

    return OK;
}


/* Largeur d'un terrain */
int largeur(Terrain *t) {
    return t->largeur;
}

/* Hauteur d'un terrain */
int hauteur(Terrain *t) {
    return t->hauteur;
}


/* Vérifie si une case est libre */
int est_case_libre(Terrain *t, int x, int y) {
  return (x >= 0 && x < t->largeur && y >= 0 && y < t->hauteur && t->tab[x][y] == LIBRE);
}

/* Affiche le terrain */
void afficher_terrain(Terrain *t) {
  for (int i = 0; i < t->hauteur; i++) {
    for (int j = 0; j < t->largeur; j++) {
      switch (t->tab[j][i]) {
        case LIBRE:
          printf(".");
          break;
        case ROCHER:
          printf("#");
          break;
        case EAU:
          printf("~");
          break;
      }
    }
    printf("\n");
  }
}

/* Écriture du terrain dans un fichier */
void ecrire_terrain(FILE *f, Terrain *t, int x, int y) {
  fprintf(f, "%d\n%d\n", t->largeur, t->hauteur);
  for (int i = 0; i < t->hauteur; i++) {
    for (int j = 0; j < t->largeur; j++) {
      if (j == x && i == y) {
        fprintf(f, "C");
      } else {
        switch (t->tab[j][i]) {
          case LIBRE:
            fprintf(f, ".");
            break;
          case ROCHER:
            fprintf(f, "#");
            break;
          case EAU:
            fprintf(f, "~");
            break;
        }
      }
    }
    fprintf(f, "\n");
  }
}