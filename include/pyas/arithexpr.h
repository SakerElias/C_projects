#ifndef _ARITHEXPR_H_
#define _ARITHEXPR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pyas/chargroup.h"
#include "pyas/list.h"
#include "pyas/rule.h"
#include "pyas/lexem.h"

  int parse_arith_expr (list_t *lexems);
  int parse_term (list_t *lexems);
  int parse_s_factor (list_t *lexems);
  int parse_factor (list_t *lexems);

#ifdef __cplusplus
}
#endif

#endif
