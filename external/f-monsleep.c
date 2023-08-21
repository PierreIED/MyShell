/* f-monsleep.c
Une re−implementation de la fonction sleep
Attention time() est invalide en C99
*/



# include <stdio.h>
# include <unistd.h>
# include <signal.h>
#include <time.h>

void dring(int n){
/* rien a faire */
}

unsigned int sleep(unsigned int nsec){
    int avant, apres;
    avant = time(0);
    signal( SIGALRM, dring);
    alarm(nsec);
    pause();
    apres = time(0);
    return ( avant + nsec) - apres;
}