#ifndef _OBSERVATEUR_H_
#define _OBSERVATEUR_H_

// On initialise un Caractere
typedef enum {A , G , D , M} Caractere ;
/* A : Avancer
   G : Tourner à gauche
   D : Tourner à droite
   M : Mesurer  */

// On initialise l'ensemble d'états
typedef enum {INITIAL , MESURE , ERREUR} Etat ;

/* Pour un automate, on a besoin d'un état initial et d'une fonction 
qui applique les transitions, ainsi que d'un état accepteur */

// Fonction qui retourne l'état initial
Etat initial() ;

// Fonction de transition
Etat transition(Etat e, Caractere c) ;

// Fonction qui retourne 1 si un état est accepteur, 0 sinon
int est_accepteur(Etat e) ;

#endif
