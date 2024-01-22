
/**
 * @file lexem.h
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Jul  2 17:59:42 2021
 * @brief Lexems.
 *
 * Lexems.
 */

#ifndef _LEXEM_H_
#define _LEXEM_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "pyas/list.h"
#include "pyas/rule.h"
  typedef struct lexem *lexem_t;


  lexem_t lexem_new( char *type, char *value, int line, int column, char *op_code );
  int     lexem_print( void *_lex );
  int     lexem_delete( void *_lex );
  char*  lexem_value( lexem_t l );
  char* lexem_op_code(lexem_t l);
  int longueurchaine( char* source, char* end);
  char* missing_word(char* chainecomplete, char* chainedecoupee);
  list_t lex( char *lex_defs, char *fichiersource );
  char ** lecture_source( char * fichiersource);
  list_t list_rules( char* def_rules);
  lexem_t lexem_peek(list_t lexems);
  int lexem_type(lexem_t lex,char *type);
  int lexem_type_strict(lexem_t lex , char *type );
  lexem_t lexem_advance(list_t *lexems);
  int next_lexem_is(list_t lexems, char *type);
  int next_lexem_is_strict(list_t lexems, char *type);
  int is_lexem_skippable(lexem_t c);
  void print_parse_error(char* msg, list_t *lexems);
  int list_lexem_print(list_t lexem);


#ifdef __cplusplus
}
#endif

#endif /* _LEXEM_H_ */
