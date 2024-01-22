/**
 * @file regexp.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Fri Jul  28 09:56:03 2022
 * @brief Regexp parsing and matching.
 *
 * Program for regexp parsing, and regexp matching.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <pyas/regexp.h>

int main ( int argc, char *argv[] ) {
  char     *end = NULL;
  int  is_match;

  if ( argc < 3 ) {
    fprintf( stderr, "Usage :\n\t%s regexp text\n", argv[ 0 ] );
    exit( EXIT_FAILURE );
  }

  chargroup_t fifo = alloc_cgroup();
  re_read(argv[1],&(fifo.file));
  is_match = re_match( &(fifo.file), argv[ 2 ], &end,1 );

  if ( is_match ) {
    printf( "The start of '%s' is %s, %s: '%s'.\n", argv[2], argv[ 1 ], *end ? "next" : "END", end );
  }
  else {
    printf( "The start of '%s' is *NOT* %s.\n", argv[2], argv[ 1 ] );
  }
  chargroup_delete (fifo,charset_delete_wrapper);
  exit( EXIT_SUCCESS );
}
