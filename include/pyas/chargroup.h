#ifndef _CHARGROUP_H_
#define _CHARGROUP_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <pyas/charset.h>

typedef struct chargroup_t{
  queue_t file; // File de charset
} chargroup_t ;

typedef struct chargroup_t chargroup_t;

chargroup_t alloc_cgroup(void);//Fonction permettant d'allouer un chargroup.
void chargroup_print (chargroup_t a, action_t printq);//Fonction permettant d'afficher le contenu d'un chargroup dans le terminal
int charset_reveal(charset_t a); //Fonction révélant la position du charset dans la file
chargroup_t chargroup_delhead(chargroup_t a, action_t delete);
chargroup_t chargroup_enqueue(chargroup_t a, charset_t b);
chargroup_t chargroup_delete(chargroup_t a, action_t delete);
#ifdef __cplusplus
}
#endif

#endif
