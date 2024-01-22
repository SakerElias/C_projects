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
#include <stdint.h>

typedef unsigned int pyobj_type;
struct pyobj_t;
typedef struct pyobj *pyobj_t;


typedef struct {
  uint16_t version_pyvm;
  struct {
    uint32_t arg_count;
    uint32_t local_count;
    uint32_t stack_size;
    uint32_t flags;
  } header;
  pyobj_t parent;
  struct {
    struct {
      uint16_t magic;
      time_t timestamp;
      uint32_t source_size;
    }   header;
    struct {
      pyobj_t interned;
      pyobj_t bytecode;
      pyobj_t consts;
      pyobj_t names;
      pyobj_t varnames;
      pyobj_t freevars;
      pyobj_t cellvars;
    }   content;
    struct {
      pyobj_t filename;
      pyobj_t name;
      uint32_t firstlineno;
      pyobj_t lnotab;
    }   trailer;
  }   binary;
}   py_codeblock;



struct pyobj {

  pyobj_type type;
  unsigned int refcount;

  union {
    struct {
      pyobj_t *value;
      int32_t size;
    } list;
    struct {
      char *buffer;
      int length;
    } string;

    py_codeblock *codeblock;

    union {
      int32_t integer;
      int64_t integer64;
      double real;
      struct{
        double real;
        double imag;
      } complex;
    } number;
  } py;
};





  pyobj_t* pyobj_change_type(pyobj_t *a,unsigned int r);
  pyobj_t pyobj_t_alloc(void);
  int parse_eol(list_t *lexems,pyobj_t *cb);
  int parse_insn(list_t *lexems, pyobj_t *cb);
  int parse_source_lineno(list_t *lexems, pyobj_t *cb);
  int parse_label(list_t *lexems, pyobj_t *cb);
  int parse_assembly_line(list_t *lexems, pyobj_t *cb);
  int parse_code(list_t *lexems, pyobj_t *cb);
  int parse_cellvars(list_t *lexems, pyobj_t *cb);
  int parse_freevars(list_t *lexems, pyobj_t *cb);
  int parse_varnames(list_t *lexems, pyobj_t *cb);
  int create_interned_strings(list_t *lexems, pyobj_t *interned_string_tuple);
  int codeblock_alloc(py_codeblock *cdb);
  int list_to_tuple(list_t l,pyobj_t *cb);
  int list_to_pylist(list_t l,pyobj_t *cb);
  int parse_names(list_t *lexems, pyobj_t *cb);
  int parse_list(list_t *lexems, pyobj_t *cb);
  int parse_repofstring_eol(list_t *lexems, pyobj_t *cb);
  int parse_tuple(list_t *lexems, pyobj_t *cb);
  int parse_constant(list_t *lexems, pyobj_t *cb);
  int parse_constants(list_t *lexems, pyobj_t *cb);
  int create_instruction(list_t *lexems, pyobj_t *string_for_insn);
  int parse_interned_strings(list_t *lexems, pyobj_t *cb);
  int parse_set_version_pyvm(list_t *lexems, pyobj_t *cb);
  int parse_set_flags(list_t *lexems, pyobj_t *cb);
  int parse_set_filename(list_t *lexems, pyobj_t *cb);
  int parse_set_names(list_t *lexems, pyobj_t *cb);
  int parse_set_source_size(list_t *lexems, pyobj_t *cb);
  int parse_set_stack_size(list_t *lexems, pyobj_t *cb);
  int parse_set_arg_count(list_t *lexems, pyobj_t *cb);
  int parse_set_kwonly_arg_count(list_t *lexems, pyobj_t *cb);
  int parse_set_posonly_arg_count(list_t *lexems, pyobj_t *cb);
  int parse_set_directives(list_t *lexems, pyobj_t *cb);
  int parse_prologue(list_t *lexems, pyobj_t *cb);
  int parse_pys(list_t *lexems, pyobj_t *cb);


#ifdef __cplusplus
}
#endif

#endif
