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

int main(){
  printf("-----------------------------------\n");
  printf("Tests basiques et manipulations sur les pyobj\n");
  // pyobj_t p;
  // (p)=pyobj_alloc();
  // (p)->type=0;
  // printf("Le type de p est %d",(p)->type);
  printf("-----------------------------------\n");printf("-----------------------------------\n");printf("-----------------------------------\n");
  lexem_t a = lexem_new("directs::names", ".names",1,0,"");
  lexem_t b = lexem_new("string", "a",2,5,"");
  lexem_t c = lexem_new("string", "b",3,5,"");
  lexem_t d = lexem_new("string", "<module>",4,5,"");
  lexem_t blank = lexem_new("blank::skip", " ", 11, 6, "vide");
  lexem_t f = lexem_new( "newline::skip","\n",1 ,10,"");
  lexem_t g = lexem_new( "newline::skip","\n",2 ,10,"");
  lexem_t h = lexem_new( "newline::skip","\n",3 ,10,"");
  lexem_t i = lexem_new( "newline::skip","\n",4 ,10,"");
  list_t l;
  l=list_new();
  l=list_add_first(l,i);
  (l)=list_add_first(l,d);
  l=list_add_first(l,h);
  (l)=list_add_first(l,c);
  l=list_add_first(l,g);
  (l)=list_add_first(l,b);
  l=list_add_first(l,f);
  l=list_add_first(l,blank);
  (l)=list_add_first(l,a);
  list_print(l,lexem_print);
  printf("\nOn a réussi à initialiser les lexems, on va tester la tuplisation des lexems\n");
  pyobj_t cb=pyobj_alloc();
  int j=parse_names(&l,&cb);
  // py_codeblock codeblock=py_codeblock_alloc();
  // codeblock.content.names=(*cb);

  printf("On attend 0, on récup %d\n",j);
  pyobj_print(cb);
  printf("On a le type %d alors que %d \n",cb->type,(unsigned int)'(');
  // printf("Quelques manipulations sur les codeblock\n");
  // // pyobj_print(am);
  // py_codeblock z;
  // codeblock_alloc(z);
  return 1;
}
