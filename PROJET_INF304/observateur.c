#include <stdio.h>
#include "observateur.h"

// Fonction qui retourne l'état initial
Etat initial() {
    return INITIAL;
}

// Fonction de transition entre les états en fonction de la Caractere
Etat transition(Etat e, Caractere c) {
    switch(e) {
        case INITIAL :
        switch(c) {
            case A :
                return ERREUR;
            case G :
            case D :
                return INITIAL;
            case M :
                return MESURE;
        }
        case MESURE :
        switch(c) {
            case A :
            case G :
            case D :
                return INITIAL;
            case M :
                return MESURE;
        }
        case ERREUR :
            return ERREUR;
    }
}

// Fonction qui retourne 1 si un état est accepteur, 0 sinon
int est_accepteur(Etat e) {
    return e!=ERREUR ;
}