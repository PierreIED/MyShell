#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char * argv[]){
    //usage : 
    if(argc !=3) perror("usage : monCp [path_to_copy] [path_to_paste], le fichier à copier doit être existant");

    // variables
    FILE *f_in, *f_out;
    char c;

    //ouverture du fichier à lire
    f_in=fopen(argv[1],"rt");
    // test de l'ouverture
    if (f_in==NULL) {
        perror("erreur d'ouverture de fichier");
        exit(1);
        }

    // ouverture du fichier à écrire
    f_out = fopen(argv[2],"w");
    // test de l'ouverture
    if(f_out==NULL){
        perror("erreur en écriture");
        exit(1);
    }

    // Boucle de copie d'un fichier à l'autre
    while((c=fgetc(f_in))!=EOF){
        fprintf(f_out,"%c",c);
    }

    // Fermeture des deux fichiers. 
    fclose(f_in);
    fclose(f_out);
    return 0;
}

