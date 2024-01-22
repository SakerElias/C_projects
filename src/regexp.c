/**
 * @file regexp.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Jul  28 09:56:03 2022
 * @brief regexp
 *
 * regexp code, as in the project's pdf document
 *
 */
#include <string.h>
#include <stdio.h>
#include <pyas/regexp.h>
#include <pyas/queue.h>
#include <pyas/charset.h>

static int re_match_one_or_more( charset_t c, queue_t *regexp, char *source , char **end ,int i);
static int re_match_zero_or_one( charset_t c, queue_t *regexp, char *source , char **end, int i );
int ismajusc(char a);
int isminuscules(char a);
int ischiffres(char a);
int transformer_tiret(char* car);
int sicplus(char* regexp,char regchar, queue_t *file , int neg, int crochet);
int sicfois(char* regexp, char regchar, queue_t *file  , int neg, int crochet);
int sicinter(char* regexp, char regchar, queue_t *file , int neg, int crochet);
int sicrien(char* regexp, char regchar, queue_t *file , int neg, int crochet);
int siccroch(char* regexp, int k, int neg, queue_t *file) ;
char minuscules[] = "abcdefghijklmnopqrstuvwxyz";
char majuscules[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char chiffres[] = "0123456789";


int ismajusc(char a) {
    return strchr(majuscules, a) != NULL;
}

int isminuscules(char a) {
    return strchr(minuscules, a) != NULL;
}

int ischiffres(char a) {
    return strchr(chiffres, a) != NULL;
}


// Transforme les expression entre crochet en suite de carctères simples (transforme entres autres les chaînes de caractère de la forme x-y)
int transformer_tiret(char* car) {
    char groupe[256] = "";
    int k = 0;

    while (car[k] != '\0') {
      if((car[k]=='\\') && (car[k+1]=='-')) {groupe[strlen(groupe)] = '-';k+=2;}
        if (car[k + 1] == '-' && car[k + 2] != '\0') {
            char a = car[k];
            char b = car[k + 2];

            if ((ismajusc(a) && ismajusc(b)) || (isminuscules(a) && isminuscules(b)) || (ischiffres(a) && ischiffres(b))) {
                char c=a;
                for (c = a; c <= b; c++) {
                    groupe[strlen(groupe)] = c; 
                }
                k += 3;
            } else {
                groupe[strlen(groupe)] = a; 
                k++;
            }
        }

        else if((car[k]=='\\') && (car[k+1]=='+')) {groupe[strlen(groupe)] = '+';k+=2;}
        else if((car[k]=='\\') && (car[k+1]==']')) {groupe[strlen(groupe)] = ']';k+=2;}
        else if((car[k]=='\\') && (car[k+1]=='*')) {groupe[strlen(groupe)] = '*';k+=2;}
        else if((car[k]=='\\') && (car[k+1]=='?')) {groupe[strlen(groupe)] = '?';k+=2;}
        else if((car[k]=='\\') && (car[k+1]=='[')) {groupe[strlen(groupe)] = '[';k+=2;}
        else if((car[k]=='\\') && (car[k+1]=='^')) {groupe[strlen(groupe)] = '^';k+=2;}
        else if((car[k]=='\\') && (car[k+1]=='.')) {groupe[strlen(groupe)] = '.';k+=2;}
        else if((car[k]=='\\') && (car[k+1]=='\\')) {groupe[strlen(groupe)] = '\\';k+=2;}
        else if((car[k]=='+')||(car[k]=='*')||(car[k]=='?')||(car[k]=='-')||(car[k]=='[')||(car[k]=='^')) {return -1;}
        else {
          if (car[k]!='\\') {
            groupe[strlen(groupe)] = car[k];} 
            k++;
        }
    }
    groupe[strlen(groupe)] = '\0';
    strcpy(car,groupe);
    return 1;
}


// fonction qui ajoute un charset de type 1 (one or more times) au chargroup donnée en argument
int sicplus(char* regexp,char regchar, queue_t *file , int neg, int crochet) {
    int type = 1;

    if (crochet) {
      charset_t a = charset_initialize();
      a=create_cset(a,regexp,neg,1);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    else {
      charset_t a = charset_initialize();
      a=create_a_cset(a,regchar,neg,type);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    return 0;
}

// fonction qui ajoute un charset de type 2 (zero or more times) au chargroup donnée en argument
int sicfois(char* regexp, char regchar, queue_t *file , int neg, int crochet) {
    if (crochet) {
      charset_t a = charset_initialize();
      a=create_cset(a,regexp,neg,2);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    else {
      charset_t a = charset_initialize();
      a=create_a_cset(a,regchar,neg,2);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    return 0;
}

// fonction qui ajoute un charset de type 3 (zero or one time) au chargroup donnée en argument
int sicinter(char* regexp, char regchar, queue_t *file , int neg, int crochet) {
    if (crochet) {
      charset_t a = charset_initialize();
      a=create_cset(a,regexp,neg,3);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    else {
      charset_t a = charset_initialize();
      a=create_a_cset(a,regchar,neg,3);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    return 0;
}

// fonction qui ajoute un charset de type 0 (one time) au chargroup donnée en argument
int sicrien(char* regexp, char regchar, queue_t *file , int neg, int crochet) {
    if (crochet) {
      charset_t a = charset_initialize();
      a=create_cset(a,regexp,neg,0);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    else {
      charset_t a=charset_initialize();
      a=create_a_cset(a,regchar,neg,0);
      *file = enqueue(*file,a);
      a->refcount++;
    }
    return 0;
}

// k = position du [ dans regexp
// fonction qui traite le cas des [], elle fait appelle aux fonctions de type "sic..." précédentes ainsi qu'a transforme tiret
int siccroch(char* regexp, int k, int neg, queue_t *file) {
    int i=0;
    int j,pos =0;
    int error;
    while ((regexp[k+i]!='\0')) {
      //teste si le caractere suivant est un ] sans être échappé
        if ((regexp[k+i]==']') && ((regexp[k+i-1]!='\\') || ((regexp[k+i-1]=='\\') && (regexp[k+i-2]=='\\')))) {
            pos = k+i;
            break;
        }
        else {
          i+=1;
        }
    }
    if ((regexp[k+i]=='\0')) {
      //on rajoute un charset vide dans le chargroup pour que re_match renvoie toujours 0 en cas d'erreur dans le re_read
      sicrien("ok",'\0',&(*file),0,0);
        return -2;
        }
    char groupe[256] = "";
    for (j=1;j<pos-k;j++) {
        groupe[strlen(groupe)] = regexp[k+j];
    }
    error = transformer_tiret(groupe);
    if (error ==-1) {
      sicrien("ok",'\0',&(*file),0,0);
      return -5;
    }

    if (regexp[pos+1]=='+') {
        sicplus(groupe,'a',file,neg,1);
        pos += 1;
    }
    else if (regexp[pos+1]=='*') {
        sicfois(groupe,'a',file,neg,1);
        pos += 1;
    }
    else if (regexp[pos+1]=='?') {
        sicinter(groupe,'a',file,neg,1);
        pos += 1;
    }
    else {
        sicrien(groupe,'a',file,neg,1);
    }
    return (pos);
    //pos = position du ]
}

int re_read(char* regexp,queue_t *file ) {
    int k =0;
    int neg = 0;
    if (file == NULL) {return 1;}
    if (regexp==NULL) {file=NULL; ; return 0;}
    //test les caractère impossible à avoir en debut de regexp
    if ((regexp[0]=='+') || (regexp[0]=='?') || (regexp[0]=='*') || (regexp[0]==']') ) {
      sicrien("ok",'\0',file,0,0);

              return -2;
       }

    while(regexp[k]!='\0') {
        if ((regexp[k]=='[')) {
            k = siccroch(regexp,k,0,file) +1;
            if (k==-4) {
              return 0;
            }
            if (k==-1 || k ==1) {
              if (k==1) {sicrien("ok",'\0',file,0,0);
              return 0;}
              else {sicrien("ok",'\0',&(*file),0,0);
              return -4;}
              }
        }
        else if (regexp[k]=='^') {
            neg = 1;
            if ((regexp[k+1]=='[')) {
                k = siccroch(regexp,k+1,neg,file) +1;
            }
            else {
              if ((regexp[k+1]=='\0'||regexp[k+1]=='+')||(regexp[k+1]=='*')||(regexp[k+1]=='?')||(regexp[k+1]=='-')||(regexp[k+1]=='\\')) {sicrien("ok",'\0',file,0,0); return -5;}
                if (regexp[k+2]=='+') {sicplus("ok",regexp[k+1],file,neg,0);k += 3;}
                else if (regexp[k+2]=='*') {sicfois("ok",regexp[k+1],file,neg,0);k += 3;}
                else if (regexp[k+2]=='?') {sicinter("ok",regexp[k+1],file,neg,0);k += 3;}
                else {sicrien("ok",regexp[k+1],file,neg,0);k += 2;}

            }
        }
        //on teste tous cas d'echappement possibles (meme un en trop : "-" c'est une erreur...)

        else if((regexp[k]=='\\') && (regexp[k+1]=='+')){
            if (regexp[k+2]=='+') {sicplus("ok",'+',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'+',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'+',file,neg,0);k += 3;}
            else {sicrien("ok",'+',file,neg,0);k += 2;}

        }
        else if((regexp[k]=='\\') && (regexp[k+1]==']')){
            if (regexp[k+2]=='+') {sicplus("ok",'/',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'/',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'/',file,neg,0);k += 3;}
            else {sicrien("ok",'/',file,neg,0);k += 2;}
        }
         else if((regexp[k]=='\\') && (regexp[k+1]=='*')){
            if (regexp[k+2]=='+') {sicplus("ok",'*',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'*',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'*',file,neg,0);k += 3;}
            else {sicrien("ok",'*',file,neg,0);k += 2;}
        }
         else if((regexp[k]=='\\') && (regexp[k+1]=='?')){
            if (regexp[k+2]=='+') {sicplus("ok",'?',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'?',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'?',file,neg,0);k += 3;}
            else {sicrien("ok",'?',file,neg,0);k += 2;}
        }
         else if((regexp[k]=='\\') && (regexp[k+1]=='-')){
            if (regexp[k+2]=='+') {sicplus("ok",'-',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'-',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'-',file,neg,0);k += 3;}
            else {sicrien("ok",'-',file,neg,0);k += 2;}
        }
         else if((regexp[k]=='\\') && (regexp[k+1]=='[')){
            if (regexp[k+2]=='+') {sicplus("ok",'[',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'[',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'[',file,neg,0);k += 3;}
            else {sicrien("ok",'[',file,neg,0);k += 2;}
        }
         else if((regexp[k]=='\\') && (regexp[k+1]=='^')){
            if (regexp[k+2]=='+') {sicplus("ok",'^',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'^',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'^',file,neg,0);k += 3;}
            else {sicrien("ok",'^',file,neg,0);k += 2;}
        }
         else if((regexp[k]=='\\') && (regexp[k+1]=='.')){
            if (regexp[k+2]=='+') {sicplus("ok",'.',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'.',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'.',file,neg,0);k += 3;}
            else {sicrien("ok",'.',file,neg,0);k += 2;}
        }
             else if((regexp[k]=='\\') && (regexp[k+1]=='\\')){
            if (regexp[k+2]=='+') {sicplus("ok",'\\',file,neg,0);k += 3;}
            else if(regexp[k+2]=='*') {sicfois("ok",'\\',file,neg,0);k += 3;}
            else if(regexp[k+2]=='?') {sicinter("ok",'\\',file,neg,0);k += 3;}
            else {sicrien("ok",'\\',file,neg,0);k += 2;}
        }
        // on teste si il y a des caractères qui comme étant placés auraient du être échappés
        else if((regexp[k]=='+')||(regexp[k]=='*')||(regexp[k]=='?')||(regexp[k]=='-')||(regexp[k]=='\\')) {
        sicrien("ok",'\0',&(*file),0,0);
        return -1;}

        else if((regexp[k]==']')) {sicrien("ok",'\0',&(*file),0,0);return -3;}

        else if (regexp[k]=='.'){
          if ((regexp[k+1]=='+')) {sicplus("acdekij12478",regexp[k],file,neg,1);k += 2;}
            else if(regexp[k+1]=='*') {sicfois("acdekij12478",regexp[k],file,neg,1);k += 2;}
            else if(regexp[k+1]=='?') {sicinter("acdekij12478",regexp[k],file,neg,1);k += 2;}
            else {sicrien("acdekij12478",regexp[k],file,neg,1);k += 1;}
        }

        else {
            if ((regexp[k+1]=='+')) {sicplus("ok",regexp[k],file,neg,0);k += 2;}
            else if(regexp[k+1]=='*') {sicfois("ok",regexp[k],file,neg,0);k += 2;}
            else if(regexp[k+1]=='?') {sicinter("ok",regexp[k],file,neg,0);k += 2;}
            else {sicrien("ok",regexp[k],file,neg,0);k += 1;}
        }
        neg = 0;
    }
    return 1;
}


static int re_match_zero_or_more( charset_t c, queue_t *a, char *source , char **end ,int i) {
  char *t = source;

  while ( '\0' != *t && ( (c->tab[(int)*t] == 1 && c->comp ==0) || (c->comp ==1 &&  c->tab[(int)*t] == 0))) t++;
  do {
    if (  re_match( a, t, end ,i) ) return 1;//On retourne 1 si t est pas reconnu par regexp
  } while ( t-- > source );

  if ( end ) *end = source;
  return 0;
}

int re_match( queue_t* a, char *source, char **end ,int i) {

  if ( NULL == source ) {
    if ( end ) *end = source;
    return 0;
  }

  if (  i>queue_length (*a) ) {
    if ( end ) *end = source;
    return 1;
  }

charset_t c = charset_initialize();
  charset_selector(*a,i,&c);

i+=1;
  if ( c->type == 2 ) {
    return re_match_zero_or_more( c, a, source, end,i);
  }
  
  if ( c->type == 1 ) {
    return re_match_one_or_more( c, a, source, end,i);
  }
  if ( c->type == 3 ) {
    return re_match_zero_or_one( c, a, source, end,i);
  }

  if (   '\0' != *source &&
       ((c->tab[(int)*source] == 1 && c->comp ==0) || (c->comp ==1 &&  c->tab[(int)*source] == 0)) ) {
    return (re_match( a, source+1, end,i));
  }
  i -=1;
  charset_delete(c);
  if ( end ) *end = source;
  return 0;
}

static int re_match_one_or_more( charset_t c, queue_t *a, char *source , char **end ,int i ) {
  char *t = source;
  if (*source == '\0') {if ( end ) *end =source; return 0;}
  if ((c->tab[(int)*source] == 1 && c->comp ==0)|| (c->comp ==1 &&  c->tab[(int)*source] == 0) ){

    return ( re_match_zero_or_more(c,a,t+1,end,i));
  }
  else{
    if ( end ) *end =source;
    return 0;
  }
}

static int re_match_zero_or_one( charset_t c, queue_t *a, char *source , char **end ,int i ){
  char*t = source;
  if (*source == '\0') {if ( end ) *end =source; return 1;}
  if ((c->tab[(int)*source] == 1 && c->comp ==0) || (c->comp ==1 &&  c->tab[(int)*source] == 0)){
    if (end) *end=source;
    return  re_match(a,t+1,end,i);
  }
  else {
    return  re_match(a,t,end,i);
  }
}