
/**
 * @file queue.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Jul  2 19:02:17 2021
 * @brief Queue.
 *
 * Queue.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* NULL */

#include "../include/pyas/queue.h"

struct link_t {
  void          *content;
  struct link_t *next;
};

queue_t queue_new( void ) {
  return NULL;
}

void*  queue_first( queue_t q ) {

  assert( !list_empty( q ) );

  return q->content;
}

int queue_empty (queue_t q) {
  return NULL==q;
}

queue_t enqueue (queue_t q, void* object) {
  // printf("On enfile\n");
  queue_t p = calloc(1,sizeof(*p));
  if (p==NULL){
    printf("Erreur dans l'allocation de la file dans enqueue\n");
    return NULL;
  }
  p->content=object;
  if(1==queue_empty (q)){
    p->next=p;
    return p;
  }
  p->next=q->next;
  q->next=p;
  return p;
}

int compare(queue_t file){                  //MAL FAIT
  if(queue_empty(file)) return 0;
  if(file->next == file) return 0;
  return 1;
}
/*
queue_t enqueue(queue_t q, void* object) {
    struct link_t *new = malloc(sizeof(struct link_t)); // Allouez un nouvel élément
    assert(new);

    new->content = object;
    new->next = NULL; // Le nouvel élément sera le dernier élément de la file

    if (q == NULL) {
        // Si la file est vide, le nouvel élément devient la file
        return new;
    } else {
        // Parcourez la file pour trouver le dernier élément
        struct link_t *p;
        for (p = q; p->next != NULL; p = p->next);

        // Ajoutez le nouvel élément à la fin de la file
        p->next = new;

        return q;
    }
}

*/



list_t  queue_to_list( queue_t q ) {
  list_t l = list_new();
  l = q->next;
  q->next = NULL;
  return l;
}


int queue_length(queue_t q){
  if (queue_empty(q)) {
    printf("La file est vide\n");
    return 0;
  }
  else{
    int i=0;
    queue_t p=q->next;
    do {
      i++;
      p=p->next;
    } while( p != q->next );
    return i;
  }
}

void* queue_dequeue( queue_t *q){
  void* e;
  assert(!queue_empty(*q));
  if (queue_length(*q)==1){//Cas particulier d'une file à un élément
    e=(*q)->content;
    free(*q);
    (*q)=NULL;
    return e;
  }
  e=((*q)->next)->content;
  queue_t tete=(*q)->next;
  (*q)->next=tete->next;
  free(tete);
  return e;
}


int queue_print(queue_t q, action_t printq){
  int i=0;
  if (queue_empty(q)) {
    printf("La file est vide\n");
  return 0;
}
  else{
    queue_t p=q->next;
    do {
      i++;
      printq(p->content);
      p=p->next;
    } while( p != (q)->next );
    return i;
  }
}


queue_t queue_delhead(queue_t q, action_t dela){
  // printf("on rentre dans delhead\n");
  assert(!queue_empty(q->next));
  // printf("Pas de problème d'assert\n");
  queue_t c=queue_new();
  // printf("Pas de problème d'alloc de file\n");
  c->content=q->content;
  c->next=q->next;
  q=c->next;
  c->next=q->next;
  dela(q->content);
  free(q);
  q=NULL;
  return c;
}
//
// int compare (queue_t file, queue_t* p_file){    /*p_file pointeur sur le debut de la file qui ne bouge pas*/
//   if(NULL == file) return 0;
//   if(file->next = p_file) return 0;
//   return 1;
// }

queue_t queue_delete(queue_t q, action_t dela){
  assert(!queue_empty(q));
  if (q==q->next){
    dela(q->content);
    return NULL;
  }
  queue_t p=q->next;
  do {
    p=p->next;
    q=queue_delhead(q,dela);
    /* code */
  } while(q!=p);
  return NULL;
}
