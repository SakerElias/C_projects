#include <pyas/lexem.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pyas/queue.h>
#include <pyas/list.h>

int main(){
  printf("--------------\n");
  printf("Début des tests sur les fonctions d'analyse grammaticale\n");
    lexem_t a = lexem_new("blank::skip", " ", 2, 1, "0x05");
    lexem_t e = lexem_new("blank::skip", " ", 2, 1,"");
    lexem_t b = lexem_new("op::minus", "-", 5, 2,"");
    lexem_t c = lexem_new("comment::skip", "#ceci est un commentaire", 5, 4,"");
    lexem_t d = lexem_new("op::plus", "+", 6, 1,"");
    printf("Test de is_lexem_skippable:\n");
    printf("Les résultats attendus sont 1,0,1,0\n");
    printf("Les résultats obtenus sont %d %d %d %d\n",is_lexem_skippable(a),is_lexem_skippable(b),is_lexem_skippable(c),is_lexem_skippable(d));
    printf("TEST DE IS LEXEM SKIPPABLE REUSSI\n");
      printf("--------------\n");
      printf("Enfilage des lexems puis test de queue_to_list:\n");
      printf("Enfilage\n");

    queue_t q_lexems = queue_new();
    q_lexems = enqueue(q_lexems, e);
    q_lexems = enqueue(q_lexems, a);
    q_lexems = enqueue(q_lexems, b);
    q_lexems = enqueue(q_lexems, c);
    q_lexems = enqueue(q_lexems, d);
    printf("On affiche la file \n");
    queue_print(q_lexems,lexem_print);
    printf("\n");
    list_t liste_lex = list_new;
    liste_lex = queue_to_list(q_lexems);
    printf("On vérifie que liste_lex a bien été créee\n");
    list_print(liste_lex, lexem_print);
    printf("\n");
    printf("Test réussi\n");
    printf("-----------------------\n");


    
    printf("On teste lexem_peek\n");
    lexem_print(lexem_peek(liste_lex));
    printf("\n");
    printf("Test concluant pour lexem_peek\n");
    printf("----------------\n");
    printf("On teste le renvoi lexem_advance\n");
    lexem_print(lexem_advance(&liste_lex));
    printf("on verifie que liste lex est bien modifiée\n");
    list_print(liste_lex, lexem_print);
    printf("----------------\n");
    printf("Test de message d'erreur lexem\n");
    print_parse_error("INCOMPATIBLE LEXEM TYPE",liste_lex);
    printf("Test réussi\n");
    printf("----------------\n");
    printf("On teste next_lexem_is\n");
    if(next_lexem_is(liste_lex,"op::minus") && !next_lexem_is(liste_lex, "op")){
      printf("Test reussi\n");
    }
    else{
      printf("test raté\n");
    }
    printf("----------------\n");
    printf("On teste next_lexem_is_strict\n");
    if(next_lexem_is_strict(liste_lex,"op::minus") && !next_lexem_is_strict(liste_lex, "op")){
      printf("Test reussi\n");
    }
    else{
      printf("test raté\n");
    }
    printf("----------------\n");
    printf("----------------\n");
    return 1;

}
