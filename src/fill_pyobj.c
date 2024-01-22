#include "pyas/pyobj.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <pyas/lexem.h>
#include <pyas/list.h>
#include <pyas/queue.h>

#include <pyas/rule.h>
#include <pyas/charset.h>
#include <pyas/chargroup.h>
#include <pyas/arithexpr.h>
#include <pyas/relat.h>



pyobj_t pyobj_alloc() {
    pyobj_t obj = (pyobj_t) calloc(1,sizeof(struct pyobj));
    return obj;
}

pyobj_t fill_refcount (pyobj_t obj) {
    (*obj).refcount = 0;
    return (obj);
}

pyobj_t fill_string (pyobj_t obj,char* str){
    (obj)->type = (pyobj_type) 's';
    (obj)->py.string.buffer = str;
    (obj)->py.string.length = strlen(str);
    return obj;
}

pyobj_t fill_tab (pyobj_t obj,pyobj_t* a,int size){
    (*obj).type = (unsigned int) '[';
    (*obj).py.list.value = a;
    (*obj).py.list.size = size;
    return obj;
}

pyobj_t fill_tuple (pyobj_t obj,pyobj_t* a,int size){
    (*obj).type = (unsigned int) '(';
    (*obj).py.list.value = a;
    (*obj).py.list.size = size;
    return obj;
}

pyobj_t fill_int32 (pyobj_t obj,uint32_t a){
    (*obj).type = (unsigned int) 'i';
    (*obj).py.number.integer = a;
    return obj;
}

pyobj_t fill_int64 (pyobj_t obj,uint32_t a){
    (*obj).type = (unsigned int) 'I';
    (*obj).py.number.integer64 = a;
    return obj;
}

pyobj_t fill_double (pyobj_t obj,double a){
    (*obj).type = (unsigned int) 'f';
    (*obj).py.number.real = a;
    return obj;
}

py_codeblock* fill_codeblock_version (py_codeblock* codeblock, uint16_t version_pyvm) {
    (*codeblock).version_pyvm = version_pyvm;
    return (codeblock);
}

py_codeblock* fill_codeblock_header (py_codeblock* codeblock, uint32_t arg_count, uint32_t local_count, uint32_t stack_size, uint32_t flags ) {
    (codeblock)->header.arg_count = arg_count;
    (codeblock)->header.local_count = local_count;
    (codeblock)->header.stack_size = stack_size;
    (codeblock)->header.flags = flags;
    (codeblock)->parent = NULL;
    return (codeblock);
}

py_codeblock* fill_codeblock_binary_header (py_codeblock* codeblock,  uint16_t magic, time_t timestamp, uint32_t source_size) {
    (codeblock)->binary.header.magic = magic;
    (codeblock)->binary.header.timestamp = timestamp;
    (codeblock)->binary.header.source_size = source_size;
    return (codeblock);
}

py_codeblock* fill_codeblock_binary_content (py_codeblock* codeblock,  pyobj_t interned, pyobj_t bytecode, pyobj_t consts, pyobj_t names, pyobj_t varnames, pyobj_t freevars, pyobj_t cellvars ) {
    (*codeblock).binary.content.interned = interned;
    (*codeblock).binary.content.bytecode = bytecode;
    (*codeblock).binary.content.consts = consts;
     (*codeblock).binary.content.names = names;
    (*codeblock).binary.content.varnames = varnames;
     (*codeblock).binary.content.freevars = freevars;
    (*codeblock).binary.content.cellvars = cellvars;
    return (codeblock);
}

py_codeblock* fill_codeblock_binary_trailer (py_codeblock* codeblock,  pyobj_t filename,pyobj_t name,uint32_t firstlineno, pyobj_t lnotab) {
    (codeblock)->binary.trailer.filename = filename;
    (codeblock)->binary.trailer.name = name;
    (codeblock)->binary.trailer.firstlineno = firstlineno;
    (codeblock)->binary.trailer.lnotab = lnotab;
    return (codeblock);
}

pyobj_t* fill_codeblock_in_pyobj (pyobj_t obj, py_codeblock* codeblock) {
    (*obj).type = (unsigned int) 'b';
    (*obj).py.codeblock = codeblock;
    return obj;
}

pyobj_t pyobj_copy(pyobj_t dest, pyobj_t source){
  int i=0;
  int k=0;
  switch (source->type) {
    case(115):
        dest->type=115;
        dest->py.string.length=source->py.string.length;
        dest->py.string.buffer = malloc(sizeof(char)*source->py.string.length);
        strcpy(dest->py.string.buffer,source->py.string.buffer);
        break;
    case(105):
        dest->type=105;
        dest->py.number.integer=source->py.number.integer;
        break;
    case(73):
        dest->type=73;
        dest->py.number.integer64=source->py.number.integer64;
        break;
    case(102):
        dest->type=102;
        dest->py.number.real=source->py.number.real;
        break;
    case(91):
        dest->type=91;
        dest->py.list.size=source->py.list.size;
        dest->py.list.value = malloc(sizeof(pyobj_t)*source->py.list.size);
        for (k=0;k<dest->py.list.size;k++) {
            pyobj_copy(*(dest->py.list.value+k),*(source->py.list.value+k));
        }
    case(40):
        dest->type=40;
        dest->py.list.size=source->py.list.size;
        dest->py.list.value = malloc(sizeof(pyobj_t)*source->py.list.size);
        for (k=0;k<dest->py.list.size;k++) {
            pyobj_copy(*(dest->py.list.value+k),*(source->py.list.value+k));
        }
        break;
    default:
            printf("erreur dans le pyobj_copy\n");
            return NULL;
  }
  return NULL;
}

int pyobj_print (pyobj_t obj) {
    int i=0;
    switch (obj->type) {
      case ( (unsigned int) '(' ):
      for (i=0;i< (obj->py.list.size) ;i++) {
          pyobj_print (*(obj->py.list.value+i));
      }
      break;
        case (115):
            printf("%s\n",(obj)->py.string.buffer);
            break;
        case (105):
            printf("%d\n",(obj)->py.number.integer);
            break;
        case (73):
            printf("%d\n",(obj)->py.number.integer64);
            break;
        case (102):
            printf("%lf\n",(obj)->py.number.real);
            break;
        case (91||40):
            for (i=0;i<obj->py.list.size;i++) {
                pyobj_print (*(obj->py.list.value+i));
            }
            break;
        case (98):
            codeblock_print(*(obj->py.codeblock));
            break;
        default:
            printf("erreur dans le pyobj_print\n");
            return 0;
    }
    return 1;
}


pyobj_t pyobj_delete (pyobj_t obj) {
    int i = 0;
    switch ((*obj).type) {
        case (115):
            //free((*obj).py.string.buffer);
            free(obj);
            break;
        case (105):
            free(obj);
            break;
        case (73):
            free(obj);
            break;
        case (102):
            free(obj);
            break;
        case (91||40):
            for (i=0;i<obj->py.list.size;i++) {
            pyobj_delete(*(obj->py.list.value+i));
            }
            free(obj->py.list.value);
            free(obj);
            break;
        case (98):
            pyobj_delete((*(obj->py.codeblock)).binary.content.interned);
            pyobj_delete((*(obj->py.codeblock)).binary.content.consts);
            pyobj_delete((*(obj->py.codeblock)).binary.content.names);
            pyobj_delete((*(obj->py.codeblock)).binary.content.varnames);
            pyobj_delete((*(obj->py.codeblock)).binary.content.freevars);
            pyobj_delete((*(obj->py.codeblock)).binary.content.cellvars);
            pyobj_delete((*(obj->py.codeblock)).binary.trailer.filename);
            pyobj_delete((*(obj->py.codeblock)).binary.trailer.name);
            free(obj->py.codeblock);
            free(obj);
            break;
        default:
            printf("erreur dans le pyobj_delete\n");
    }
    return NULL;
}


char* fill_bytecode_prolog (pyobj_t bytecode, py_codeblock* codeblock) {
    bytecode->type = 73;
    bytecode->py.string.length = 10000;
    bytecode->py.string.buffer = malloc(bytecode->py.string.length*sizeof(char));
    int k=0;
    int i =0;
    uint8_t code = 99;

    //uint16_t *


    char* bcode = bytecode->py.string.buffer;

    memcpy(bcode,&((codeblock)->version_pyvm),sizeof((codeblock)->version_pyvm));
    //printf("%d %d %d %d \n",k, (unsigned char)bcode[0],(unsigned char)bcode[1],(codeblock)->version_pyvm);
    bcode += sizeof(((codeblock)->version_pyvm));

    memcpy(bcode,&((codeblock)->binary.header.magic),sizeof((codeblock)->binary.header.magic));
    bcode+= sizeof((codeblock)->binary.header.magic);

    memcpy(bcode,&((codeblock)->binary.header.timestamp),sizeof(uint32_t));
    bcode+= sizeof(uint32_t);

    memcpy(bcode,&(code),sizeof(code));
    bcode+= sizeof(code);

    memcpy(bcode,&((codeblock)->header.arg_count),sizeof((codeblock)->header.arg_count));
    bcode+= sizeof((codeblock)->header.arg_count);

    memcpy(bcode,&((codeblock)->header.local_count),sizeof((codeblock)->header.local_count));
    bcode+= sizeof((codeblock)->header.local_count);

    memcpy(bcode,&((codeblock)->header.stack_size),sizeof((codeblock)->header.stack_size));
    bcode+= sizeof((codeblock)->header.stack_size);

    memcpy(bcode,&((codeblock)->header.flags),sizeof((codeblock)->header.flags));
    bcode+= sizeof((codeblock)->header.flags);



    // printf("taille de version: %d\n",sizeof(((codeblock)->version_pyvm)));
    // printf("taille de magic: %d\n",sizeof(uint32_t));
    // printf("taille de timescamp: %d\n",sizeof((codeblock)->binary.header.timestamp));

    //printf("%x", bcode[i]);

    FILE* pyc_file = fopen("exemple.pyc","wb");

    for (i=0;i<25;i+=1) {
        fwrite(&(bytecode->py.string.buffer[i]),2,1,pyc_file);
        printf("%02x", (uint8_t)bytecode->py.string.buffer[i]);
        printf(" ");
    }
    printf("\n");
    return bytecode->py.string.buffer;
    fclose(pyc_file);
}

int codeblock_print (py_codeblock cb){
  printf("La version python de la vm:  %d\n",cb.version_pyvm);
  printf("Le nombre d'argument est: %d\n",cb.header.arg_count);
  printf("Le local_count est: %d\n",cb.header.local_count);
  printf("La taille de la pile est: %d\n",cb.header.stack_size);
  printf("Les flags sont: %d\n",cb.header.flags);
  printf("Le magic est: %d\n",cb.binary.header.magic);
  printf("La dernière date de modification du fichier est: %d\n",cb.binary.header.timestamp);
  printf("La taille du fichier source est: %d\n", cb.binary.header.source_size);

  // printf("Les éléments internés:\n");
  // pyobj_print(cb.binary.content.interned);
  printf("Les lexems reconnus dans le .txt:\n");
  list_lexem_print(cb.binary.content.bytecode);
  // printf("Les constantes:\n");
  // pyobj_print(cb.binary.content.consts);
  // printf("Les noms des constantes:\n");
  // pyobj_print(cb.binary.content.names);
  // printf("Les nom des variables:\n");
  // pyobj_print(cb.binary.content.varnames);
  // printf("Les freevars sont:\n");
  // pyobj_print(cb.binary.content.freevars);
  printf("Les cellvars sont:\n");
  pyobj_print(cb.binary.content.cellvars);

  printf("Le nom du fichier est:\n");
  pyobj_print(cb.binary.trailer.filename);
  printf("Le nom est:\n");
  pyobj_print(cb.binary.trailer.name);
  printf("Le numéro de la première ligne est: %d\n",cb.binary.trailer.firstlineno);
  printf("lnotab est:\n",cb.binary.trailer.lnotab);
  pyobj_print(cb.binary.trailer.lnotab);
}
