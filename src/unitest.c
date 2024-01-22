//#define _XOPEN_SOURCE 700

#define _GNU_SOURCE /* RTLD_NEXT */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <err.h>
#include <execinfo.h>
#include <dlfcn.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include <setjmp.h>

#include <unitest/unitest.h>
#include <pyas/macros.h>

test_t this; 

struct memblk_t { 
  size_t    size; 
}; 

#ifdef TRACE_MEM

typedef struct trace_t { 
  struct trace_t *next; 
  const char     *file; 
  const char     *fun; 
  int             line; 
  void           *ptr; 
} trace_t; 

#define H_TRACE 255
trace_t *blocks[ H_TRACE ] = { NULL }; 

/* At each instant, alloc'd pointers are unique values */
static void trace_add( trace_t trace ) { 
  trace_t *t = this.env.sys._malloc( sizeof( *t ) ); 
  assert( t ); 
  memcpy( t, &trace, sizeof( *t ) ); 
  if ( this.env.verbose > 3 ) { 
    fprintf( stderr, "Adding trace %p from %s:%d\n", trace.ptr, trace.fun, trace.line );
  }
  t->next = blocks[ (size_t)trace.ptr % H_TRACE ]; 
  blocks[ (size_t)trace.ptr % H_TRACE ] = t; 
  return; 
}

static void trace_remove( trace_t trace ) { 
  trace_t *t = blocks[ (size_t)trace.ptr % H_TRACE ]; 

  if ( t && trace.ptr == t->ptr ) { 
    if ( this.env.verbose > 3 ) { 
      fprintf( stderr, "Removing trace of %p from %s:%d\n", trace.ptr, trace.fun, trace.line ); 
    }
    blocks[ (size_t)trace.ptr % H_TRACE ] = t->next; 
    this.env.sys._free( t ); 
    return; 
  }
  
  for ( ; t ; t = t->next ) { 
    if ( t->next && trace.ptr == t->next->ptr ) { 
      trace_t *save = t->next->next; 
      this.env.sys._free( t->next ); 
      t->next = save; 
      if ( this.env.verbose > 3 ) { 
	fprintf( stderr, "Removing trace of %p from %s:%d\n", trace.ptr, trace.fun, trace.line );
      }
      break; 
    }
  }
  
  return; 
}

static void trace_flush( void ) { 
  size_t   i; 
  trace_t *t; 

  for ( i = 0 , t = blocks[ 0 ] ; i < H_TRACE ; i++ ) { 
    trace_t *next; 
    for ( next = NULL , t = blocks[ i ] ; t ; t = next ) { 
      next = t->next; 
      this.env.sys._free( t ); 
    }
  }
}

static int compare_traces( const void *p1, const void *p2 ) { 
  trace_t   *t1 = *(trace_t**)p1; 
  trace_t   *t2 = *(trace_t**)p2; 
  int       ret = strcmp( t1->fun, t2->fun ); 

  if ( ret ) return ret; 

  if ( t1->line == t2->line ) { 
    struct memblk_t *b1 = ((struct memblk_t*)(t1->ptr))-1; 
    struct memblk_t *b2 = ((struct memblk_t*)(t2->ptr))-1; 
    if ( b1->size == b2->size ) return 0; 
    return b1->size  < b2->size ? -1 : 1; 
  }
  return t1->line < t2->line ? -1 : 1; 
}

struct alloc_count_t {
  const char *file; 
  const char *fun; 
  int         line; 
  size_t      blocks; 
  size_t      memory; 
}; 

static int compare_alloc_counts( const void *p1, const void *p2 ) { 
  const struct alloc_count_t *a1 = p1; 
  const struct alloc_count_t *a2 = p2; 

  if ( a1->blocks == a2->blocks ) { 
    if ( a1->memory == a2->memory ) return 0; 
    return a1->memory > a2->memory ? -1 : 1; 
  }
  return a1->blocks > a2->blocks ? -1 : 1; 
}

static void trace_log( FILE *fp ) { 
  trace_t       *t; 
  size_t         i, j; 
  const char    *curr_fun    = NULL; 
  int            curr_line   = -1; 
  size_t         curr_blocks = 0; 
  size_t         curr_memory = 0; 
  size_t         max         = 0; 

  struct alloc_count_t *printable   = NULL; 

  trace_t      **sorted      = this.env.sys._malloc( this.total.blocks * sizeof( *sorted ) ); 
  
  for ( i = 0 , j = 0 ; i < H_TRACE ; i++ ) 
    for ( t = blocks[ i ] ; t ; sorted[ j++ ] = t , t = t->next );

  assert( j == this.total.blocks ); 

  qsort( sorted, this.total.blocks, sizeof( sorted[ 0 ] ), compare_traces ); 

  curr_fun  = sorted[ 0 ]->fun; 
  curr_line = sorted[ 0 ]->line; 

  for ( i = 0 , j = 0 ; i < this.total.blocks ; i++ ) { 
    struct memblk_t *b = ((struct memblk_t*)(sorted[ i ]->ptr))-1; 
    curr_blocks++;
    curr_memory += b->size;

    if ( i == this.total.blocks-1 ||
	 sorted[ i+1 ]->line > curr_line ||
	 sorted[ i+1 ]->fun != curr_fun ) { 
      j++; 

      if ( i < this.total.blocks-1 ) { 
	curr_fun    = sorted[ i+1 ]->fun;
	curr_line   = sorted[ i+1 ]->line;
      }
    }
  }

  printable = this.env.sys._malloc( j * sizeof( *printable ) );
  assert( printable ); 
  
  curr_fun    = sorted[ 0 ]->fun; 
  curr_line   = sorted[ 0 ]->line; 
  curr_blocks = 0; 
  curr_memory = 0; 

  for ( i = 0 , j = 0 , max = 0 ; i < this.total.blocks ; i++ ) { 
    struct memblk_t *b = ((struct memblk_t*)(sorted[ i ]->ptr))-1; 
    curr_blocks++;
    curr_memory += b->size;

    if ( i == this.total.blocks-1 || ( i < this.total.blocks-1 && ( sorted[ i+1 ]->line > curr_line || sorted[ i+1 ]->fun != curr_fun ) ) ) { 
      printable[ j ].file   = sorted[ i ]->file; 
      printable[ j ].fun    = curr_fun; 
      printable[ j ].line   = curr_line; 
      printable[ j ].blocks = curr_blocks; 
      printable[ j ].memory = curr_memory; 

      if ( max < printable[ j ].blocks*sizeof(struct memblk_t)+printable[ j ].memory ) {
	max = printable[ j ].blocks*sizeof(struct memblk_t)+printable[ j ].memory; 
      }
      
      if ( i < this.total.blocks-1 ) { 
	curr_fun    = sorted[ i+1 ]->fun; 
	curr_line   = sorted[ i+1 ]->line; 
      }
      j++; 

      curr_blocks = 0; 
      curr_memory = 0; 
    }
  }

  qsort( printable, j, sizeof( *printable ), compare_alloc_counts ); 

  max = this.total.memory + this.total.blocks*sizeof( struct memblk_t ); 
  
  for ( i = 0 ; i < j ; i++ ) { 
    float  thresh = .2; 
    size_t   size = printable[ i ].memory + printable[ i ].blocks*sizeof( struct memblk_t ); 
    char file[ BUFSIZ ]; 
    char *ptr = file, *buff = file; 
    memcpy( file, printable[ i ].file, 1+strlen( printable[ i ].file ) ); 
    for ( ptr = strtok( file, "/" ) ; ptr ; buff = ptr , ptr = strtok( NULL, "/" ) ); 


    if ( size > max*thresh ) { 
      TEST_STYLE_DESCR(); 
    }
    
    if ( 1 == this.total.blocks ) {
      fprintf( fp, "   100%% at " ); 
    }
    else { 
      fprintf( fp, " %5.1lf%% at ",
	       100. * size / (double)( this.total.memory + this.total.blocks*sizeof( struct memblk_t ) ) ); 
    }
    fprintf( fp, "%s: %s:%d: %zu block%s, %zu byte%s (%zu real)\n",
	     buff, printable[ i ].fun, printable[ i ].line, 
	     printable[ i ].blocks, printable[ i ].blocks > 1 ? "s" : "", 
	     printable[ i ].memory, printable[ i ].memory > 1 ? "s" : "",
	     printable[ i ].blocks*sizeof(struct memblk_t) + printable[ i ].memory); 

    if ( size > max*thresh ) { 
      STYLE_RESET( stderr ); 
    }
  }

  this.env.sys._free( printable ); 
  this.env.sys._free( sorted ); 
  
  return; 
}

#endif

void *__malloc( size_t size TRACE ) { 
  struct memblk_t *b = this.env.sys._malloc( sizeof( struct memblk_t ) + size ); 

#ifdef TRACE_MEM
  trace_t trace = { NULL, file, fun, line, NULL }; 
#endif
  
  b->size = size; 
  
  this.suite.blocks++; 
  this.suite.memory += b->size; 
  
  this.total.blocks++; 
  this.total.memory += b->size; 

  if ( this.env.verbose > 2 ) { 
    fprintf( stderr, "  [*] Allocating %zu bytes.\n", b->size );
  }

  b++; 

#ifdef TRACE_MEM
  trace.ptr = b; 
  trace_add( trace ); 
#endif
  
  return b; 
}

void *__calloc( size_t nmemb, size_t size TRACE ) { 
  struct memblk_t *b = this.env.sys._malloc( sizeof( struct memblk_t ) + nmemb*size ); 

#ifdef TRACE_MEM
  trace_t trace = { NULL, file, fun, line, NULL }; 
#endif
  
  b->size = nmemb*size; 
  
  this.suite.blocks++; 
  this.suite.memory += b->size; 
  
  this.total.blocks++; 
  this.total.memory += b->size; 

  memset( b+1, 0, b->size ); 
  
  if ( this.env.verbose > 2 ) { 
    fprintf( stderr, "  [*] Allocating %zu bytes.\n", b->size );
  }

  b++; 

#ifdef TRACE_MEM
  trace.ptr = b;
  trace_add( trace ); 
#endif
  
  return b; 
}

void *__realloc( void *ptr, size_t size TRACE ) { 
  struct memblk_t *b = ((struct memblk_t*)ptr)-1; 
  void            *o = b; 

#ifdef TRACE_MEM
  trace_t trace = { NULL, file, fun, line, NULL }; 
#endif
  
  if ( size == b->size ) return ptr; 

  if ( size > b->size ) { 
    this.suite.memory += size - b->size; 
    this.total.memory += size - b->size; 
    if ( this.env.verbose > 2 ) { 
      fprintf( stderr, "  [*] Allocating %zu bytes.\n", size - b->size );
    }
  }
  else { 
    this.suite.memory -= b->size - size; 
    this.total.memory -= b->size - size; 
    if ( this.env.verbose > 2 ) { 
      fprintf( stderr, "  [*] Freeing %zu bytes.\n", b->size - size );
    }
  }

  b = this.env.sys._realloc( b, size + sizeof( struct memblk_t ) ); 
  assert( b ); 

#ifdef TRACE_MEM
  if ( o != b ) { 
    trace.ptr = ptr; 
    trace_remove( trace ); 
    trace.ptr = b+1; 
    trace_add( trace ); 
  }
#endif
  
  b->size = size; 

  b++; 

  return b; 
}

char *__strdup( const char *s TRACE ) { 
  char *dup = NULL; 

  if ( s ) { 
    size_t len = strlen( s ); 
#ifdef TRACE_MEM
    dup = __malloc( 1+len, file, fun, line ); 
#else
    dup = malloc( 1+len );
#endif
    assert( dup ); 
    memcpy( dup, s, 1+len ); 
  }

  return dup; 
}

void  __free( void *ptr TRACE ) { 

  if ( ptr ) { 
    struct memblk_t *b = ((struct memblk_t*)ptr)-1; 

#ifdef TRACE_MEM
    trace_t trace = { NULL, file, fun, line, ptr }; 
#endif
  
    this.suite.blocks--; 
    this.suite.memory -= b->size; 

    this.total.blocks--; 
    this.total.memory -= b->size; 

    if ( this.env.verbose > 2 ) { 
      fprintf( stderr, "  [*] Freeing %zu bytes.\n", b->size );
    }

#ifdef TRACE_MEM
    trace_remove( trace ); 
#endif
  
    this.env.sys._free( b ); 
  }
  
  return; 
}

static void test_finish( void ) { 

  test_suite( NULL ); 

  if ( this.total.blocks ) { 
    TEST_STYLE_ERROR(); 
    fprintf( stderr, "Leaking" ); 
    STYLE_RESET( stderr ); 
    fprintf( stderr, ": " ); 
    TEST_STYLE_EMPH(); 
    fprintf( stderr, "%u block%s, %u byte%s (%zu real)", 
	     this.total.blocks, 
	     this.total.blocks > 1 ? "s" : "", 
	     this.total.memory, 
	     this.total.memory > 1 ? "s" : "",
	     this.total.blocks*sizeof(struct memblk_t) + this.total.memory ); 
    STYLE_RESET( stderr ); 
#ifdef TRACE_MEM
    fprintf( stderr, " by ranking:\n" ); 

    trace_log( stderr ); 

    trace_flush();
#else 
    fprintf( stderr, ".\n" ); 
#endif
  }

  this.env.sys._free( this.env.out.oracle ); 
  this.env.sys._free( this.env.out.captured ); 

  this.env.sys._free( this.env.err.oracle ); 
  this.env.sys._free( this.env.err.captured ); 

  exit( this.total.total - this.total.passed ); 
}

/*
  Adapted from:
  https://github.com/ThrowTheSwitch/Unity
  MIT License, Copyright (c) <year> 2007-21
  by Mike Karlesky, Mark VanderVoord and Greg Williams.
 */

static int addr2line( char const * const program_name, char *func, void const * const addr ) {
  char addr2line_cmd[512] = {0};

  /* TODO: Find command for OSX __APPLE__. Maybe atos ?: */
  /* sprintf( addr2line_cmd, "atos -o %.256s %p",  program_name, addr ); */
  
  if ( func && *func ) { /* func+addr */
    sprintf( addr2line_cmd, "addr2line -i -f -p -e %.256s `printf %%x $((0x\\`nm -s %s | grep %s | head -1 | awk '{ print $1 }'\\`+%p))` 2> /dev/null",
	     program_name, program_name, func, addr );

  }
  else { /* absolute addr */
    sprintf( addr2line_cmd, "addr2line -i -f -p -e %.256s %p", 
	     program_name, addr );
  }

  /* fprintf( stderr, "EXECUTING '%s' on '%x', pfun = %s\n", program_name, addr, func );  */
  
  return system( addr2line_cmd );
}

#define MAX_STACK_FRAMES 256 /* Likely to be lost in recursion already */

static void *stack_traces[MAX_STACK_FRAMES];

void posix_print_stack_trace( void ) { 
  int   i, trace_size = 0; 
  char     **messages = (char **)NULL; 
  int           first = 1; 

  trace_size = backtrace( stack_traces, MAX_STACK_FRAMES );
  messages   = backtrace_symbols( stack_traces, trace_size );

  for ( i = 3; i < trace_size-2; i++ ) {
    char *c = messages[ i ];
    char *paddr, *pfun;
    void *text_addr;

    if ( NULL == c ) continue; 

    if ( *c != '.' && *c ) { 
      /* Uncomment (and remove 'continue' stmt) to get libc calls: */
      /* fprintf( stderr, "%s\n", c );  */
      continue;
    }
    
    if ( first ) { 
      first = 0; 
      fprintf( stderr, " >> In " );
    }
    else {
      fprintf( stderr, " Called by " );
    }

    while ( '(' != *c ) c++;
    *c = '\0';
    pfun = c+1;
    while ( '+' != *c ) c++;
    *c = '\0';
    paddr = c+1;
    c = paddr;
    while ( ')' != *c ) c++;
    *c = '\0';
    sscanf( paddr, "%p", &text_addr );

    
    TEST_STYLE_EMPH_LIGHT(); 
    
    addr2line( this.env.real_prog_name, pfun, text_addr ); 

    STYLE_RESET( stderr );

    if ( 3 == i ) {
      fprintf( stderr, "\n" );
    }
  }

  fprintf( stderr, "\n" );
  this.env.sys._free( messages );
}

#define SIG_CASE_PRINT( sig, msg )					\
  if ( this.env.verbose ) {						\
    fprintf( stderr, "\n" );						\
  }									\
  TEST_STYLE_ERROR();							\
  fprintf( stderr, " *** FATAL ERROR :: %s [%s] ***",			\
	   sig, msg );							\
  STYLE_RESET( stderr )

#define SIG_SUB_CASE__( sig, sub, msg ) case sub :			\
  SIG_CASE_PRINT( sig, msg );						\
  break

#define SIG_SUB_CASE( sig, sub, msg ) SIG_SUB_CASE__( #sig, sub, msg )
#define SIG_CASE( signal, msg )       SIG_SUB_CASE__( #signal, signal, msg )

#define HANDLE_SIGNAL( signal ) do {					\
    if ( sigaction( (signal), &sig_action, NULL ) != 0 ) {		\
      err( EXIT_FAILURE, "sigaction" );					\
    }									\
  } while ( 0 )

 
void set_signal_handler( void ) {

  struct sigaction sig_action;

  sig_action.sa_sigaction = posix_signal_handler;
  sig_action.sa_flags     = SA_SIGINFO | SA_RESETHAND;

  sigemptyset( &sig_action.sa_mask );

  HANDLE_SIGNAL( SIGSEGV );
  HANDLE_SIGNAL( SIGFPE  );
  HANDLE_SIGNAL( SIGALRM );
  HANDLE_SIGNAL( SIGINT  );
  HANDLE_SIGNAL( SIGILL  );
  HANDLE_SIGNAL( SIGTERM );
  HANDLE_SIGNAL( SIGABRT );
}

void posix_signal_handler( int sig, siginfo_t *siginfo, void *context ) { 

  alarm( 0 ); 

  /* During a test, stdout and stderr are redirected to
     /dev/null - so restore it back to /dev/tty first. 
  */
  freopen( "/dev/tty", "a", stdout ); 
  setvbuf( stderr, NULL, _IOLBF, 0 ); 
  freopen( "/dev/tty", "a", stderr ); 
  setvbuf( stderr, NULL, _IONBF, 0 ); 

  if ( SIGABRT != sig || !this.env.expect_abort ) { 
    fprintf( stderr, "%s\n", this.env.err.captured ); 

    switch ( sig ) { 
      SIG_CASE( SIGSEGV, "Segmentation fault" ); 
      SIG_CASE( SIGINT , "Got Ctrl-C" ); 
      SIG_CASE( SIGALRM, "Reached time-out" ); 
      SIG_CASE( SIGTERM, "Received termination request" ); 
      SIG_CASE( SIGABRT, "Aborted" ); 
    case SIGFPE:
      switch( siginfo->si_code ) {
	SIG_SUB_CASE( SIGFPE, FPE_INTDIV, "Integer divide by zero" );
	SIG_SUB_CASE( SIGFPE, FPE_INTOVF, "Integer overflow" );
	SIG_SUB_CASE( SIGFPE, FPE_FLTDIV, "Floating point divide by zero" );
	SIG_SUB_CASE( SIGFPE, FPE_FLTOVF, "Floating point overflow" );
	SIG_SUB_CASE( SIGFPE, FPE_FLTUND, "Floating point underflow" );
	SIG_SUB_CASE( SIGFPE, FPE_FLTRES, "Floating point inexact result" );
	SIG_SUB_CASE( SIGFPE, FPE_FLTINV, "Floating point invalid operation" );
	SIG_SUB_CASE( SIGFPE, FPE_FLTSUB, "Subscript out of range" );
      default:
	SIG_CASE_PRINT( "SIGFPE", "Arithmetic exception" );
	break;
      }
      break;
    case SIGILL:
      switch( siginfo->si_code ) {
	SIG_SUB_CASE( SIGILL, ILL_ILLOPC, "Illegal opcode" );
	SIG_SUB_CASE( SIGILL, ILL_ILLOPN, "Illegal operand" );
	SIG_SUB_CASE( SIGILL, ILL_ILLADR, "Illegal addressing mode" );
	SIG_SUB_CASE( SIGILL, ILL_ILLTRP, "Illegal trap" );
	SIG_SUB_CASE( SIGILL, ILL_PRVOPC, "Privileged opcode" );
	SIG_SUB_CASE( SIGILL, ILL_PRVREG, "Privileged register" );
	SIG_SUB_CASE( SIGILL, ILL_COPROC, "Coprocessor error" );
	SIG_SUB_CASE( SIGILL, ILL_BADSTK, "Internal stack error" );
      default:
	SIG_CASE_PRINT( "SIGILL", "Illegal instruction" );
	break;
      }
      break;
      
    default :
      break;
    }
    
    fputs( "\n\n", stderr );
  
    posix_print_stack_trace();
  }
  
  if ( SIGINT  == sig ) {
    this.suite.interrupted++; 
    this.total.interrupted++;
  }
  if ( SIGABRT == sig ) {
    this.suite.aborted++;
    this.total.aborted++;
  }
  if ( SIGALRM == sig ) {
    this.suite.timed_out++;
    this.total.timed_out++;
  }
  
// #ifdef __APPLE__
//   TEST_STYLE_INFO(); 
//   fprintf(stderr, "unitest.c can't handle signals details on MacOS. Please run on Linux for details.\n");
//   STYLE_RESET( stderr );
// #else
  if ( this.env.debug && ( SIGSEGV == sig || ( !this.env.expect_abort && SIGABRT == sig ) ) ) { 

    char debugger[ 1+STRLEN ];

    /*
      https://askubuntu.com/questions/41629/after-upgrade-gdb-wont-attach-to-process
     */
    snprintf( debugger, STRLEN, "sudo gdb -p %d", getpid() );

    system( debugger );

    TEST_STYLE_ERROR(); 
    fprintf( stderr, "\nGo fix your code now ;-)\n" );
    STYLE_RESET( stderr ); 
  }
// #endif

  if ( SIGSEGV == sig || SIGTERM == sig ) { 
    _Exit( EXIT_FAILURE );
  }

  siglongjmp( this.env.restart, 1 ); 
  
  UNUSED( context ); 
}

void    test_dump__( char *buff, char *orc, char *which ) { 
  FILE         *fp = NULL; 
  size_t       len = 0; 
  char   dump_file[ 1024+1 ]; 

  if ( !buff || !orc || !which || !*buff ) return;

  len = strlen( buff ); 
  
  snprintf( dump_file, 1024, "%s.%s", orc, which ); 

  fp = fopen( dump_file, "w" );
  if ( NULL == fp ) {
    FATAL( "Unable to open '%s' for writing: %s, aborting.\n", dump_file, strerror( errno ) );
  }

  if ( fwrite( buff, 1, len, fp ) < len ) { 
    FATAL( "Could not write buffer: %s, aborting.\n", strerror( errno ) ); 
  }
  
  fclose( fp ); 
}

char *orc_fmt( char *dst, const char *fmt, ... ) { 
  FILE    *fp; 
  va_list  va; 
  
  if ( !dst || !fmt ) return NULL; 

  va_start( va, fmt ); 
  
  fp = fopen( fmt, "r" ); 

  if ( fp ) { 
    char *buff = this.env.sys._malloc( this.env.TEST_SIZE ); 
    assert( buff ); 

    memset( buff, 0, this.env.TEST_SIZE ); 

    fread( buff, 1, this.env.TEST_SIZE, fp ); 

    fclose( fp ); 

    vsnprintf( dst, this.env.TEST_SIZE, buff, va ); 

    this.env.sys._free( buff ); 
  }
  else vsnprintf( dst, this.env.TEST_SIZE, fmt, va ); 

  va_end( va ); 

  return dst; 
}
  
static void print_test_usage( char *progname ) {
  fprintf( stderr, "\n SYNOPSIS\n\n" );
  fprintf( stderr, "  %s [OPTIONS]\n\n", progname );
  fprintf( stderr, " OPTIONS\n\n" );
  fprintf( stderr, "  -v | --verbose[=<level>]\tSet test verbosity <level> (dflt: 1)\n" );
  fprintf( stderr, "  -g | --debug  \t\tLaunch debugger on crash\n" );
  fprintf( stderr, "  -h | --help   \t\tPrint this help\n" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "  --test-timeout=<timeout>\tStop test after <timeout> secs (dflt: 2)\n" );
  fprintf( stderr, "  --test-oracles=<orc_dir>\tPrefix oracle file names with <orc_dir>/\n" ); 
  fprintf( stderr, "  --test-size=<orc_size>\tMax. oracle size, in bytes (dflt: 8K)\n" ); 
  fprintf( stderr, "\n" );
  fprintf( stderr, " NOTES\n\n" );
  fprintf( stderr, "  1. test-timeout, test-oracles and test-size supersede environment variables\n" ); 
  fprintf( stderr, "     TEST_TIMEOUT, TEST_ORACLES and TEST_SIZE, respectively.\n" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "  2. Integer values may be suffixed with k, K, m or M.\n" ); 
  fprintf( stderr, "\n" );
  fprintf( stderr, " EXAMPLE\n\n" );
  fprintf( stderr, "  Launch test in verbose mode and spin off debugger on crash (your best\n" );
  fprintf( stderr, "  bet):\n\n" );
  fprintf( stderr, "  $ %s -v -g\n", progname ); 
  fprintf( stderr, "\n" );
}

static char *get_runtime_string( char *dflt, char *opt_name, int argc, char *argv[] ) { 
  char      *ret = dflt; 
  int          i; 

  if ( opt_name ) { 
    char     *from_env = NULL; 
    char      env_name[ 1024 ]; 
    char opt_real_name[ 1024 ];

    for ( i = 0 , ret = opt_name ; *ret || ( env_name[ i ] = '\0' ) ; i++ , ret++ ) { 
      switch ( *ret ) {
      case '-' : env_name[ i ] = '_';
	break; 
      default  : env_name[ i ] = toupper( *ret ); 
	break; 
      }
    }

    from_env = getenv( env_name ); 

    if ( from_env ) ret = from_env; 

    snprintf( opt_real_name, 1023, "--%s=", opt_name ); 

    for ( ret = dflt , i = 1 ; i < argc ; i++ ) { 

      if ( argv[ i ] == strstr( argv[ i ], opt_real_name ) ) {
	return argv[ i ] + strlen( opt_real_name );
      }
    }
  }

  return ret; 
}

static long int get_runtime_int( char *dflt, char *opt_name, int argc, char *argv[] ) { 
  char      *val = get_runtime_string( dflt, opt_name, argc, argv ); 
  char      *end = NULL; 
  long int   ret = 0; 

  errno = 0; 

  ret   = strtol( val, &end, 0 ); 


  if ( ( errno == ERANGE && ( ret == LONG_MAX || ret == LONG_MIN ) ) 
      || ( errno != 0 && ret == 0 ) ) { 
    fprintf( stderr, "Unable to read integer: %s, aborting.\n", strerror( errno ) ); 
    abort(); 
  }

  if ( 0 == ret ) ret = 1; 

  switch ( *end ) { 
  case 'k' : ret *= 1e3;       break; 
  case 'K' : ret *= 1024;      break; 
  case 'm' : ret *= 1e6;       break; 
  case 'M' : ret *= 1024*1024; break; 
  default : break; 
  }

  return ret; 
}

void    unit_test( int argc, char *argv[] ) {
  int      i = 0; 
  char  *ptr = NULL; 

#ifdef TRACE_MEM
  memset( blocks, 0, sizeof( blocks ) ); 
#endif
  
  this.env.sys._malloc  = dlsym( RTLD_NEXT, "malloc" ); 
  this.env.sys._realloc = dlsym( RTLD_NEXT, "realloc" ); 
  this.env.sys._free    = dlsym( RTLD_NEXT, "free" ); 

  memcpy( this.env.real_prog_name, argv[ 0 ], 1+strlen( argv[ 0 ] ) ); 
  
  this.env.TEST_ORACLES = get_runtime_string( ".", "test-oracles", argc, argv ); 
  this.env.TEST_TIMEOUT = get_runtime_int( "2", "test-timeout", argc, argv ); 
  this.env.TEST_SIZE    = get_runtime_int( "8K", "test-size", argc, argv ); 

  this.env.out.oracle   = this.env.sys._malloc( this.env.TEST_SIZE ); 
  assert( this.env.out.oracle ); 
  this.env.out.captured = this.env.sys._malloc( this.env.TEST_SIZE ); 
  assert( this.env.out.captured ); 
  
  this.env.err.oracle   = this.env.sys._malloc( this.env.TEST_SIZE ); 
  assert( this.env.err.oracle ); 
  this.env.err.captured = this.env.sys._malloc( this.env.TEST_SIZE ); 
  assert( this.env.err.captured ); 

  memset( this.env.out.oracle, 0, this.env.TEST_SIZE ); 
  memset( this.env.err.oracle, 0, this.env.TEST_SIZE );	

  for ( ptr = strtok( argv[ 0 ], "/" ) ; ptr ; this.env.prog_name = ptr, ptr = strtok( NULL, "/" ) ) { 
    if ( argv[ 0 ] != ptr ) ptr[ -1 ] = '/'; 
  }

  set_signal_handler(); 

  for ( i = 1 ; i < argc ; i++ ) { 

    if ( !strcmp( "-h", argv[ i ] ) || !strcmp( "--help", argv[ i ] ) ) { 
      print_test_usage( this.env.prog_name ); 
      exit( EXIT_SUCCESS ); 
    }
    
    if ( '-' == argv[ i ][ 0 ] ) { 
      if ( 'v' == argv[ i ][ 1 ] ) { 
	this.env.verbose = 1; 
	if ( '=' == argv[ i ][ 2 ] ) { 
	  unsigned level = 0; 
	  if ( 1 == sscanf( argv[ i ] + 3, "%u", &level ) ) this.env.verbose = level; 
	}
      }
      if ( 'g' == argv[ i ][ 1 ] ) { 
	this.env.debug   = 1; 
      }
    }
    if ( !strncmp( "--verbose", argv[ i ], 9 ) ) { 
      this.env.verbose = 1; 
      if ( '=' == argv[ i ][ 9 ] ) { 
	unsigned level = 0; 
	if ( 1 == sscanf( argv[ i ] + 10, "%u", &level ) ) this.env.verbose = level; 
      }
    }
    if ( !strcmp( "--debug", argv[ i ] ) ) { 
      this.env.debug   = 1; 
    }
  }

  atexit( test_finish );
}
