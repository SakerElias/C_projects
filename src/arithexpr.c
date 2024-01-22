

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

int parse_arith_expr (list_t *lexems){
  printf("Parsing arithmetic expression\n");
  if (-1==parse_term(lexems)){
    return -1;
  }
  while (next_lexem_is(lexems,"op::sub")||next_lexem_is(lexems,"op::sum")){
    lexem_advance(lexems);
    if(-1==parse_term(lexems)){
      return -1;
    }
  }
  return 0;
}


int parse_factor(list_t *lexems){
  printf("Parsing factors\n");

  if (next_lexem_is(lexems,"number")||next_lexem_is(lexems,"identifier")){
    printf("*Found factor:");
    lexem_print(lexem_peek(lexems));
    printf("\n");
    lexem_advance(lexems);
    return 0;
  }
  if(next_lexem_is(lexems,"paren::left")){
    lexem_advance(lexems);
    if(-1==parse_arith_expr(lexems)){
      return -1;
    }
    if(next_lexem_is(lexems,"paren::right")){
      lexem_advance(lexems);
      return 0;
    }
    print_parse_error("Missing right parenthesis",lexems);
    return -1;
  }
  print_parse_error("Unexpected input",lexems);
  return -1;
}

int parse_term(list_t *lexems){
  printf("Parsing term\n");
  if (-1==parse_s_factor(lexems)){
    return -1;
  }
  while(next_lexem_is(lexems,"op::mul")||next_lexem_is(lexems,"op::div")){
    lexem_advance(lexems);
    if(-1==parse_s_factor(lexems)){
      return -1;
    }
  }
  return 0;
}


int parse_s_factor(list_t *lexems){
  printf("Parsing s_factor\n");
  if( next_lexem_is(*lexems,"number") || next_lexem_is(*lexems,"identifier") ){
    if(-1==parse_factor(lexems)){
      return -1;
    }
  }
  return 0;
}
