#ifndef _FILL_OBJ_H_
#define _FILL_OBJ_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <pyas/pyobj.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <time.h>

#include <pyas/lexem.h>
#include <pyas/list.h>
#include <pyas/queue.h>

#include <pyas/rule.h>
#include <pyas/charset.h>
#include <pyas/chargroup.h>
#include <pyas/arithexpr.h>
#include <pyas/relat.h>

pyobj_t pyobj_alloc();
pyobj_t fill_refcount (pyobj_t obj);
pyobj_t fill_string (pyobj_t obj,char* str);
pyobj_t fill_tab (pyobj_t obj,pyobj_t a,int size);
pyobj_t fill_tuple (pyobj_t obj,pyobj_t* a,int size);
pyobj_t fill_int32 (pyobj_t obj,int a);
pyobj_t fill_int64 (pyobj_t obj,int a);
pyobj_t fill_double (pyobj_t obj,double a);
py_codeblock* fill_codeblock_version (py_codeblock* codeblock, int version_pyvm);
py_codeblock* fill_codeblock_header (py_codeblock* codeblock, uint32_t arg_count, uint32_t local_count, uint32_t stack_size, uint32_t flags ) ;
py_codeblock* fill_codeblock_binary_header (py_codeblock* codeblock,  uint32_t magic, time_t timestamp, uint32_t source_size);
py_codeblock* fill_codeblock_binary_content (py_codeblock* codeblock,  pyobj_t interned, pyobj_t bytecode, pyobj_t consts, pyobj_t names, pyobj_t varnames, pyobj_t freevars, pyobj_t cellvars );
py_codeblock* fill_codeblock_binary_trailer (py_codeblock* codeblock,  pyobj_t filename,pyobj_t name,uint32_t firstlineno, pyobj_t lnotab);
pyobj_t fill_codeblock_in_pyobj (pyobj_t obj, py_codeblock* codeblock);
int pyobj_print (pyobj_t obj);
pyobj_t pyobj_delete (pyobj_t obj);
pyobj_t pyobj_copy(pyobj_t dest, pyobj_t source);
char* fill_bytecode_prolog (pyobj_t bytecode, py_codeblock* codeblock);
int codeblock_print (py_codeblock cb);


#ifdef __cplusplus
}
#endif
#endif
