#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <readline/readline.h>


enum {
    MaxLigne = 1024, // longueur max d'une ligne de commandes
    MaxMot = MaxLigne / 2, // nbre max de mot dans la ligne
    MaxDirs = 100, // nbre max de repertoire dans PATH
    MaxPathLength = 512, // longueur max d'un nom de fichier
    MaxCommandes = 3 // Nombre max de commandes possibles
};

typedef struct Commande{int fd_in; int fd_out; char*  ligne[MaxMot];} commande;
typedef commande *ptr_com;

// prototypes
int decouper(char *, char *, char *[], int);
int spawn(char *, char *[], int, int);
int moncd(int, char *[]);
void redirect(int, int);
void remove_adress(char*[], int);

int compare_mots(char*, int, char*[], int);
int decode_ligne(char*[], ptr_com [MaxCommandes], int*,int[MaxCommandes-1][2], int*, int, int*);
int execute_commande(commande, char*[], int);
ptr_com init_commande();
char ** initArray(int);

//auto_completion
char ** function_name_completion(const char *, int, int);
char * function_name_generator(const char *, int);



//prompt :
#define PROMPT "?"