/**
 * @file unitest.h
 * @author François Cayre <cayre@uvolante.org>
 * @date Mon Aug  8 08:47:06 2022
 * @brief Unit testing.
 *
 * Unit testing.
 */

#ifndef _UNITEST2_H_
#define _UNITEST2_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TRACE_MEM

  /*
     Use https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#256-colors
     to implement your very own distinctive bad taste here:
  */
#define TEST_STYLE_SUCCESS()      STYLE( stderr, 118, ST_BOLD )
#define TEST_STYLE_INFO()         STYLE( stderr, 231, ST_BOLD )
#define TEST_STYLE_ERROR()        STYLE( stderr, 196, ST_BOLD )
#define TEST_STYLE_EMPH()         STYLE( stderr, 160, ST_BOLD )
#define TEST_STYLE_EMPH_LIGHT()   STYLE( stderr, 105, ST_UNDERLINE";"ST_ITALIC";"ST_FAINT )
#define TEST_STYLE_DESCR()        STYLE( stderr, 173, ST_BOLD";"ST_ITALIC )

#define VERBOSE( cond, fmt, ... ) if ( cond )			\
    MSG_FMT( 0, "VERBOSE", 34, ST_FAINT, fmt, ##__VA_ARGS__ )
#define INFO( fmt, ... )					\
  MSG_FMT( 0, "  INFO ", 147, ST_BOLD, fmt, ##__VA_ARGS__ )
#define WARNING( fmt, ... )					\
  MSG_FMT( 0, "WARNING", 208, ST_BOLD, fmt, ##__VA_ARGS__ )
#define ERROR( fmt, ... )					\
  MSG_FMT( 0, " ERROR ", 196, ST_BOLD, fmt, ##__VA_ARGS__ )
#define FATAL( fmt, ... )					\
  MSG_FMT( 1, " FATAL ", 160, ST_BOLD";"ST_BLINK";"ST_ITALIC, fmt, ##__VA_ARGS__ )

#define _XOPEN_SOURCE 700

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <signal.h>
#include <setjmp.h>

#define ST_BOLD                "1"
#define ST_FAINT               "2"
#define ST_ITALIC              "3"
#define ST_UNDERLINE           "4"
#define ST_UNDERLINE_DBL      "21"
#define ST_BLINK               "5"
#define ST_OVERLINE           "53"
#define ST_INVERSE             "7"
#define ST_DELETED             "9"

#define ANSI_ESC           "\x1b"
#define TERM_CSI      ANSI_ESC"["

#define STYLE( file, color, style )			\
  fprintf( (file), TERM_CSI style";38;5;"#color"m" )

#define STYLE_RESET( file ) fprintf( file, TERM_CSI"0m" )

#define MSG_FMT( fatal, prompt, color, style, fmt, ... ) do {		\
    char    buf[ BUFSIZ ];						\
    char     *p = buf;							\
    size_t  len = 0;							\
    size_t    n = 0;							\
    memset( buf, 0, BUFSIZ );						\
    len = snprintf( buf, BUFSIZ, fmt, ##__VA_ARGS__ );			\
    for ( n = len ; '\n' == buf[ n-1 ] ; n-- );				\
    buf[ n ] = '\0';							\
    for ( p = buf ; '\n' == *p && p-buf < BUFSIZ-1 ; p++ ) {		\
      fprintf( stderr, "\n" );						\
    }									\
    STYLE( stderr, color, style );					\
    for ( p = strtok( p, "\n" ) ; p ; p = strtok( NULL, "\n" ) )	\
      fprintf( stderr, "[ %s ] %s\n", prompt, p );			\
    STYLE_RESET( stderr );						\
    fprintf( stderr, "%s", buf+n+1 );					\
    if ( fatal ) abort();						\
  } while ( 0 )

  typedef struct {
    unsigned    total;
    unsigned    passed;
    unsigned    failed;
    unsigned    interrupted;
    unsigned    aborted;
    unsigned    timed_out;

    unsigned    blocks;
    unsigned    memory;
  } test_counter_t;

  typedef struct {
    struct {
      sigjmp_buf   restart;
      char        *TEST_ORACLES;
      int          TEST_TIMEOUT;
      int          TEST_SIZE;
      char         real_prog_name[ 1024 ];
      char        *prog_name;
      char        *test_name;
      int          verbose;
      int          debug;
      int          expect_abort;
      struct {
	char      *captured;
	char      *oracle;
	char       file[ BUFSIZ ];
	int        is_OK;
      }            out, err;
      struct {
	void    *(*_malloc)( size_t );
	void    *(*_realloc)( void*, size_t );
	void     (*_free)( void* );
      }            sys;
    }              env;
    test_counter_t suite;
    test_counter_t total;
  } test_t;

  extern test_t this;

#ifdef TRACE_MEM
#define TRACE  , const char *file, const char *fun, int line
#define TRACED , __FILE__, __func__, __LINE__
#else
#define TRACE
#define TRACED
#endif


  void *__malloc( size_t size TRACE );
  void *__calloc( size_t nmemb, size_t size TRACE );
  void *__realloc( void *ptr, size_t size TRACE );
  char *__strdup( const char *s TRACE );
  void  __free( void *ptr TRACE );

#define   malloc( size )        __malloc( size TRACED )
#define   calloc( nmemb, size ) __calloc( nmemb, size TRACED )
#define   realloc( ptr, size )  __realloc( ptr, size TRACED )
#define   strdup( s )           __strdup( s TRACED )
#define   free( ptr )           __free( ptr TRACED )

  void    unit_test( int argc, char *argv[] );
  void    posix_signal_handler( int sig, siginfo_t *siginfo, void *context );
  void    set_signal_handler( void );

  void    test_dump__( char *buff, char *orc, char *which );
#define   test_dump( buff, orc, which ) test_dump__( (buff), (orc), #which )

#define PREPARE_ORACLE( orc, dst, file ) do {				\
    if ( orc && (char*)orc != (char*)dst ) {				\
      FILE         *fp = NULL;						\
      snprintf( file, BUFSIZ-1, "%s/%s", this.env.TEST_ORACLES,	orc );	\
      fp = fopen( file, "r" );						\
      if ( NULL == fp ) {						\
	file[ 0 ] = '\0';						\
	olen = strlen( orc );						\
	if ( (int)olen > this.env.TEST_SIZE ) olen = this.env.TEST_SIZE; \
	memcpy( dst, orc, olen );					\
      }									\
      else {								\
	olen = fread( dst, 1, this.env.TEST_SIZE, fp );			\
	fclose( fp );							\
      }									\
      while ( olen && '\n' == dst[ olen-1 ] ) {				\
	dst[ --olen ] = '\0';						\
      }									\
    }									\
  } while ( 0 )

  char *orc_fmt( char *dst, const char *fmt, ... );

#define orc_out( fmt, ... )  orc_fmt( this.env.out.oracle, fmt, ##__VA_ARGS__ )
#define orc_err( fmt, ... )  orc_fmt( this.env.err.oracle, fmt, ##__VA_ARGS__ )

#define COMPARE_ORACLE( res, orc, captured ) do {			\
    size_t l = strlen( captured );					\
    while ( l && '\n' == captured[ l-1 ] )				\
      captured[ --l ] = '\0';						\
    if ( strcmp( orc, captured ) ) {					\
      res = 0;								\
    }									\
  } while ( 0 )

#define PRINT_DIFF( orc, captured, name ) do {				\
    size_t exp_len = strlen( orc );					\
    size_t    olen = strlen( captured );				\
    size_t       i = 0;							\
    size_t max_len = 0;							\
    size_t min_len = exp_len < olen ? exp_len : olen;			\
    for ( max_len = 0; max_len < min_len ; max_len++ ) {		\
      if ( orc[ max_len ] != captured[ max_len ] ) break;		\
    }									\
    fprintf( stderr, "\n " );						\
    TEST_STYLE_EMPH_LIGHT();						\
    fprintf( stderr, "Expected" );					\
    STYLE_RESET( stderr );						\
    fprintf( stderr, " %s", '\0' == *orc ? "nothing" : "'" );		\
    for ( i = 0 ; i < max_len ; i++ ) {					\
      fprintf( stderr, "%c", orc[ i ] );				\
    }									\
    TEST_STYLE_ERROR();							\
    for ( ; i < exp_len ; i++ ) {					\
      fprintf( stderr, "%c", orc[ i ] );				\
    }									\
    STYLE_RESET( stderr );						\
    if ( *orc ) fprintf( stderr, "'" );					\
    fprintf( stderr, "\n " );						\
    TEST_STYLE_EMPH_LIGHT();						\
    fprintf( stderr, "But got" );					\
    STYLE_RESET( stderr );						\
    fprintf( stderr, "%s", *captured ? ": '" : " nothing" );		\
    for ( i = 0 ; *captured && i < max_len ; i++ ) {			\
      fprintf( stderr, "%c", captured[ i ] );				\
    }									\
    TEST_STYLE_ERROR();							\
    for ( ; *captured && i < olen ; i++ ) {				\
      fprintf( stderr, "%c", captured[ i ] );				\
    }									\
    STYLE_RESET( stderr );						\
    if ( *captured ) fprintf( stderr, "'" );				\
    fprintf( stderr, " on " #name );					\
  } while ( 0 )



#define test_driver( exp_abort, test, __orc_out, __orc_err, comment, ... ) do { \
    int      ret__  = 0;						\
    size_t   olen = 0;							\
    unsigned saved_ntests_aborted = this.suite.aborted;			\
    char     *_orc_out = __orc_out; /* only one macro parm eval! */	\
    char     *_orc_err = __orc_err;					\
    this.env.expect_abort         = exp_abort;				\
    memset( this.env.out.captured, '\0', this.env.TEST_SIZE );		\
    memset( this.env.err.captured, '\0', this.env.TEST_SIZE );		\
    PREPARE_ORACLE( _orc_out, this.env.out.oracle, this.env.out.file );	\
    PREPARE_ORACLE( _orc_err, this.env.err.oracle, this.env.err.file );	\
    this.env.out.is_OK = 1;						\
    this.env.err.is_OK = 1;						\
    this.suite.total++;							\
    this.total.total++;							\
    if ( this.env.verbose ) {						\
      TEST_STYLE_DESCR();						\
      fprintf( stderr, comment, ## __VA_ARGS__ );			\
      STYLE_RESET( stderr );						\
      fprintf( stderr, ": " );						\
    }									\
    if ( _orc_out ) {							\
      fflush( stdout );							\
      freopen( "/dev/null", "a", stdout );				\
      setvbuf( stdout, this.env.out.captured, _IOFBF,			\
	       this.env.TEST_SIZE );					\
    }									\
    if ( _orc_err && !this.env.debug ) {				\
      freopen( "/dev/null", "a", stderr );				\
      setvbuf( stderr, this.env.err.captured, _IOFBF,			\
	       this.env.TEST_SIZE );					\
    }									\
    if ( 0 == sigsetjmp( this.env.restart, 1 ) ) {			\
      alarm( this.env.TEST_TIMEOUT );					\
      ret__ = !!(test);							\
      alarm( 0 );							\
    }									\
    else {								\
      alarm( 0 );							\
      set_signal_handler();						\
      ret__ = this.env.expect_abort ?					\
	( this.suite.aborted == 1 + saved_ntests_aborted ) : 0;		\
    }									\
    if ( _orc_out ) {							\
      fflush( stdout );							\
      freopen( "/dev/tty", "a", stdout );				\
      setvbuf( stdout, NULL, _IOLBF, 0 );				\
      if ( *this.env.out.file )						\
	test_dump( this.env.out.captured, this.env.out.file, stdout );	\
      if ( !this.env.expect_abort )					\
	COMPARE_ORACLE( this.env.out.is_OK,				\
			this.env.out.oracle, this.env.out.captured );	\
    }									\
    if ( _orc_err && !this.env.debug ) {				\
      fflush( stderr );							\
      freopen( "/dev/tty", "a", stderr );				\
      setvbuf( stderr, NULL, _IONBF, 0 );				\
      if ( *this.env.err.file )						\
	test_dump( this.env.err.captured, this.env.err.file, stderr );	\
      if ( !this.env.expect_abort )					\
	COMPARE_ORACLE( this.env.err.is_OK,				\
			this.env.err.oracle, this.env.err.captured );	\
    }									\
    if ( this.env.out.is_OK && this.env.err.is_OK && ret__ ) {		\
      this.suite.passed++;						\
      this.total.passed++;						\
      if ( this.env.verbose ) {						\
	TEST_STYLE_SUCCESS();						\
	fprintf( stderr, "PASSED" );					\
	STYLE_RESET( stderr );						\
      }									\
    }									\
    else {								\
      if ( this.env.verbose ) {						\
	TEST_STYLE_ERROR();						\
	fprintf( stderr, "FAILED" );					\
	STYLE_RESET( stderr );						\
	if ( !this.env.out.is_OK ) {					\
	  PRINT_DIFF( this.env.out.oracle, this.env.out.captured,	\
		      stdout );						\
	  if ( !this.env.err.is_OK || !ret__ )				\
	    fprintf( stderr, "\nand" );					\
        }								\
	if ( !this.env.err.is_OK ) {					\
	  PRINT_DIFF( this.env.err.oracle, this.env.err.captured,	\
		      stderr );						\
	  if ( !ret__ )							\
	    fprintf( stderr, "\nand" );					\
        }								\
	if ( !ret__ ) {							\
	  if ( this.suite.aborted == saved_ntests_aborted ) {		\
	    fprintf( stderr, " assertion " );				\
	    TEST_STYLE_ERROR();						\
	    fprintf( stderr, "%s", #test );				\
	    STYLE_RESET( stderr );					\
	  }								\
	  else {							\
	    fprintf( stderr, " (aborted)" );				\
	  }								\
	}								\
	fprintf( stderr, " at " );					\
	TEST_STYLE_EMPH_LIGHT();					\
	fprintf( stderr, "%s:%s:%d", this.env.prog_name,		\
		 __func__, __LINE__ );					\
	STYLE_RESET( stderr );						\
      }									\
    }									\
    memset( this.env.out.oracle, 0, this.env.TEST_SIZE );		\
    memset( this.env.err.oracle, 0, this.env.TEST_SIZE );		\
    if ( this.env.verbose ) {						\
      fprintf( stderr, ".\n" );						\
    }									\
  } while ( 0 )

#define test_oracle( test, ... ) test_driver( 0, test, ## __VA_ARGS__ )
#define test_assert( test, ... ) test_driver( 0, test, (char*)NULL,	\
					      (char*)NULL, ## __VA_ARGS__ )
#define test_abort( test, ... )  test_driver( 1, test, (char*)NULL,	\
					      (char*)NULL, ## __VA_ARGS__ )

#define test_suite( msg ) do {						\
    if ( this.env.test_name ) {						\
      if ( this.suite.passed != this.suite.total ) {			\
	if ( !this.env.verbose ) {					\
	  fprintf( stderr, ": " );					\
	}								\
	TEST_STYLE_ERROR();						\
	fprintf( stderr, "FAILED " );					\
	TEST_STYLE_EMPH();						\
	fprintf( stderr, "%u test%s",					\
		 this.suite.total - this.suite.passed,			\
		 this.suite.total - this.suite.passed > 1 ? "s" : "");	\
	STYLE_RESET( stderr );						\
	fprintf( stderr, " (out of %u)", this.suite.total );		\
	if ( this.suite.interrupted || this.suite.timed_out || this.suite.aborted ) { \
	  if ( this.suite.aborted ) {					\
	    fprintf( stderr, ", " );					\
	    TEST_STYLE_EMPH();						\
	    fprintf( stderr, "%u aborted", this.suite.aborted );	\
	    STYLE_RESET( stderr );					\
	  }								\
	  if ( this.suite.interrupted ) {				\
	    fprintf( stderr, ", " );					\
	    TEST_STYLE_EMPH();						\
	    fprintf( stderr, "%u interrupted", this.suite.interrupted ); \
	    STYLE_RESET( stderr );					\
	  }								\
	  if ( this.suite.timed_out )	{				\
	    fprintf( stderr, ", " );					\
	    TEST_STYLE_EMPH();						\
	    fprintf( stderr, "%u timed out", this.suite.timed_out );	\
	    STYLE_RESET( stderr );					\
	  }								\
	}								\
	if ( this.suite.blocks ) {					\
	fprintf( stderr, ", and " );					\
	TEST_STYLE_EMPH();						\
	    fprintf( stderr, "%u leak%s (%u byte%s)",			\
		     this.suite.blocks,					\
		     this.suite.blocks > 1 ? "s" : "",			\
		     this.suite.memory,					\
		     this.suite.memory > 1 ? "s" : "");			\
	    STYLE_RESET( stderr );					\
	}								\
	fprintf( stderr, ".\n" );					\
	if ( 0 == this.env.verbose ) {					\
          TEST_STYLE_INFO();						\
          fprintf( stderr, "Relaunch with -v for details.\n");		\
          STYLE_RESET( stderr );					\
	}								\
      }									\
      else {								\
	if ( !this.env.verbose ) {					\
	  fprintf( stderr, ": " );					\
	  TEST_STYLE_SUCCESS();						\
	  fprintf( stderr, "PASSED" );					\
	  STYLE_RESET( stderr );					\
	  if ( this.suite.blocks ) {					\
	    fprintf( stderr, ", but " );				\
	  }								\
	}								\
	else {								\
	  if ( this.suite.blocks ) {					\
	    TEST_STYLE_INFO();						\
	    fprintf( stderr, "Suite" );					\
	    STYLE_RESET( stderr );					\
	    fprintf( stderr, ": " );					\
	    TEST_STYLE_EMPH();						\
	  }								\
	}								\
	if ( this.suite.blocks ) {					\
	  TEST_STYLE_EMPH();						\
	  fprintf( stderr, "%u leak%s (%u byte%s)",			\
		   this.suite.blocks,					\
		   this.suite.blocks > 1 ? "s" : "",			\
		   this.suite.memory,					\
		   this.suite.memory > 1 ? "s" : "");			\
	  STYLE_RESET( stderr );					\
	  fprintf( stderr, "." );					\
	}								\
	if ( !this.env.verbose || this.suite.blocks )			\
	  fprintf( stderr, "\n" );					\
      }								        \
    }									\
    this.env.test_name = msg;						\
    if ( this.env.test_name ) {						\
      TEST_STYLE_INFO();						\
      fprintf( stderr, "[ %12s ] %s",					\
	       __func__, this.env.test_name );				\
      STYLE_RESET( stderr );						\
      if ( this.env.verbose ) {						\
	fprintf( stderr, "\n" );					\
      }									\
    }									\
    this.suite.total       = 0;						\
    this.suite.passed      = 0;						\
    this.suite.failed      = 0;						\
    this.suite.interrupted = 0;						\
    this.suite.aborted     = 0;						\
    this.suite.timed_out   = 0;						\
    this.suite.blocks      = 0;						\
    this.suite.memory      = 0;						\
  } while ( 0 )

#ifdef __cplusplus
}
#endif

#endif /* _UNITEST2_H_ */
