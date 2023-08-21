#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]){
    if (argc !=2) perror("usage : monMan [nom_de_commande] - Commandes documentées : moncd, monCp, monexit, monkill, monMan, monsleep");
    char * man_title="man_pages/man_";
    size_t taille = strlen(man_title)+strlen(argv[1]);
    char * man_file =(char *) malloc(taille);
    strcpy(man_file, man_title);
    strcat(man_file, argv[1]);

    FILE *f;
    char c;
    f=fopen(man_file,"rt");

    if (f==NULL) perror("erreur d'ouverture de fichier");
    while((c=fgetc(f))!=EOF){
        printf("%c",c);
    }

    free(man_file);
    fclose(f);
    return 0;

}