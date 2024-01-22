#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pyas/regexp.h>
#include <pyas/charset.h>
#include <pyas/chargroup.h>




int main() {
  printf("-----------------------------------------\n");
  printf("-----------------------------------------\n");

  printf ("Test du charset_print \n");
    chargroup_t fifo=alloc_cgroup();
    int num;
    charset_t set = charset_initialize();
    set = create_cset(set, "abc", 0, 1);
    fifo.file = enqueue(fifo.file, set);
    num = queue_print(fifo.file, charset_print_wrapper);
    printf("on a print %d, element\n",num);
    printf("Premier print\n");

    printf("On a print le premier charset\n");

    charset_t a= charset_initialize();
    a=create_cset(a, "ztr", 0, 1);
    fifo.file = enqueue(fifo.file, a);
    printf("ON VA AFFICHER LA FILE AVEC DEUX ELEMENTS\n");

    printf("-----------------------------------------\n");
    printf("-----------------------------------------\n");

    printf("Changement de a\n");
    printf("On print a avant le changement\n");
    charset_print(a);
    charset_t b=charset_initialize();
    a=charset_load(create_a_cset(b,'c', 0, 0),a);
    printf("On print a après le changement\n");
    charset_print(a);
    printf("Nous avons print a après le changement, on enfile ensuite deux fois a dans fifo\n");
    fifo.file = enqueue(fifo.file, charset_load(a,charset_initialize()));
    fifo.file = enqueue(fifo.file, charset_load(a,charset_initialize()));

    printf("-----------------------------------------\n");
    printf("-----------------------------------------\n");
    printf("Nous affichons ensuite la file.\n");
    queue_print(fifo.file, charset_print_wrapper);

    printf("On passe à chargroup_delhead\n");
    fifo = chargroup_delhead(fifo, charset_delete_wrapper);
    printf("Le chargroup avait précédemment 4 éléments, vérifions qu'il n'en a plus que trois\n");
    queue_print(fifo.file,charset_print_wrapper);
    fifo = chargroup_delhead(fifo, charset_delete_wrapper);
    printf("Le chargroup avait précédemment trois éléments, vérifions qu'il n'en a plus que deux\n");
    queue_print(fifo.file,charset_print_wrapper);
    chargroup_delete(fifo,charset_delete_wrapper);
    charset_delete_wrapper(b);

    return 0;
}
