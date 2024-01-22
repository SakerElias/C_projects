#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pyas/regexp.h>
#include <pyas/charset.h>
#include <pyas/chargroup.h>

int main(){
  printf("-----------------------------------------\n");
  printf("-----------------------------------------\n");
  printf ("Test simple sur les files \n");
  chargroup_t fifo=alloc_cgroup();
  chargroup_print(fifo,charset_print_wrapper);
  charset_t set = charset_initialize();
  set = create_cset(set, "abc", 0, 1);
  fifo = chargroup_enqueue(fifo, set);
  printf("Nombre de ref %d\n",set->refcount);
  printf("On passe au print du chargroup ne contenant qu'un seul élément\n");
  queue_print(fifo.file,charset_print_wrapper);
  printf("On vient d'afficher le chargroup qui ne contient bien qu'un seul élément,\n");
  printf("-----------------------------------------\n");


  printf("-----------------------------------------\n");
  printf("Test de chargroup_delhead\n");
  printf("On rajoute un charset en tête de liste.\n");
  charset_t newset = charset_initialize();
  newset = create_cset(newset, "rtz", 0, 1);
  fifo = chargroup_enqueue(fifo, newset);
  queue_print(fifo.file,charset_print_wrapper);
  printf("La file contient deux charsets\n");
  printf("-----------------------------------------\n");
  printf("-----------------------------------------\n");
  printf("Maintenant il faut delhead\n");
  // printf("ehoh\n");
  fifo=chargroup_delhead(fifo, charset_delete_wrapper);
  printf ("Test effectué \n");
  queue_print(fifo.file,charset_print_wrapper);
  chargroup_delete(fifo,charset_delete_wrapper);
  printf("---------------FIN DU TEST\n");
  // printf("-----------------------------------------\n");
  // printf("-----------------------------------------\n");
  // chargroup_t c = alloc_cgroup();
  // printf("premiere alloc de c, on applique re_read avec abc\n");
  // re_read("abc",&(c.file));
  // printf("on affiche le chargroup\n");
  // queue_print(c.file,charset_print_wrapper);
  // chargroup_delete(c,charset_delete_wrapper);
  // printf("on affiche le chargroup apres le delete\n");
  // queue_print(c.file,charset_print_wrapper);
  // re_read("efg",&c.file);
  // printf("on affiche le chargroup apres le re_read efg\n");
  // queue_print(c.file,charset_print_wrapper);
  // chargroup_delete(c,charset_delete_wrapper);
  return 1;
}
