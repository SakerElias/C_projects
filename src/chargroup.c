#include "../include/pyas/chargroup.h"
#include "../include/pyas/queue.h"
#include "../include/pyas/charset.h"
#include <assert.h>

struct link_t {
  void          *content;
  struct link_t *next;
};

chargroup_t alloc_cgroup(void){
  chargroup_t a;
  a.file=queue_new();
  return a;
}

// chargroup_t enqueue(chargroup_t cgroup, void* a, action_t load (void * cset1, void * cset2)){
//
// }

int chargroup_length(chargroup_t a){
  return queue_length(a.file);
}

 void chargroup_print (chargroup_t a, action_t printq){
   int i;
   if (queue_empty(a.file)){
     queue_print(a.file,printq);
   }
   printf("on rentre dans c print\n");
   for ( i=0; i < chargroup_length(a) ;i++ ) {
     printf("on est boucle du cprint\n");
     printq((a.file)->content);
     a.file=(a.file)->next;
   }
 }


chargroup_t chargroup_delhead(chargroup_t a, action_t delete){
  queue_t file;
  file = (a.file)->next;
  (a.file)->next = a.file->next->next;
  delete(file->content);
  free(file);
  return a;
}

chargroup_t chargroup_enqueue(chargroup_t a, charset_t b){
b->refcount++;
a.file=enqueue(a.file,b);
return a;
}

chargroup_t chargroup_delete(chargroup_t a, action_t delete){
  if (a.file == NULL) {return a;}
  while(a.file!=a.file->next){
    a=chargroup_delhead(a,delete);
  }
  delete(a.file->content);
  free(a.file);
  a.file=NULL;
  return a;
}
