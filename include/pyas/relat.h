#ifndef _ARITHEXPR_H_
#define _ARITHEXPR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "pyas/chargroup.h"
#include "pyas/list.h"
#include "pyas/rule.h"
#include "pyas/lexem.h"
#include "pyas/arithexpr.h"

  typedef enum {
    OP_BINARY_PLUS ,
    OP_BINARY_MINUS ,
    OP_BINARY_MUL ,
    OP_BINARY_DIV ,
    OP_UNARY_PLUS ,
    OP_UNARY_MINUS ,
    NUMBER ,
    VARIABLE
  } arith_op ;

  struct arith_ast {
    arith_op op;
    union {
      struct {
        struct arith_ast *left ;
        struct arith_ast *right ;
      }  binary_op ;
      struct {
        struct arith_ast *expr ;
      }     unary_op;
      double value ;
      char *identifier;
    }
  };
  
  typedef struct arith_ast *arith_ast_t;

  arith_ast_t arith_binary_op( lexem_t l, arith_ast_t left, arith_ast_t right );
  arith_ast_t arith_unary_op( lexem_t l, arith_ast_t factor );
  arith_ast_t arith_number( lexem_t l );
  arith_ast_t arith_variable( lexem_t l );


#ifdef __cplusplus
}
#endif

#endif
