#ifndef _RULE_H_
#define _RULE_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "pyas/list.h"
  typedef struct rule *rule_t;

  rule_t rule_initialize(rule_t a);
  int     rule_print( void *_rule );
  rule_t rule_delete(rule_t a);
  


#ifdef __cplusplus
}
#endif

#endif /* _RULE_H_ */