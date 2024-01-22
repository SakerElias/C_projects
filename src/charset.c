#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* NULL */
#include <string.h>

#include "../include/pyas/charset.h"

struct link_t {
  void          *content;
  struct link_t *next;
};


charset_t charset_initialize(void){
  charset_t a= (charset_t)malloc(sizeof(*a));
  if (a == NULL) {
        fprintf(stderr, "L'allocation de mémoire a échoué.\n");
        return NULL;
  }
  int i;
  for (i=0;i<256;i++){
    a->tab[i]=0;
  }
  a->type=0;
  a->comp=0;
  a->refcount=0;
  return a;
}


charset_t charset_reverse(charset_t a){ //Fonction permettant de prendre le complémentaire d’un ensemble de 256 entiers
  if (a->comp==1){
    int i;
    for (i=0;i<256;i++){
      a->tab[i]=1-a->tab[i];
    }
  }
  return a;
}



charset_t create_cset(charset_t a, char *s,int neg, int type){ // A utiliser dans les crochets
  int t=strlen(s);
  int i;
  if (strcmp(s,"acdekij12478")==0) {
      for (i=0;i<256;i++){
    //printf("create_cset");
    a->tab[i]=1;
  }}
  else {
  for (i=0;i<t;i++){
    //printf("create_cset");
    a->tab[(int)s[i]]=1;
  }}
  a->comp=neg;
  a->type =type;
  return a;
}

charset_t create_a_cset(charset_t a, char s,int neg, int type){
  a->comp=neg;
  a->type =type;
  a->tab[(int)s]=1;
  return a;
}

void charset_print (charset_t a){
  int i=0;
  int m=0;
  //printf("Le nombre d'occurences de ce charset dans une structure de données est de %d\n",a->refcount);
  char *phrase=calloc(256,sizeof(char));
    if(a->comp ==0){
      if (a->type==0){
        for(i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) {
            *(phrase+m)=i;
            m++;
          }
        }
        if (*phrase!='\0') printf("One in \"%s\", one time.\n",phrase);
      }
      if (a->type==1){
        for (i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) { // acolade manquante !
            //printf("AJOUT d'un caractère à la chaine phrase\n");
            *(phrase+m)=i; // on ajoute le caractère i ! Pas tab[i] !!!
            m++;
          }
        }
        printf("One in \"%s\", one or more times.\n",phrase);
      }
      if (a->type ==2){
        for (i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) { // acolade manquante !
            //printf("AJOUT d'un caractère à la chaine phrase\n");
            *(phrase+m)=i; // on ajoute le caractère i ! Pas tab[i] !!!
            m++;
          }
        }
        printf("One in \"%s\", zero or more times.\n",phrase);
      }
      if (a->type ==3){
        for (i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) { // acolade manquante !
            //printf("AJOUT d'un caractère à la chaine phrase\n");
            *(phrase+m)=i; // on ajoute le caractère i ! Pas tab[i] !!!
            m++;
          }
        }
        printf("One in \"%s\", zero or one time.\n",phrase);
      }
    }
    else {
      if (a->type==0){
        for(i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) {
            *(phrase+m)=i;
            m++;
          }
        }
        printf("One not in \"%s\", one time.\n",phrase);
      }
      if (a->type==1){
        for (i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) { // acolade manquante !
            //printf("AJOUT d'un caractère à la chaine phrase\n");
            *(phrase+m)=i; // on ajoute le caractère i ! Pas tab[i] !!!
            m++;
          }
        }
        printf("One not in \"%s\", one or more times.\n",phrase);
      }
      if (a->type ==2){
        for (i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) { // acolade manquante !
            //printf("AJOUT d'un caractère à la chaine phrase\n");
            *(phrase+m)=i; // on ajoute le caractère i ! Pas tab[i] !!!
            m++;
          }
        }
        printf("One not in \"%s\", zero or more times.\n",phrase);
      }
      if (a->type ==3){
        for (i=0;i<255;i++){
          //printf("Le tableau %d\n",a->tab[i]);
          if (a->tab[i]) { // acolade manquante !
            //printf("AJOUT d'un caractère à la chaine phrase\n");
            *(phrase+m)=i; // on ajoute le caractère i ! Pas tab[i] !!!
            m++;
          }
        }
        printf("One not in \"%s\", zero or one time.\n",phrase);
      }
    }
  free(phrase);
}

charset_t charset_delete(charset_t a){
  if (a->refcount == 1){
    free(a);
  }
  else a->refcount--;
  return NULL;
}

int charset_selector(queue_t a,int i,charset_t*c) {
  queue_t b= a;
  if (i>queue_length(a)) {printf("erreur dans charset_selector\n"); return 0;}
  else {
    int k;
    for (k=0;k<i;k++){
      b = b->next;
    }
  }
  *c = (charset_t) charset_load( b->content,*c);
  return 1;
}

charset_t charset_load(charset_t a,charset_t b){
  b->type=a->type;
  b->comp=a->comp;
  int i;
  for(i=0;i<255;i++){
    b->tab[i]=a->tab[i];
  }
  return b;
}
int charset_print_wrapper(void *arg) {
    charset_print((struct charlink_t *)arg);
    return 0;
}
int charset_delete_wrapper(void *arg) {
    charset_delete((struct charlink_t *)arg);
    return 0;
}
