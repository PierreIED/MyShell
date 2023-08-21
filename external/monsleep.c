/* monsleep.c
Une re−implementation de la commande sleep
v2.0 : accepte plusieurs arguments.
Auteur : Pierre DAHERON
Contexte :cours de système d'exploitation  IED P8
*/

// Compilation : gcc -g -Wall monsleep.c f-monsleep.c

# include <stdio.h>
# include <stdlib.h>
# include <ctype.h>
# include <unistd.h>

static void invalid( char * prog, char * s) {
    fprintf ( stderr, "%s: invalid time interval %s\n", prog, s) ;
    exit (1) ;
}

int main(int ac, char * av[]) {
    int i ;
    int nsec;
    if ( ac < 2){
        fprintf ( stderr, "usage: %s N\n", av[0]);
        return 1;
    }
    for(int j=1;j<ac;j++){


        for( i = 0; isdigit ( av[1][ i ]) ; i ++)
        ;
        if (! isdigit ( av[j][0]) ) invalid( av[0], av[j]) ;
        nsec = atoi ( av[j]) ;
        switch(av[j][i ]) {
            default:
                invalid( av[0], av[j]) ;
            case 'd':
                nsec *= 24*3600;
            case 'h':
                nsec *= 3600;
            case 'm':
                nsec *= 60;
            case 's' :
            case 0:
            ;
        }
        sleep(nsec);
    }
    return 0;
}