#include "terrain.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    FILE *f;
    Terrain t;
    int x, y;
    erreur_terrain err;

    if (argc < 2) {
        printf("Usage : %s <fichier>\n", argv[0]);
        return 1;
    }

    char nom_fichier[100]; // Choisissez une taille suffisamment grande
    strcpy(nom_fichier, argv[1]);

    do {
        f = fopen(nom_fichier, "r");
        if (f == NULL) {
            printf("Erreur : Le fichier %s n'a pas pu être ouvert.\n", nom_fichier);
            printf("Entrez un nouveau nom de fichier : ");
            scanf("%s", nom_fichier);
            continue;
        }

        err = lire_terrain(f, &t, &x, &y);
        fclose(f);

        switch (err) {
            case ERREUR_LECTURE_LARGEUR:
            case ERREUR_LARGEUR_INCORRECTE:
                printf("Erreur : La lecture de la largeur a échoué.\n");
                break;
            case ERREUR_LECTURE_HAUTEUR:
            case ERREUR_HAUTEUR_INCORRECTE:
                printf("Erreur : La lecture de la hauteur a échoué.\n");
                break;
            case ERREUR_CARACTERE_INCORRECT:
                printf("Erreur : Caractère incorrect trouvé dans le terrain.\n");
                break;
            case ERREUR_LIGNE_TROP_LONGUE:
                printf("Erreur : Une ligne du terrain est trop longue.\n");
                break;
            case OK:
                // Afficher la position initiale du robot uniquement en cas de succès
                afficher_terrain(&t);
                printf("Position initiale du robot : (%d, %d)\n", x, y);
                break;
            default:
                // Sortir de la boucle si le type d'erreur n'est pas ERREUR_FICHIER
                break;
        }
    } while (err == ERREUR_FICHIER);

    return 0;
}
