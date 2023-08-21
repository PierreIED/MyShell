#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char* string ;
// Quelques uns des signaux utilisés : 
typedef struct {string nom; int num;} Signal;
Signal SIGNAUX[8] = {{"SIGHUP",1}, {"SIGINT",2},{"SIGQUIT",3}, {"SIGKILL",4},{"SIGABRT",6}, {"SIGKILL",9},{"SIGALARM",14},{"SIGTERM",15} };


// Prototypes :
int usage(string);
int option(string, int*);
int search_signal(int);


int main(int ac, string argv[]){
    if (ac <2) usage("usage : kill [pid]");
    int i=1;
    int signal = 15; // défaut : SIGTERM
    while (i < ac){
        if(argv[i][0]=='-') option(argv[i], &signal);
        else{
            if (atoi(argv[i])==0){
                // si le pid n'est pas un nombre
                usage("usage : les pid doivent être des nombres");
            }
            else{
                // si le pid n'est pas 0, on envoie le signal voulu:
                if(signal){
                    kill(atoi(argv[i]), signal);
                }
                else{
                    // sinon (option -l), on affiche le signal correspondant au nombre donné
                    int idx = search_signal(atoi(argv[i]));
                    if (idx==-1) printf("pas de signal associé à ce nombre.");
                    else printf("%d : %s \n", SIGNAUX[idx].num, SIGNAUX[idx].nom);
                }
            }
        }
        i++;
    }
    if (!signal && ac==2){
        for(int i=0;i<8;i++){
            printf("%d) %s\n", SIGNAUX[i].num, SIGNAUX[i].nom);
        }
    }
    
    return 0;
}

int usage(string message){
    printf("%s\n", message);
    exit(1);
}

int option(string option, int* signal){
    if(! strcmp(option,"-l")){
        *signal = 0;
    }
    else{
        if(atoi(option+1) !=0){
            *signal = atoi(option+1);
            return 0;
        }
        for(int i=0; i< 8; i++){
            if(!strcmp(option+1, SIGNAUX[i].nom)){
                *signal = SIGNAUX[i].num;
                return 0;
            }
        }
        usage("les options disponibles sont -l pour lister les fonctions, ou -[nom_du_signal], ou -[num_du_signal]");
    }
    return 0;
}

int search_signal(int num){
    for(int i=0; i <8;i++){
        if(SIGNAUX[i].num == num) return i;
    }
    return -1;
}