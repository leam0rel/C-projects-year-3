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


/* ------------------------------------------------------------------------ */
/*                      M A C R O - F O N C T I O N S                       */
/* ------------------------------------------------------------------------ */
#define CHECK_T(status, msg)                                                 \
  if (0 != (status))   {                                                     \
    fprintf(stderr, "pthread erreur : %s\n", msg);                           \
    exit (EXIT_FAILURE);                                                     \
  }

#define CHECK(status, msg)                                                   \
    if (-1 == (status)) {                                                    \
        perror(msg);                                                         \
        exit(EXIT_FAILURE);                                                  \
    }


/* ------------------------------------------------------------------------ */

//Programme des philosophes avec les threads
#define NB_PHILOS 6
// voisin de droite :
#define voisinDroite(i) (((i) == NB_PHILOS - 1) ? 0 : (i) + 1)

// voisin de gauche :
#define voisinGauche(i) (((i) == 0) ? NB_PHILOS - 1 : (i)-1)

//test
#define test(i) (e[(i)] == S && e[voisinGauche(i)] != M && e[voisinDroite(i)] != M)


//Définitions diverses
typedef void*(*pf_t)(void *); // utile pour la création des threads
typedef enum{P,M,S} etat_t; // ensemble des états possibles
etat_t e[NB_PHILOS]; //tableau qui coporte autant d'état que de philosophes
char * etatstochar = "PMS"; //on convertit les états dans une chaine de caractères, c'est un char *
sem_t mutex; // sémaphore 
sem_t SP[NB_PHILOS]; //


//Fonction qui permet d'afficher les états
void afficherEtats(void){
    for (int i=0; i<NB_PHILOS; i++){ //on parcourt le tableau char *etatstochar
        printf("%c\t", etatstochar[e[i]]);

    }
    printf("\n");
}



void tester(int i){
    if (test(i)){
        e[i]=M; afficherEtats();
        sem_post(&SP[i]);
    }
}


void *philo(long no){
    while (1){
        //penser
        sem_wait(&mutex);
        e[no] = S;
        afficherEtats();
        tester(no);
        sem_post(&mutex);
        //souhaiter
        sem_wait(&SP[no]);
        //manger
        sem_wait(&mutex);
        e[no]=P; afficherEtats();
        tester(voisinDroite(no));
        tester(voisinGauche(no));
        sem_post(&mutex);
    }
    pthread_exit(NULL);

}

//le main va contenir l'initialisation des sémaphores et la création/initialisation des threads, 
//initier la mutex avec 0
//CHECK_T permet de montrer les erreurs

int main(){
    pthread_t thread[NB_PHILOS];
    //création des sémaphores
    CHECK(sem_init(&mutex, 0, 0), "erreur");
    for (long i=0; i<NB_PHILOS; i++){
        CHECK(sem_init(&SP[i],0,0), "erreur");
    }
    //création des threads
    for (long i=0; i<NB_PHILOS; i++)
        CHECK_T(pthread_create (&thread[i], NULL, (pf_t)philo, (void *)i), "pthread_create()");

    sem_post(&mutex); //top départ

    //attente
    for (long i=0; i<NB_PHILOS; i++){
        CHECK_T(pthread_join (thread[i], NULL), "pthread_join()");}

    //destruction des sémaphores
    CHECK(sem_destroy(&mutex), "erreur");
    for (long i=0; i<NB_PHILOS; i++){
        CHECK(sem_destroy(&SP[i]), "erreur");
    }
}
