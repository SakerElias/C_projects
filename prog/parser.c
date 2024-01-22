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

int main(int argc, char **argv){

    lexem_t lexem;
    lexem = list_new();

    if (argc != 3){
    printf("Erreur, veuillez entrer le fichier de definition et le code source");
    exit(EXIT_FAILURE);
  }
  lexem = lex(argv[1], argv[2]);
  pyobj_t cdb=pyobj_alloc();
  if (cdb==NULL){
    printf("L'allocation de l'objet de code Python a échoué\n");
    exit(EXIT_FAILURE);
  }
  list_print(lexem,lexem_print);
  py_codeblock z;
  codeblock_alloc(&z);
  cdb->py.codeblock=&z;
  parse_pys(&lexem,&cdb);

  //pyobj_print(parse(&lexem));

  //list_delete(lexem);
  exit(EXIT_SUCCESS);
  return 1;
  }
