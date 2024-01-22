#include <stdlib.h>
#include <stdio.h>

#include <pyas/lexem.h>


int main(int argc, char **argv){
  
    lexem_t lexem;
    lexem = list_new();

  if (argc != 3){
    printf("Erreur, veuillez entrer le fichier de definition et le code source");
    exit(EXIT_FAILURE);
  }
  lexem = lex(argv[1], argv[2]);
  list_lexem_print(lexem);
  exit(EXIT_SUCCESS);
  return 1;
}
