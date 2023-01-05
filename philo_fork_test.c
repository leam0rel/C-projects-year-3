/* ------------------------------------------------------------------------ */
/*                   E N T Ê T E S    S T A N D A R D S                     */
/* ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>


/* ------------------------------------------------------------------------ */
/*                      M A C R O - F O N C T I O N S                       */
/* ------------------------------------------------------------------------ */


#define CHECK_SEM(status, msg)                                             \
    if(SEM_FAILED == (status))                                                  \
    {                                                                        \
        perror(msg);                                                         \
        exit(EXIT_FAILURE);                                                  \
    }

#define CHECK(status, msg)                                                   \
    if (-1 == (status)) {                                                    \
        perror(msg);                                                         \
        exit(EXIT_FAILURE);                                                  \
    }


/* ------------------------------------------------------------------------ */

//Programme des philosophes avec les fork
#define NB_PHILOS 3

//voisin de droite :
#define voisinDroite(i) (((i) == NB_PHILOS - 1) ? 0 : (i) + 1)

//voisin de gauche :
#define voisinGauche(i) (((i) == 0) ? NB_PHILOS - 1 : (i)-1)

//test
#define test(i) (etatsTab[(i)] == S && etatsTab[voisinGauche(i)] != M && etatsTab[voisinDroite(i)] != M)


//Définitions diverses
typedef enum{P,M,S} etat_t; // ensemble des états possibles
etat_t etatsTab[NB_PHILOS]; //tableau qui coporte autant d'état que de philosophes
sem_t *mutex; // sémaphores mutex 
sem_t *SP[NB_PHILOS]; // sémaphores philo


//on affiche les états
void afficherEtats(){

    for (long i = 0; i < NB_PHILOS; i++){

        if (etatsTab[i] == P){ 
            printf("P ");
        }
        if (etatsTab[i] == S){ 
            printf("S ");
        }
        if (etatsTab[i] == M){ 
            printf("M ");
        }
    }
    printf("\n");
}

//c'est ici qu'on utilise test(i)
void tester(int i){
    if (test(i)){
        etatsTab[i]=M; 
        afficherEtats();
        sem_post(SP[i]);
    }
}

//voir pq on met ca dans des fichiers lol // car un fork peut pas echanger des données avec des processus

//pour lire l'état du philosophe
void lireEtat(char * file) {
    FILE *pF = fopen(file, "r");
    fread(etatsTab, sizeof(etat_t), NB_PHILOS, pF);
    fclose(pF);
}

//pour changer l'état du philosophe
void changerEtat(char * file) {
    FILE *pF = fopen(file, "w");
    fwrite(etatsTab, sizeof(etat_t), NB_PHILOS, pF);
    fclose(pF);
}



void *philo(long no){    

    
    while (1){
        //penser
        sem_wait(mutex);
        lireEtat("etat");
        etatsTab[no] = S;
        changerEtat("etat");
        afficherEtats();
        tester(no);
        sem_post(mutex);
        //souhaiter
        sem_wait(SP[no]);
        //manger
        sem_wait(mutex);
        lireEtat("etat");
        etatsTab[no]=P; 
        changerEtat("etat");
        afficherEtats();
        tester(voisinDroite(no));
        tester(voisinGauche(no));
        sem_post(mutex);
    }
    return EXIT_SUCCESS;

}

int main(){
    
      
    //initialisation pid_t des philosophes
    pid_t tidPH[NB_PHILOS];
     

    //init semaphores MUTEX
    CHECK_SEM(mutex = sem_open("PH_mutex", O_CREAT, S_IRWXU, 1), "sem_open()");
    CHECK(sem_init(mutex, 1, 0), "sem_init()");

    //création des sémaphores DES PHILO
    for (int i = 0; i < NB_PHILOS; i++) 
    {
        char nomSem[10];
        sprintf(nomSem, "SP_%d", i);                                               
        CHECK_SEM(SP[i] = sem_open(nomSem, O_CREAT, S_IRWXU, 1), "sem_open"); 
        CHECK(sem_init(SP[i], 1, 0), "sem_init()");

    }

    //ecrire une fois dans le fichier car le main possède le fichier SINON CA MARCHE PAS
    changerEtat("etat");

    //création des processus
    for (int i = 0; i < NB_PHILOS; i++)
    {
        CHECK(tidPH[i] = fork(), "Echec fork"); // vérifie si c'est fils i (philosophe i)
        if (tidPH[i] == 0)

        {
            // code du fils i (philosophe i)
            philo(i);
            exit(EXIT_SUCCESS);
        }
    }

    //donne le top départ 
    CHECK(sem_post(mutex), "--erreur sur sem_post(TOP départ)"); 

    //attente de la terminaison des fils
    for (long i = 0; i < NB_PHILOS; i++) {
        waitpid(tidPH[i], NULL, 0);
    }
    

}