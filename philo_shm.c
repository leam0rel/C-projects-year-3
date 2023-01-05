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
#include <sys/types.h>
#include <sys/mman.h>


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
etat_t *etatsTab; //ecrit etats philo
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



void *philo(long no){    

    while (1){
        //penser
        sem_wait(mutex);
        etatsTab[no] = S;
        afficherEtats();
        tester(no);
        sem_post(mutex);
        //souhaiter
        sem_wait(SP[no]);
        //manger
        sem_wait(mutex);
        etatsTab[no]=P; 
        afficherEtats();
        tester(voisinDroite(no));
        tester(voisinGauche(no));
        sem_post(mutex);
    }
    return EXIT_SUCCESS;

}

int main(){
    
    //initialisation pid_t from philosophers for shared memory 
    pid_t tidPH[NB_PHILOS];
    int	fd;
    long sizePage;

    //initialisation de la zone mémoire
    fd = shm_open("philo", O_RDWR | O_CREAT, S_IRWXU);  //S_IRWXU donne des permissions à tout le monde = lire/écrire/exécuter
    sizePage = sysconf(_SC_PAGE_SIZE); //huh
    ftruncate(fd,sizePage);
    etatsTab = mmap(NULL,sizePage, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    
    //init sémaphores MUTEX
    CHECK_SEM(mutex = sem_open("PH_mutex", O_CREAT, S_IRWXU, 1), "sem_open()");
    CHECK(sem_init(mutex, 1, 0), "sem_init()");


    //création des sémaphores DES PHILO
    for (int i = 0; i < NB_PHILOS; i++){
        char nomSem[10];
        sprintf(nomSem, "SP_%d", i);                                               
        CHECK_SEM(SP[i] = sem_open("nomSem", O_CREAT, S_IRWXU, 1), "sem_open"); //pas grave si nomSem entre ""
        CHECK(sem_init(SP[i], 1, 0), "sem_init()");

    }

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

    
    //tuer la zone mémoire
    munmap(etatsTab,sizePage); //on déconnecte
    close(fd);
    shm_unlink("philo");



    //supprimer les sémaphores DES PHILOS
    for (int i = 0; i < NB_PHILOS; i++)
        CHECK(sem_destroy(SP[i]), "erreur sem_destroy(SP)");


    //supprimer les sémaphores MUTEX
    CHECK(sem_destroy(mutex), "erreur sem_destroy(mutex)");

   


    





















}