#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> #include <time.h>
#define CHECK(sts,msg) if ((sts) == -1) { perror(msg); exit(sts); }
#define N 3
#define DUREE_OUV 20
#define DUREE_FER (2 * DUREE_OUV)
unsigned int dureeOuv = DUREE_OUV; // décompte durée d’ouverture
unsigned int numClient = 0; // numéro du client traité
//unsigned int nbCL = ???; // nombre de places libres
//int pid [???]; // tableau des clients actuels
void coiffeur (void) { // Q3/Q6 : A compléter
    while (1){
        
    }
    return EXIT_SUCCESS;
}



void client (unsigned int numClient) { // Q4/Q7 : A compléter
}



void *acquitterLesFils(void *arg) { // Q8 : A compléter
// Gérer la fin des processus fils à base de la signalisation fils/père
pthread_exit(0); }



void tuerLesFils(???) { // Q9 : A compléter
// Ce traitement est activé sur réception d’un CTRL C
// Terminaison de tous les processus et
// suppression des sémaphores
}
int main ()
{ // Q5 : Mise en place des sémaphores
//
// Q3 : Mise en place du processus coiffeur avec gestion de l’alarme
//
// Q4 : Installation du gestionnaire de SIGUSR1
//
// Q8 : Mise en place du thread d’acquittement
//
// Q9 : Installation du gestionnaire du CTRL-C
//
// Q10 : Modification de la politique d'ordonnacement
while (1) { pause() ;
}
return EXIT_SUCCESS;
}