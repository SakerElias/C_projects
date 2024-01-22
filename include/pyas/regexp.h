/**
 * @file regexp.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Jul  28 09:56:03 2022
 * @brief General include for regexp
 *
 * General include for regexp, as in the project's pdf document
 */

#ifndef _REGEXP_H_
#define _REGEXP_H_
#include "../include/pyas/chargroup.h"
#include "../include/pyas/charset.h"


#ifdef __cplusplus
extern "C" {
#endif
struct link_t {
  void          *content;
  struct link_t *next;
};

typedef struct link_t *queue_t;
int re_read( char* regexp , queue_t *file);
int re_match( queue_t *regexp , char *source , char **end ,int i );

#ifdef __cplusplus
}
#endif

#endif /* _ALL_H_ */
