#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pyas/regexp.h>




int main(int argc, char**argv) {
    if (argc!=2) {
        printf("Veuillez rentrer uniquement une expression régulière\n");
    exit(EXIT_FAILURE);
    }
    else {

    chargroup_t fifo = alloc_cgroup();
    int r = re_read(argv[1],&(fifo.file));
    if (r == 0){
        printf("Erreur: Opérateur actif entre crochet\n");
        exit(EXIT_FAILURE);
    }
    if (r == -1){
        printf("Erreur: Opérateur mal placé: sans caractère avant\n");
        exit(EXIT_FAILURE);
    }
       if (r == -5){
        printf("Erreur: Opérateur mal placé: sans caractère apres le \"^\"\n");
        exit(EXIT_FAILURE);
    }
    if (r == -2){
        printf("Erreur de syntaxe sur le premier terme\n");
        exit(EXIT_FAILURE);
    }
     if (r == -3){
        printf("Erreur: Crochet ouvrant manquant\n");
        exit(EXIT_FAILURE);
    }
    if (r == -4){
        printf("Erreur: Crochet fermant manquant\n");
        exit(EXIT_FAILURE);
    }


    queue_print(fifo.file,charset_print_wrapper);
    chargroup_delete(fifo , charset_delete_wrapper);
    }
    exit(EXIT_SUCCESS);
    return 0;
}
