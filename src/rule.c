#include <assert.h>
#include <stdio.h>
#include <stdlib.h> /* NULL */
#include <string.h>
#include "../include/pyas/rule.h"

struct rule {
  char* name;
  char* value;
  char* op_code;
};


rule_t rule_initialize(rule_t a){
    a = malloc(sizeof(*a));
    if (NULL == a){
        printf("Erreur d'allocation de la règle");
        exit(EXIT_FAILURE);
    }
    a->name = malloc(50*sizeof(char));
    a->value = malloc(50*sizeof(char));
    a->op_code = malloc(13*sizeof(char));
    return a;
}

int     rule_print( void *_rule ) {
    rule_t rule = _rule;
    return printf( "[%s:%s]",
        rule->name,
        rule->value);
}

rule_t rule_delete( rule_t a){
    free(a->name);
    free(a->value);
    free(a);
}

