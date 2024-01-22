#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <pyas/lexem.h>
#include <pyas/list.h>
#include <pyas/queue.h>
#include <pyas/rule.h>
#include <pyas/charset.h>
#include <pyas/chargroup.h>
#include <pyas/arithexpr.h>
#include <pyas/relat.h>

struct lexem {
  char *type;
  char *value;
  int   line;
  int   column;
};


typedef struct lexem *lexem_t;

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

arith_ast_t arith_binary_op( lexem_t l, arith_ast_t left, arith_ast_t right ){
  arith_ast_t a=calloc(1,sizeof(*a));
  a->binary_op.left=left;
  a->binary_op.right=right;

  if (strcmp(l->type,"op::sum")==0) a->op=OP_BINARY_PLUS;
  if (strcmp(l->type,"op::sub")==0) a->op=OP_BINARY_MINUS;
  if (strcmp(l->type,"op::mul")==0) a->op=OP_BINARY_MUL;
  if (strcmp(l->type,"op::div")==0) a->op=OP_BINARY_DIV;
  return a;
}

arith_ast_t arith_unary_op( lexem_t l, arith_ast_t factor ){
  arith_ast_t a=calloc(1,sizeof(*a));
  a->unary_op.expr=factor;
  if(strcmp(l->value,"-")==0) a->op=OP_UNARY_MINUS;
  if(strcmp(l->value,"+")==0) a->op=OP_UNARY_PLUS;
}

arith_ast_t arith_number( lexem_t l ){
arith_ast_t a=calloc(1,sizeof(*a));
a->op=NUMBER;
a->value=strtod(l->value,NULL);
return a;
}

arith_ast_t arith_variable( lexem_t l ){
arith_ast_t a=calloc(1,sizeof(*a));
a->op=VARIABLE;
strcpy(a->identifier,l->value);
return a;
}
