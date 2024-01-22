
/**
 * @file lexem.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Jul  2 17:59:27 2021
 * @brief Lexems.
 *
 * Lexems.
 */

#define _POSIX_C_SOURCE 200809L /* strdup(3) */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>

#include <pyas/regexp.h>
#include <pyas/lexem.h>
#include <pyas/list.h>
#include <pyas/queue.h>
#include <pyas/rule.h>
#include <pyas/charset.h>
#include <pyas/chargroup.h>


struct lexem {
  char *type;
  char *value;
  int   line;
  int   column;
  char* op_code;
};


struct rule {
  char *name;
  char *value;
  char *op_code;
};



lexem_t lexem_new( char *type, char *value, int line, int column, char* op_code ) {
  lexem_t lex = calloc( 1, sizeof( *lex ) );

  assert( lex );

  if ( type  && *type  ) lex->type  = strdup( type );
  if ( value && *value ) lex->value = strdup( value );
  if ( op_code && *op_code ) lex->op_code = strdup( op_code );
  lex->line   = line;
  lex->column = column;

  return lex;
}

int     lexem_print( void *_lex ) {
  lexem_t lex = _lex;

  printf( "[%d:%d:%s] %s",
		 lex->line,
		 lex->column,
		 lex->type,
     lex->value);
     return 1;
}

int     lexem_delete( void *_lex ) {
  lexem_t lex = _lex;

  if ( lex ) {
    free( lex->type );
    free( lex->value );
    free( lex->op_code );
  }

  free( lex );

  return 0;
}

char*  lexem_value( lexem_t l ) {

  assert( l );

  return l->value;
}

char* lexem_op_code(lexem_t l){

  assert( l );

  return l->op_code;
}

int longueurchaine(char* source, char* ligne){                //va permettre de trouver le numéro de colonne en calculant le nombre d'élèments entre la source
  int k = 0;                                                //et la source après re_match
  assert(NULL != source);
  while ( ligne!= source ){
    ligne ++;
    k ++;
  }
  return k;
}


char **lecture_source(char* fichiersource){
  FILE * fichier;
  int N = 1000;
  char **source = calloc(1000,sizeof(*source));
  fichier = fopen(fichiersource, "r");
  int k = 0;
  if (NULL == fichier){
    printf("Erreur ouverture fichier");
    exit(EXIT_FAILURE);
  }
  source[0] = calloc(1000, sizeof(char));
  while (fgets(source[k], N, fichier) != NULL){
    k++;
    source[k] = calloc(1000,sizeof(char));
  }
  return source;
}

char* missing_word(char* chainecomplete, char* chainedecoupee){
  int k = 0;
  char* p_complete = chainecomplete;
  while(strcmp(p_complete, chainedecoupee)!=0){
    p_complete ++;
    k++;
  }
  char *mot = calloc(1000,sizeof(char));
  int i;
  for (i = 0; i<k; i++){
    mot[i] = chainecomplete[i];
  }
  return mot;
}


list_t list_rules(char* def_rules){
  int N = 256;                                //Taille maximum d'une ligne de def_rules en nombre de caractères
  long pos = 0;                                //Position dans le fichier
  queue_t q_liste = queue_new();
  rule_t curr_regle = NULL;
  char ligne[N];
  char *nom_exp;
  char *exp;
  char *op_code;
  nom_exp = calloc(50, sizeof(char));
  exp = calloc(50, sizeof(char));
  op_code = calloc(14, sizeof(char));
  FILE * regles;
  regles = fopen(def_rules, "r");
  curr_regle = rule_initialize(curr_regle);     //On ajoute manuellement certaines règles qui ne sont pas reconnues correctement par rules_read
  strcpy(curr_regle->name, "blank::skip");
  strcpy(curr_regle->value, "[ \t]+");
  strcpy(curr_regle->op_code, "vide");
  q_liste = enqueue(q_liste, curr_regle);
  curr_regle = rule_initialize(curr_regle);
  strcpy(curr_regle->name, "newline::skip");
  strcpy(curr_regle->value, "\n");
  strcpy(curr_regle->op_code, "vide");
  q_liste = enqueue(q_liste, curr_regle);
  curr_regle = rule_initialize(curr_regle);
  strcpy(curr_regle->name, "string");
  strcpy(curr_regle->value, "\".*\"");
  strcpy(curr_regle->op_code, "vide");
  q_liste = enqueue(q_liste, curr_regle);
  if (NULL == regles){
    printf("Erreur ouverture fichier");
    exit(EXIT_FAILURE);
  }
  do{
    pos = ftell(regles);
    fgets(ligne, N, regles);
    if ('\n' == ligne[0]) {;}
    else if ('#' == ligne[0]) {;}
    else{
      fseek(regles, pos, SEEK_SET);
      fscanf(regles, "%s %s %s", nom_exp, exp, op_code);
      curr_regle = rule_initialize(curr_regle);
      strcpy(curr_regle->name , nom_exp);
      strcpy(curr_regle->op_code , op_code);
      strcpy(curr_regle->value , exp);
      q_liste = enqueue(q_liste, curr_regle);
    }
  } while (NULL != fgets(ligne, N, regles));



  list_t liste = calloc(1,sizeof(*liste));
  liste = queue_to_list(q_liste);


  //free(q_liste);

  fclose(regles);


  return liste;
}

 list_t lex( char* lex_defs, char* fichiersource){
  char **source =  lecture_source(fichiersource);   //on importe le fichier source
  list_t rules = list_rules(lex_defs);          //on importe la liste de definition
  list_t lexems = list_new();                   
  printf("la ligne %d vaut %s", 16, source[16]);
  queue_t q_lexems = queue_new();
  char *curr_rule = calloc(1,sizeof(*curr_rule));
  list_t regle;                                      // Pour le parcours de la liste des règles
  lexem_t curr_lex;
  int num_colonne = 0;
  int k = 0;
  while(source[k] != NULL){
    if(source[k][0] == '#'){              //Si la ligne entiere est seulement un commentaire
      curr_lex = lexem_new("comment::skip", source[k], k+1, 0, "vide");
      q_lexems = enqueue(q_lexems, curr_lex);
      num_colonne = 0;
      k++;
      continue;
    }
    if(source[k][0] == '\n'){      //Si la ligne entiere est vide
      curr_lex = lexem_new("newline::skip", source[k], k+1, 0, "vide");
      q_lexems = enqueue(q_lexems, curr_lex);
      num_colonne = 0;
      k++;
      continue;
    }
    while (source[k][0] != '\0') {
      for (regle = rules ; !list_empty(regle); regle = regle->next){
        rule_t p = rule_initialize(p);
        p = list_first(regle);
        chargroup_t ligne_source = alloc_cgroup();
        int regexp_valide = re_read(p->value, &(ligne_source.file));  //Sert à tester si la regexp est valide mais aussi à la stocker dans le chargroup pour re_match plus tard
        char* ligne2 = source[k];
        int correspondance = re_match(&(ligne_source.file), source[k], &source[k],1);
        if(correspondance == 0 ){
          source[k] = ligne2;
        }
        if (regexp_valide == 1 && correspondance) {    //Si il y a matching entre la source et la regexp considérée (qui doit être valide)
          char *valeur_lex = missing_word(ligne2, source[k]);   //Permet de recuperer la 'valeur' du lexeme qui a matché et non toute la ligne
        //char *valeur_lex = "1";
          curr_lex = lexem_new(p->name, valeur_lex, k+1, num_colonne, p->op_code);
          q_lexems = enqueue(q_lexems, curr_lex);
          num_colonne += source[k]-ligne2;
          if (strcmp(p->value, "\n") ==0)  num_colonne = 0;      //S'il y a un retour à la ligne, on remet le numero de colonne à 0
          chargroup_delete(ligne_source, charset_delete_wrapper);
          break;
          }
        if (list_empty(regle->next)){
          int u = 0;
          while(source[k][u+1] != ' '){
            u++;
          }
          char* s = strtok(source[k], " \t\n");    //permet de recuperer uniquement le lexeme non reconnu et pas toute la ligne
          printf("[ ERROR ] Invalid lexem on line %d - column %d: '%s'\n",k+1 , num_colonne, s);
          exit(EXIT_FAILURE);
          }
      }
    }
    k++;
    num_colonne =0;
  }
  lexems = queue_to_list(q_lexems);
  free(curr_rule);
  free(source);
  free(rules);
  return lexems;
}

int is_lexem_skippable(lexem_t c){
  char *type = c->type;
  while (*type != '\0'){
    type ++;
  }
  if( (*(type -1) == 'p') && (*(type -2) == 'i') && (*(type -3) == 'k') && (*(type -4) == 's') ){
    return 1;
  }
  return 0;
}


lexem_t lexem_peek(list_t lexems){
  if(list_empty(lexems)){
    printf("Can't peek a lexem from an empty list!\n");
    exit(EXIT_FAILURE);
  }
  if((lexems!=NULL) && (!is_lexem_skippable(lexems->content))){
    return lexems->content;
  }
  else return lexem_peek(lexems->next);
}

void print_parse_error(char* msg, list_t *lexems){
  lexem_t a=(*lexems)->content;
  printf("Error while parsing: %s.It occured at line: %d and column: %d: \n",msg,a->line,a->column);
}

lexem_t lexem_advance(list_t *lexems){
  while(is_lexem_skippable((*lexems)->content)){
    list_t p_lex = *lexems;
    (*lexems) = (*lexems)->next;
    lexem_delete(p_lex->content);
  }
  lexem_t a = (*lexems)->content;
  return a;
}

int lexem_type(lexem_t lex,char *type_test){
  char* s = calloc(strlen(type_test),sizeof(char));
  int k = 0;
  for (k=0; k <= strlen(type_test); k++){
    s[k] = (lex->type)[k];
  }
  if(strcmp(lex->type, type_test) == 0) return 0;
  return -1;
}

int lexem_type_strict(lexem_t lex , char *type ){
  if((strcmp(lex->type, type) == 0)) return 0;
  return -1;
}

int next_lexem_is(list_t lexems, char *type){
  assert(!list_empty(lexems));
  lexem_t a=(lexems)->content;
  return lexem_type(a,type);
}
int next_lexem_is_strict(list_t lexems, char *type){
  assert(!list_empty(lexems));
  lexem_t a = list_first(lexems);
  return lexem_type_strict(a,type);
}

int list_lexem_print(list_t lexem){
  if (list_empty(lexem)){
    return 0;
  }
  list_print(lexem, lexem_print);
  return 1;
}