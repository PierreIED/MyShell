//
// Created by pierred on 24/04/2022.
//
#include "sys.h"

//**********************************************       Fonctions utilitaires    *************************************************
// Variable des noms de commandes pour l'autocomplétion
char* my_function_names[] = {
        "monCp",
        "monkill",
        "monMan",
        "monsleep",
        "monexit",
        "moncd",
        NULL
};


// Fonction decouper, pour transformer une chaine de caractères en tableau de mots
int decouper(char * ligne, char * sep, char * mot[], int mots_max){
    int i;

    mot[0] = strtok(ligne, sep);
    for (i=1; mot[i-1] !=0 ; i++){
        if(i == mots_max){
            fprintf ( stderr, "Erreur dans la fonction decouper: trop de mots\n");
            mot[i-1] = 0;
            break;
        }
        mot[i] = strtok(NULL, sep);
    }
    return i;
}

// Fonction spawn (wrapper pour execv)
int spawn(char * path, char * args[], int fdIn, int fdOut){
    redirect(fdIn, STDIN_FILENO);
    redirect(fdOut, STDOUT_FILENO);
    //On peut ensuite exécuter avec execv, qui permet d'ajouter un nombre variable d'arguments à exec

    execv(path,args);

    // si l'execv est exécuté, on sort de la fonction spawn, donc pas de retour.


    //gestion des erreurs : on n'affiche rien si la commande n'est pas réalisée

    return 1;
}

void redirect(int new, int old){
    if (old != new){
        int dup = dup2(new, old);
        if(dup == -1) perror("erreur de dup");
    }
}


// Fonction compareMots
// Rôle : compare un mot entré à une liste de mot
// arguments : char * mot entré, char * []liste de mots
// Retour : l'index du mot, ou -1 si le mot n'est pas dans la liste.

int compare_mots(char * mot_cherche, int idx, char * listeMots[], int longueur_liste){
    // si on arrive au bout de la liste sans avoir trouvé le mot
    if(idx == longueur_liste) return -1;
    // si le mot suivant de la liste est le mot recherché
    if( strlen(mot_cherche)> 0 && !strcmp(mot_cherche, listeMots[idx])) return idx;
    // Sinon, on lance la récursion
    return compare_mots(mot_cherche, idx+1, listeMots, longueur_liste);
}

// Fonction decode_ligne
// Rôle : analyser une ligne de code pour pouvoir l'utiliser dans le shell
// Arguments :  mots : liste de mots de la ligne de codes
//              commandes : liste de commandes
//              ptr_stdin : pointeur vers le fd d'entrée
//              ptr_stdout : pointeur vers le fd de sortie
//              ptr_pipe : pointeur vers l'int pipe
//              ptr_arrierePlan : pointeur vers l'int arrierePlan
// retour : le nb de commandes

int decode_ligne(char * mots[], ptr_com commandes[MaxCommandes], int* nb_pipe,int pipeList[MaxCommandes-1][2], int* arrierePlan, int nb_args, int* m_if){
    int nb_commandes =1;
    int idx_commande=0;
    char * mots_cherches[] = {"&","<",">",">>","|"};
    int test_mot, temp;
    for(int i=0;i<nb_args && mots[i];i++){
        test_mot = compare_mots(mots[i], 0, mots_cherches,5);

        switch(test_mot){
            case 0 : // cas de &
                if(*nb_pipe !=0) {
                    perror("& incompatible avec |");
                    return 0;
                }
                if(nb_commandes ==MaxCommandes){
                    perror("nombre de commandes max atteint ");
                    return 0;
                }
                // On augmente le nombre de commandes si le & n'est pas en dernière position
                if(i != nb_args-1) {
                    nb_commandes++;
                    idx_commande=0;
                }
                // on fixe l'arrière plan à 1 (true)
                *arrierePlan = 1;
                // pas d'ajout du mot "&" dans une des commandes.
                break;
            case 1 : // cas de <
                commandes[nb_commandes-1] -> fd_in = open(mots[i+1], O_RDONLY);
                // test de l'ouverture du fichier
                if(commandes[nb_commandes-1] -> fd_in <0) {
                    // si erreur d'ouverture : on l'affiche
                    perror("erreur d'ouverture de fichier");
                    // et on réinitialise le stdin pour éviter les erreurs
                    commandes[nb_commandes-1] -> fd_in=STDIN_FILENO;
                }
                // on incrémente i pour supprimer l'adresse
                i++;
                break;
            case 2 : // cas de >
                // Ce mode doit effacer le fichier s'il existe :
                unlink(mots[i+1]);
                commandes[nb_commandes-1] -> fd_out = open(mots[i+1], O_WRONLY | O_CREAT, S_IRWXU);
                if(commandes[nb_commandes-1] -> fd_out <0) {
                    // si erreur d'ouverture : on l'affiche
                    perror("erreur d'ouverture de fichier");
                    // et on réinitialise le stdin pour éviter les erreurs
                    commandes[nb_commandes-1] -> fd_out =STDOUT_FILENO;
                }
                // on incrémente i pour supprimer l'adresse
                i++;
                break;
            case 3 : // cas de >>
                commandes[nb_commandes-1] -> fd_out = open(mots[i+1], O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
                if(commandes[nb_commandes-1] -> fd_out<0) {
                    // si erreur d'ouverture : on l'affiche
                    perror("erreur d'ouverture de fichier");
                    // et on réinitialise le stdin pour éviter les erreurs
                    commandes[nb_commandes-1] -> fd_out=STDOUT_FILENO;
                }

                // on incrémente i pour supprimer l'adresse
                i++;
                break;
            case 4 : // cas de |
                // conflit avec & :
                if(*arrierePlan !=0){
                    perror("& incompatible avec |");
                    return 0;
                }
                // nb max pipes
                if(*nb_pipe==MaxCommandes-1){
                    perror("2 pipes max dans ce shell");
                    return 0;
                }
                // création du pipe :
                temp = pipe( pipeList[*nb_pipe]);
                if(temp !=0){
                    perror("erreur de création du pipe");
                    return 0;
                }
                commandes[nb_commandes-1]-> fd_out = pipeList[*nb_pipe][1];
                // on change le numéro de la commande
                nb_commandes++;
                idx_commande=0;
                commandes[nb_commandes-1]-> fd_in = pipeList[*nb_pipe][0];

                // et on augmente le nombre de pipes
                *nb_pipe +=1;
                break;
            default :
                // autres cas, on ajoute le mot dans la commande courante.
                commandes[nb_commandes-1]->ligne [idx_commande] = mots[i];
                idx_commande++;
        }
    }
    return nb_commandes;
}


// Fonction : exécute commande
// Rôle : exécuter une commande
// arguments : commande la commande à exécuter
// Retour : aucun

int execute_commande(commande commande_en_cours, char * dirs[], int arriere_plan){
    char pathname [MaxPathLength];
    int tmp;

    if(commande_en_cours.ligne [0] == 0) //ligne vide
        return 0;

    //implémentation des commandes internes
    if(!strcmp(commande_en_cours.ligne [0],"moncd")){
        // Pour la fonction moncd, on n'a pas besoin d'avoir "NULL" du tableau mot.
        int nbmots = 0;
        while(commande_en_cours.ligne[nbmots]){
            nbmots++;
        }
        // On considère donc les n-1 mots.
        moncd(nbmots,commande_en_cours.ligne);
        return 0;
    }
    if (!strcmp(commande_en_cours.ligne[0],"monexit")){
        return 1;
    }

    tmp = fork();   // création du processus enfant
    if(tmp < 0){
        perror("fork");
        return 0;
    }

    if(tmp != 0){//processus parent
        if(! arriere_plan) while(wait(0) != tmp);
        return 0;
    }

    // Processus enfant, on exécute la commande


    for(int i=0; dirs[i];i++){

        snprintf(pathname, sizeof(pathname), "%s/%s", dirs[i], commande_en_cours.ligne[0]);
        spawn(pathname, commande_en_cours.ligne,commande_en_cours.fd_in,commande_en_cours.fd_out);
    }

    return 0;

}



char ** function_name_completion(const char * text, int start, int end){
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, function_name_generator);
}

char * function_name_generator(const char * text, int state){
    static int list_index, len;
    char *name;

    if(! state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = my_function_names[list_index++])) {
        if (strncmp(name, text, len) == 0){
            return strdup(name);
        }
        else{
            free(name);
        }
    }

    return NULL;
}


// Fonction : ini_commande
// Rôle : initilisation d'une structure commande pour pouvoir la modifier ensuite.

ptr_com init_commande(){
    ptr_com new_command = (ptr_com) malloc(sizeof(commande));
    new_command -> fd_in = STDIN_FILENO;
    new_command -> fd_out = STDOUT_FILENO;
    *new_command -> ligne = *initArray(MaxMot);
    return new_command;
}


char ** initArray(int array_length){
    static char * array[MaxMot];
    for(int i=0; i< array_length;i++){
        array[i]=NULL;
    }
    return array;
}

// **********************************************   Fonction de commandes internes **********************************************************

// Fonction moncd pour changer de répertoire
int moncd(int ac, char * commande[]){
    char * dir;
    int t;

    // traitement des arguments
    if(ac < 2){
        dir = getenv("HOME");
        if (dir == 0)
            dir = "\temp";
    }
    else if (ac > 2){
        fprintf ( stderr, "usage: %s [dir] ne doit  avoir qu'un argument\n", commande[0]) ;
        return 1;
    }
    else{
        dir = commande[1];
    }

    // changement de répertoire
    t = chdir(dir) ;
    if ( t < 0){
        perror(dir) ;
        return 1;
    }
    return 0;
}

