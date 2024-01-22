#include "pyas/pyobj.h"
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
#include <pyas/fill_pyobj.h>



pyobj_t* pyobj_change_type(pyobj_t *a,unsigned int r){
  (*a)->type=r;
  return a;
}


pyobj_t pyobj_t_alloc(void){
  pyobj_t a=calloc(1,sizeof(struct pyobj));
  return a;
}

int parse_eol(list_t *lexems, pyobj_t *cb){
    printf("Parsing the end of a line\n");
    if (!list_empty(*lexems) && 0==next_lexem_is_strict(*lexems,"blank::skip") ){
      (*lexems)=list_next(*lexems);
    }
    if ( !list_empty(*lexems) && 0==next_lexem_is_strict(*lexems,"comment::skip") ){
      (*lexems)=list_next(*lexems);
    }
    if ( list_empty(*lexems) || 0 != next_lexem_is_strict(*lexems,"newline::skip") ){
      return -1;
      }
      else {
        (*lexems)=list_next(*lexems);
      }
    if ( !list_empty(*lexems) && 0==next_lexem_is_strict(*lexems,"blank::skip")){
        (*lexems)=list_next(*lexems);
      }
    return 0;
}

int parse(list_t lexem,py_codeblock cdb){

}
int parse_insn(list_t *lexems, pyobj_t *cb){
  (*cb)->type=(unsigned int)'s';
  printf("Parsing an instruction\n");
  if (next_lexem_is_strict(*lexems,"insn::0")){
    (*lexems)=list_next(*lexems);
    char* phrase=malloc(sizeof(char*));
    (*cb)->py.string.length=strlen( lexem_value(list_first(*lexems)) );
    strcpy((*cb)->py.string.buffer,lexem_value(list_first(*lexems)));
    return 0;
  }
  if (next_lexem_is_strict(*lexems,"insn::1")){
    (*lexems)=list_next(*lexems);
    if(next_lexem_is(*lexems,"integer::dec") || next_lexem_is(*lexems,"symbol")){
      (*lexems)=list_next(*lexems);
      char* phrase=malloc(sizeof(char*));
      (*cb)->py.string.length=strlen( lexem_value(list_first(*lexems)) );
      strcpy((*cb)->py.string.buffer,lexem_value(list_first(*lexems)));
      return 0;
    }
    else return -1;
  }
  else {
    return -1;
  }
}


int parse_source_lineno(list_t *lexems, pyobj_t *cb){
  printf("Parsing a source line number\n");
  if(next_lexem_is_strict((*lexems),"directs::line") ){
    (*lexems)=list_next(*lexems);
    if(next_lexem_is_strict(*lexems,"blank::skip")){
      (*lexems)=list_next(*lexems);
      if(next_lexem_is_strict(*lexems,"integer::dec")){
        (*cb)->type=(unsigned int)'i';
        (*lexems)=list_next(*lexems);
        (*cb)->py.number.integer=(int)lexem_value(list_first(*lexems));
        return 0;
      }
    }
  }
  else return -1;
}


int parse_label(list_t *lexems, pyobj_t *cb){
  printf("Parsing a label\n");
  if(next_lexem_is_strict(*lexems,"symbol")){
    (*lexems)=list_next(*lexems);
    if(next_lexem_is_strict(*lexems,"blank::skip")){
      (*lexems)=list_next(*lexems);
      if(next_lexem_is_strict(*lexems,"colon")){
        (*lexems)=list_next(*lexems);
        return 0;
      }
    }
  }
  else {
    return -1;
  }
}

int parse_assembly_line(list_t *lexems, pyobj_t *cb){
  printf("Parsing an assembly line\n");
  if( 0==parse_insn(*lexems,cb) || 0==parse_label(*lexems,cb) || 0==parse_source_lineno(*lexems,cb) ){
    return 0;
  }
  else {
    return -1;
  }
}


int parse_code(list_t *lexems, pyobj_t *cb){
  if(next_lexem_is_strict(*lexems,"dir::text")){
    (*lexems)=list_next(*lexems);
    if (0==parse_eol(*lexems,cb)){
      while(0==parse_assembly_line(*lexems,cb)){
        if (0==parse_eol(*lexems,cb)){
          return 0;
        }
      }
      return 0;
    }
  }
  else{
    return -1;
  }
}

int parse_cellvars(list_t *lexems, pyobj_t *cb){
  (*cb)->type=(unsigned int)'(';
  printf("Parsing cellvars\n");
  if (0==next_lexem_is_strict(*lexems,"directs::cellvars")){
    (*lexems)=list_next(*lexems);
    list_t q=list_new();
    if (0==parse_eol(lexems,cb)){
      while( !list_empty(*lexems)  && 0==next_lexem_is_strict(*lexems,"string")) {
        pyobj_t s=pyobj_t_alloc();
        s->refcount=0;
        lexem_t a=list_first(*lexems);
        s=fill_string(s,lexem_value(a));
        q=list_add_first(q,s);
        *lexems=list_next(*lexems);
        if (-1==parse_eol(lexems,cb)) {return -1; }
      }
      int j=list_to_tuple(q,cb);
      printf("print du cellvar dans la parse\n");
      pyobj_print(*cb);
      printf("fait\n");
    }
    else {return -1;}
  }
  else {return -1;}
  return 0;
}


int parse_freevars(list_t *lexems, pyobj_t *cb){
  (*cb)->type=(unsigned int)'(';
  printf("Parsing freevars\n");
  if (0==next_lexem_is_strict(*lexems,"directs::freevars")){
    (*lexems)=list_next(*lexems);
    list_t q=list_new();
    if (0==parse_eol(lexems,cb)){
      while( !list_empty(*lexems)  && 0==next_lexem_is_strict(*lexems,"string")) {
        pyobj_t s=pyobj_t_alloc();
        s->refcount=0;
        lexem_t a=list_first(*lexems);
        s=fill_string(s,lexem_value(a));
        q=list_add_first(q,s);
        *lexems=list_next(*lexems);
        if (-1==parse_eol(lexems,cb)) {return -1; }
      }
      int j=list_to_tuple(q,cb);
      printf("print du freevar dans la parse\n");
      pyobj_print(*cb);
      printf("fait\n");
    }
    else {return -1;}
  }
  else {return -1;}
  return 0;
}

int parse_varnames(list_t *lexems, pyobj_t *cb){
  (*cb)->type=(unsigned int)'(';
  printf("Parsing variable names\n");
  if (next_lexem_is_strict(*lexems,"directs::varnames")){
    (*lexems)=list_next(*lexems);
    list_t q=list_new();
    if (0==parse_eol(lexems,cb)){
      while( !list_empty(*lexems)  && next_lexem_is_strict(*lexems,"string")) {
        pyobj_t s=pyobj_t_alloc();
        s->refcount=0;
        lexem_t a=list_first(*lexems);
        s=fill_string(s,lexem_value(a));
        pyobj_print(s);
        q=list_add_first(q,s);
        *lexems=list_next(*lexems);
        if (-1==parse_eol(lexems,cb)) {return -1; }
      }
      int j=list_to_tuple(q,cb);
      // pyobj_print(*cb);
    }
    else {return -1;}
  }
  else {return -1;}
  return 0;
}



int create_interned_strings(list_t *lexems, pyobj_t *interned_string_tuple){
  unsigned int typ =(unsigned int) 'R';
  pyobj_t *a;
  *a=pyobj_t_alloc();
  // uint32_t i=0;
  (*interned_string_tuple)->type=typ;
  (*interned_string_tuple)->refcount=0;
  do{
    a=fill_string(*a,lexem_value(lexem_peek(*lexems)));
    // i++;
    interned_string_tuple=fill_tab(*interned_string_tuple,*a,strlen( lexem_value(lexem_peek(*lexems)) ));
  }
  while(0==parse_interned_strings(lexems,interned_string_tuple));
  free(*a);
  return 1;
}
int codeblock_alloc(py_codeblock *cdb){
  (*cdb).header.arg_count,(*cdb).header.local_count,(*cdb).header.stack_size,(*cdb).header.flags=0;
  (*cdb).parent=pyobj_alloc();
  (*cdb).binary.header.magic,(*cdb).binary.header.source_size=0;
  time(&((*cdb).binary.header.timestamp));
  (*cdb).binary.content.interned,(*cdb).binary.content.bytecode,(*cdb).binary.content.consts,(*cdb).binary.content.names,(*cdb).binary.content.varnames,(*cdb).binary.content.freevars,(*cdb).binary.content.cellvars=pyobj_alloc();
  (*cdb).binary.trailer.filename,(*cdb).binary.trailer.name,(*cdb).binary.trailer.lnotab=pyobj_alloc();
  return 1;
}

int list_to_tuple(list_t l,pyobj_t *cb){
  if (list_empty(l)){
    printf("Liste vide en argument de list_to_tuple\n");
    return NULL;
  }
  list_t k;
  int i=0;
  (*cb)->py.list.size=list_length(l);
  ((*cb)->py.list.value)=calloc(list_length(l),sizeof(pyobj_t));
  // printf("On vérifie la taille de la liste %d\n",list_length(l));
  for (k=l;!list_empty(k);k=list_next(k)){
    ((*cb)->py.list.value)[i]=list_first(k);
    i++;
    }
  pyobj_print(*cb);
  return 0;
}

int list_to_pylist(list_t l,pyobj_t *cb){
  if (list_empty(l)){
    printf("Liste vide en argument de list_to_tuple\n");
    return NULL;
  }
  list_t k;
  int i=0;
  (*cb)->type=(unsigned int)'[';
  (*cb)->py.list.size=list_length(l);
  ((*cb)->py.list.value)=calloc(list_length(l),sizeof(pyobj_t));
  // printf("On vérifie la taille de la liste %d\n",list_length(l));
  for (k=l;!list_empty(k);k=list_next(k)){
    ((*cb)->py.list.value)[i]=list_first(k);
    i++;
    }
  pyobj_print(*cb);
  return 0;
}


int parse_names(list_t *lexems, pyobj_t *cb){
  (*cb)->type=(unsigned int)'(';
  printf("Parsing names\n");
  if (0==next_lexem_is_strict(*lexems,"directs::names")){
    (*lexems)=list_next(*lexems);
    list_t q=list_new();
    if (0==parse_eol(lexems,cb)){
      while( !list_empty(*lexems)  && 0==next_lexem_is_strict(*lexems,"string")) {
        pyobj_t s=pyobj_t_alloc();
        s->refcount=0;
        lexem_t a=list_first(*lexems);
        s=fill_string(s,lexem_value(a));
        q=list_add_first(q,s);
        *lexems=list_next(*lexems);
        if (-1==parse_eol(lexems,cb)) {return -1; }
      }
      int j=list_to_tuple(q,cb);
    }
    else {return -1;}
  }
  else {return -1;}
  return 0;
}

int parse_list(list_t *lexems, pyobj_t *cb){
  printf("Parsing a list\n");
  if(next_lexem_is_strict(*lexems,"brack::left")){
    list_t l=list_new();
    while(next_lexem_is_strict(*lexems,"blank::skip")){
      pyobj_t elem=pyobj_alloc();
      (*lexems)=list_next(*lexems);
      parse_constant(lexems,&elem);
      l=list_add_first(l,elem);
    }
    if(next_lexem_is_strict(*lexems,"blank::skip")){
      (*lexems)=list_next(*lexems);
    }
    if(next_lexem_is_strict(*lexems,"brack::right")) {
      list_to_pylist(l,cb);
      return 0;}
    else return -1;
  }
  else return -1;
}

int parse_repofstring_eol(list_t *lexems, pyobj_t *cb){
  if (next_lexem_is_strict(*lexems,"string")){
    (*lexems)=list_next(*lexems);
    while(0==parse_eol(lexems,cb)){
    }
  }
  return 0;
}


int parse_tuple(list_t *lexems, pyobj_t *cb){
  printf("Parsing a tuple\n");
  if(0==next_lexem_is_strict(*lexems,"paren::left")){
    list_t l=list_new();
    while(0==next_lexem_is_strict(*lexems,"blank::skip")){
      pyobj_t elem=pyobj_alloc();
      (*lexems)=list_next(*lexems);
      parse_constant(lexems,&elem);
      l=list_add_first(l,elem);
    }
    if(0==next_lexem_is_strict(*lexems,"blank::skip")){
      (*lexems)=list_next(*lexems);
    }
    if(0==next_lexem_is_strict(*lexems,"paren::right")) {
      list_to_tuple(l,cb);
      return 0;}
    else printf("No left parenthesis\n");return -1;
  }
  else printf("No left parenthesis, there isn't any tuple\n" );return -1;
}

int parse_constant(list_t *lexems, pyobj_t *cb){
  if (list_empty(*lexems)){
    return -1;
  }
  printf("Parsing a constant\n");
  if( 0==next_lexem_is_strict(*lexems,"number::uint") ){
    (*cb)->type=(unsigned int)'i';
    (*cb)->py.number.integer=(uint32_t)strtol(lexem_value(list_first(*lexems)),NULL,10);
    (*lexems)=list_next(*lexems);
    return 0;
  }
  if(0==next_lexem_is_strict(*lexems,"parent::left")){
    if(0==parse_tuple(lexems,cb)) return 0;
    else return -1;
  }
  if( 0==next_lexem_is_strict(*lexems,"number::hexa") ){
    (*cb)->type=(unsigned int)'i';
    (*cb)->py.number.integer=lexem_value(list_first(lexems));
    (*lexems)=list_next(*lexems);
    return 0;
  }
  if( 0==next_lexem_is_strict(*lexems,"number::float") ){
    (*cb)->type=(unsigned int)'f';
    (*cb)->py.number.real=strtod(lexem_value(list_first(lexems)),NULL);
    (*lexems)=list_next(*lexems);
    return 0;
  }
  if( 0==next_lexem_is_strict(*lexems,"string") ){
    (*cb)->type=(unsigned int)'s';
    (*cb)->py.string.length=strlen(lexem_value(list_first(lexems)));
    (*cb)->py.string.buffer=calloc(strlen(lexem_value(list_first(lexems))) , sizeof(char));
    strcpy( (*cb)->py.string.buffer,lexem_value(list_first(lexems)));
    (*lexems)=list_next(*lexems);
    return 0;
  }
  if( 0==next_lexem_is_strict(*lexems,"pycst::True") ){
    (*cb)->type=(unsigned int)'T';
    (*cb)->py.string.length=strlen(lexem_value(list_first(lexems)));
    (*cb)->py.string.buffer=calloc(strlen(lexem_value(list_first(lexems))) , sizeof(char));
    strcpy( (*cb)->py.string.buffer,lexem_value(list_first(lexems)));
    (*lexems)=list_next(*lexems);
    return 0;
  }
  if( 0==next_lexem_is_strict(*lexems,"pycst::False") ){
    (*cb)->type=(unsigned int)'F';
    (*cb)->py.string.length=strlen(lexem_value(list_first(lexems)));
    (*cb)->py.string.buffer=calloc(strlen(lexem_value(list_first(lexems))) , sizeof(char));
    strcpy( (*cb)->py.string.buffer,lexem_value(list_first(lexems)));
    (*lexems)=list_next(*lexems);
    return 0;
  }
  if( 0==next_lexem_is_strict(*lexems,"pycst::None") ){
    printf("aaaaa\n");
    (*cb)->type=(unsigned int)'N';
    (*cb)->py.string.length=strlen(lexem_value(list_first(lexems)));
    (*cb)->py.string.buffer=calloc(strlen(lexem_value(list_first(lexems))) , sizeof(char));
    strcpy( (*cb)->py.string.buffer,lexem_value(list_first(lexems)));
    (*lexems)=list_next(*lexems);
    return 0;
  }
  else return -1;
}

int parse_constants(list_t *lexems, pyobj_t *cb){
  printf("Parsing all the constants\n");
  if ( 0==next_lexem_is_strict(*lexems,"directs::consts")){
    (*lexems)=list_next(*lexems);
    if (0==parse_eol(lexems,cb)){
      while (0==parse_constant(lexems,cb)  && !list_empty(*lexems)){
        if (0==parse_eol(lexems,cb)&&!list_empty(*lexems)){
        }
      }
    }
    else return -1;
  }
  else return -1;
}

// int create_tuple_rse(list_t *lexems, pyobj_t *cb, py_codeblock *cb){
//
// }

int create_instruction(list_t *lexems, pyobj_t *string_for_insn){
  (*string_for_insn)->type=(unsigned int) 's';
  (*string_for_insn)->refcount=0;
  string_for_insn=fill_string(*string_for_insn,lexem_value(lexem_peek(*lexems)));
  return 1;
}


int parse_interned_strings(list_t *lexems, pyobj_t *cb){
  (*cb)->type=(unsigned int)'(';
  printf("Parsing interned strings\n");
  if (0==next_lexem_is_strict(*lexems,"directs::interned")){
    (*lexems)=list_next(*lexems);
    list_t q=list_new();
    if (0==parse_eol(lexems,cb)){
      while( !list_empty(*lexems)  && 0==next_lexem_is_strict(*lexems,"string")) {
        pyobj_t s=pyobj_t_alloc();
        s->refcount=0;
        lexem_t a=list_first(*lexems);
        s=fill_string(s,lexem_value(a));
        q=list_add_first(q,s);
        *lexems=list_next(*lexems);
        if (-1==parse_eol(lexems,cb)) {return -1; }
      }
      int j=list_to_tuple(q,cb);
    }
    else {return -1;}
  }
  else {return -1;}
  return 0;
}


int parse_set_version_pyvm(list_t *lexems, pyobj_t *cb){
  printf("Parsing the version of the Python VM\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "directs::set")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "version_pyvm")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "integer::dec")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*((*cb)->py.codeblock)).version_pyvm=(int) lexem_value(list_first(lexems));
  parse_eol(lexems,cb);
  return 0;
}

int parse_set_flags(list_t *lexems, pyobj_t *cb){
  printf("Parsing set_flags\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "dir::set")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "flags")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "integer::hex")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*((*cb)->py.codeblock)).header.flags = (uint32_t) lexem_value(list_first(*lexems));
  parse_eol(lexems,cb);
  return 0;
}

int parse_set_filename(list_t *lexems, pyobj_t *cb){
  printf("Parsing set_filename\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "dir::set")){
    return -1;
  }
  // pyobj_t obj;
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "filename")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "string")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*((*cb)->py.codeblock)).binary.trailer.filename = fill_string((*((*cb)->py.codeblock)).binary.trailer.filename,lexem_value(list_first(*lexems)) );
  parse_eol(lexems,cb);
  return 0;
}

int parse_set_names(list_t *lexems, pyobj_t *cb){
    printf("Parsing set_names\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "dir::set")){
    return -1;
  }
  // pyobj_t obj;
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "name")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "string")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*((*cb)->py.codeblock)).binary.trailer.name = fill_string((*((*cb)->py.codeblock)).binary.trailer.name,lexem_value(list_first(*lexems)) );
  parse_eol(lexems,cb);
  return 0;
}


int parse_set_source_size(list_t *lexems, pyobj_t *cb){
  printf("Parsing set_source_size\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "dir::set")){
    return -1;
  }
  // pyobj_t obj;
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "source_size")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "integer::dec")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*((*cb)->py.codeblock)).binary.header.source_size=fill_int64((*((*cb)->py.codeblock)).binary.trailer.filename,lexem_value(list_first(*lexems)) );
  parse_eol(lexems,cb);
  return 0;
}


int parse_set_stack_size(list_t *lexems, pyobj_t *cb){
  printf("Parsing stack size\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "dir::set")){
    return -1;
  }
  // pyobj_t obj;
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "stack_size")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "integer::dec")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*((*cb)->py.codeblock)).header.stack_size = fill_int32((*((*cb)->py.codeblock)).header.stack_size,lexem_value(list_first(*lexems)) );
  parse_eol(lexems,cb);
  return 0;
}

int parse_set_arg_count(list_t *lexems, pyobj_t *cb){
  printf("Parsing set arg count\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "dir::set")){
    return -1;
  }
  // pyobj_t obj;
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "arg_count")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "integer::dec")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*cb)->type=(unsigned int)'i';
  (*((*cb)->py.codeblock)).header.arg_count = fill_int32((*((*cb)->py.codeblock)).header.arg_count,lexem_value(list_first(*lexems)) );
  parse_eol(lexems,cb);
  return 0;
}

int parse_set_kwonly_arg_count(list_t *lexems, pyobj_t *cb){
  printf("Parsing kwonly arg count\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "directs::set")){
    return -1;
  }
  // pyobj_t obj;
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "kwonly_arg_count")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "integer::dec")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*cb)->type=(unsigned int)'i';
  (*cb)->py.number.integer=lexem_value(list_first(lexems));
  parse_eol(lexems,cb);
  return 0;
}

int parse_set_posonly_arg_count(list_t *lexems, pyobj_t *cb){
  printf("Parsing posonly arg count\n");
  if (!next_lexem_is_strict(list_first((*lexems)), "directs::set")){
    return -1;
  }
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  (*lexems) = list_next(*lexems);
  if (strcmp(lexem_value((list_first(*lexems))), "posnoly_arg_count")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  if (!next_lexem_is_strict(list_first((*lexems)), "blank::skip")){
    return -1;
  }
  //Fonction remplir
  if (!next_lexem_is_strict(list_first((*lexems)), "integer::dec")){
    return -1;
  }
  //Fonction remplir
  (*lexems) = list_next(*lexems);
  (*cb)->type=(unsigned int)'i';
  (*cb)->py.number.integer=lexem_value(list_first(lexems));
  parse_eol(lexems,cb);
  return 0;
}


int parse_set_directives(list_t *lexems, pyobj_t *cb){
  printf("Parsing set directives\n");
  // pyobj_t obj;
  if (-1 == parse_set_version_pyvm(lexems,cb)){
    return -1;
  }

  if (-1 == parse_set_flags(lexems,cb)) return -1;

  if (-1 == parse_set_filename(lexems,cb)) return -1;
  if (-1 == parse_set_names(lexems,cb)) return -1;
  if( 0 == parse_set_source_size(lexems,cb)){
  }
  if (-1 == parse_set_stack_size(lexems,cb)) return -1;
  if (-1 == parse_set_arg_count(lexems,cb)) return -1;
  if ( 0 == parse_set_kwonly_arg_count(lexems,cb)){

  }
  if ( 0 == parse_set_posonly_arg_count(lexems,cb)){

  }
  return 0;
}

int parse_prologue(list_t *lexems, pyobj_t *cb){
  printf("Parsing prologue\n");
  // pyobj_t obj;
  if (-1 == parse_set_directives(lexems,cb)){
    return -1;
  }
  if (-1 == parse_interned_strings( lexems, & ((*((*cb)->py.codeblock)).binary.content.interned ) )){
    return -1;
  }
  if (-1 == parse_constants(lexems, & ((*((*cb)->py.codeblock)).binary.content.consts ) )){
    return -1;
  }
  if (-1 == parse_names(lexems,& ((*((*cb)->py.codeblock)).binary.content.names ) )){
    return -1;
  }

  if (0 == parse_varnames(lexems, & ((*((*cb)->py.codeblock)).binary.content.interned ) ) ){
    }
    if (0 == parse_freevars(lexems, & ((*((*cb)->py.codeblock)).binary.content.freevars ) )){

    }
    if ( 0== parse_cellvars(lexems, & ((*((*cb)->py.codeblock)).binary.content.cellvars ) )){
    }
  return 0;
}


int parse_pys(list_t *lexems, pyobj_t *cb){
  printf("Parsing the pys file\n");
  while(parse_eol(lexems,cb) == 0 ){

  }
  // pyobj_t obj;
  if (-1 == parse_prologue(lexems,cb)){
    return -1;
  }
  if (-1== parse_code(lexems,cb)){
    return -1;
  }
  return 0;
}
