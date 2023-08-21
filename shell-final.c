#include "sys.h"


/********************************************************************
 Nom : shell-final.c
 Rôle : implémentation d'un shell
 Auteur : Pierre DAHERON - 20013464
 Version : 2.0
 Licence : réalisée dans le cadre du cours de système d'exploitation (IED-P8)
 Compilation : make
 Usage :  monshell (sans argument)
 ***********************************************************************/



int main(int ac, char* ldc[]){
    //définitions
    char * ligne = NULL;
    char * mot [MaxMot];
    char * dirs [MaxDirs];
    int i, j, nb_args, nb_pipe, arriere_plan, exit,m_if, nb_commandes;
    int pipe_list [MaxCommandes-1][2];


    // Création d'un tableau de commandes et initialisation du tableau. 
    ptr_com commandes[MaxCommandes];
    for(i =0; i< MaxCommandes; i++){
        commandes[i] = init_commande();
    }
    // initialisation des pipes
    nb_pipe = 0;

    // Découper une copie de PATH en répertoires
    int next_dir= decouper(getenv("PATH"),":",dirs,MaxDirs)-1;
    // ajout du répertoire des commandes externes
    dirs[next_dir]="external";

    // auto-completion : ajout des noms de fonctions
    rl_attempted_completion_function = function_name_completion;

    //Boucle de lecture et d'interprétation des commandes

    while(1){
        ligne = readline(PROMPT);
        //*************************** RAZ des variables : *****************************
        // RAZ des redirections (file descriptors, fd):
        for(i=0; i< MaxCommandes; i++){
            commandes[i] -> fd_in = STDIN_FILENO;
            commandes[i] -> fd_out = STDOUT_FILENO;
            j=0;
            while(commandes[i]->ligne[j]){
                commandes[i]-> ligne[j] = NULL;
                j++;
            }
        }
        // RAZ des pipes
        for(i=0; i < nb_pipe;i++){
            pipe_list[i][1]= STDOUT_FILENO;
        }
        nb_args = decouper(ligne," \t\n",mot,MaxMot);
        //gestion de l'arrière-plan
        arriere_plan = 0;
        // pipes
        nb_pipe = 0;
        // monIf
        m_if=0;


        // *************************      décodage de la ligne :    ***************************

        nb_commandes = decode_ligne(mot, commandes,&nb_pipe,pipe_list, &arriere_plan,  nb_args, &m_if);

        // *****************************    Exécution de la commande   ************************

        if(nb_pipe > 0){
            execute_commande(*commandes[0], dirs,0);
            for(i=1;i<nb_commandes;i++){
                // en cas de pipe, on ferme l'entrée du pipe précédent
                close(pipe_list[i-1][1]);
                execute_commande(*commandes[i], dirs,0);
                close(pipe_list[i-1][0]);
            }

        }

        else{
            // Pour la première commande, on considère l'arrière plan
            exit = execute_commande(*commandes[0], dirs, arriere_plan);
            if(exit == 1) break;
            // Pour les autres commandes, pas d'arrière plan.
            for(i=1;i<nb_commandes;i++){

                execute_commande(*commandes[i], dirs,0);

            }
        }
    }

    printf("Ciao\n");

    // libération de l'espace alloué : 
    for(i=0; i< MaxCommandes;i++){
        free(commandes[i]);
    }
    free(ligne);
    return 0;
    
}










