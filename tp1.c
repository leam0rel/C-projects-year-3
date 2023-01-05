#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/wait.h>

/* 	Ajouter ici les #include nécessaires à l'utilisation des 
 *	fonctions Unix de manipulation de fichiers
 */	
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h> 


#ifdef NO_DEBUG

#define	CHECK(sts, msg)				\
	if ( -1 == (sts))					\
	{							\
		perror(msg);				\
		exit(EXIT_FAILURE);			\
	} 							\
	else printf("%s ... Ok\n", msg)
#else
#define	CHECK(sts, msg)				\
	if ( -1 == (sts))					\
	{							\
		perror(msg);				\
		exit(EXIT_FAILURE);			\
	} 
#endif

void processFils(int);

int main (void)
{
	pid_t		pid;
	int		fd;
//	double	Matrice[2][2] = {{0.}};
	double	x;
	int 		i, nb;
	
	/*	Compléter l'opération CHECK ci-dessous de façon à réaliser 
	 * 	l'ouverture du fichier Lse3_tp4.dat
	 *	Préciser la signification du mode d'ouverture
	 *	Préciser la signification des permissions attribuées
	 */
	CHECK(fd = open("LSE3_1213_TP4.date",O_CREAT|O_RDWR,S_IRWXU), "open()");

	
	pid = fork();
	switch(pid)
	{
		case -1:
			perror("fork");
			exit(EXIT_FAILURE);
			
		case  0:
			processFils(fd);
			
		default:	/** suite du processus père	*/
			CHECK(waitpid(pid, NULL, 0), "waitpid()");
			
			/*	Compléter l'opération CHECK ci-dessous de façon à
			 *  revenir au début du fichier
			 */
			CHECK(lseek(fd,0,0), "lseek()");
			
			/*	Compléter l'opération CHECK ci-dessous, de façon à
			 *	lire la première valeur réelle enregistrée dans le 
			 *	fichier
			 */
			
			CHECK(nb = read(fd,&x,sizeof(x)), "read()");
			
			for (i = 0; nb > 0 ; i++)
			{
				if (i % 4 == 0)
					printf("\n%4d :\t", i + 1);
				printf("%11.6f\t", x);
				
				/*	Compléter l'opération CHECK ci-dessous, 
			 	 *	de façon à lire la valeur réelle suivante			 	 	 *	dans le fichier
			 	 */				
				CHECK(nb = read(fd,&x,sizeof(x)), "read()");				
			}
			
			printf("\n----------------------------------------"
			       "----------------------------------------\n");
	}	

	/*	Compléter l'opération CHECK ci-dessous, de façon à réaliser
	 * 	la fermeture du fichier
	 */
	CHECK(close(fd), "close()");
	
	return EXIT_SUCCESS;
}

void	processFils(int fd)
{
	double Mat[2][2] = 
		{
			{ 3.141592653589793, 2.7182818284590452},
			{ 1.414213562373095, 1.6180339887498948}
		};
	int i;
	
	for (i = 0; i < 3; i++)
	{
		/*	Compléter l'opération CHECK ci-dessous de façon à 
		 *	enregistrer le contenu complet de la matrice Mat.
		 */
		CHECK(write(fd,Mat,sizeof(Mat)), "write()");
		
		/* Exemple de mise à jour de la matrice		*/
		Mat[0][0] *= Mat[0][0]; Mat[0][1] *= Mat[0][1];
		Mat[1][0] *= Mat[1][0]; Mat[1][1] *= Mat[1][1];
	}	
	exit(EXIT_SUCCESS);
}

