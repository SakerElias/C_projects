#ifndef _CHARSET_H_
#define _CHARSET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdio.h>
#include <pyas/callbacks.h>
#include <pyas/queue.h>
#include <stddef.h>


struct charlink_t {
  int tab[256]; //tableau de 0 et de 1 représentant la présence des caractères
  int type;  //entier codant le type d’apparition possible
  int comp; // entier codant la négation cad la présence du ^ devant le charset
  unsigned int refcount;//Compte le nombre de fois ou le charset est référencé dans une structure de données
};
typedef struct charlink_t *charset_t;

charset_t charset_reverse(charset_t a); //Fonction permettant de prendre le complémentaire d’un ensemble de 256 entiers.
charset_t charset_initialize(void);//Fonction permettant d'initialiser le charset.
charset_t charset_load(charset_t a, charset_t b);//Fonction permettant de passer le contenu d'un chaset dans un autre. On load a dans b
charset_t create_cset(charset_t a, char *s,int neg, int type);//Fonction permettant de créer un charset à partir d'une chaine de caractère.
charset_t create_a_cset(charset_t a, char s,int neg, int type);//Fonction permettant de créer un charset à partir d'un caractère.
void charset_print (charset_t a);//Fonction permettant d'afficher le contenu d'un charset.
charset_t charset_delete(charset_t a);
int charset_print_wrapper(void *arg);
int charset_delete_wrapper(void *arg);
int charset_selector(queue_t a,int i,charset_t*c);
#ifdef __cplusplus
}
#endif

#endif
