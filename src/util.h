/* * util.h - utility routines.
*
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 */
#ifndef __INDEX_UTIL__
#define __INDEX_UTIL__

typedef int (*compFunc)(const void *, const void *) ;

void            setProgramName(void ) ;
char        *   getCollation(void) ;
void            setCollation( char * theLocale ) ;
void            releaseCollation(void) ;
char        *   getProgramName( void ) ;
void 			yerror( int strIndex, const char *handler, const char *variable, int ourErrCode ) ;
char        *   getMyLocale(void) ;
void            releaseProgramName(void) ;
void            set_dbase_dir(void) ;
void 		*	yrealloc( void *ptr, size_t sz, const char *fromHandler, const char * forVariable ) ;
void        *	ymalloc( size_t sz,const char *fromHandler, const char *forVariable ) ;
void            memerrmsg( const char *func_name ) ;
void            factionerrmsg( const char *file_name, const char *faction ) ;
wchar_t     *   savewstr( const wchar_t *wstr, const char *handler, const char *variable ) ;
wchar_t     *   makewstr( const char *handler, const char *variable  ) ;
char        *   savestr(const char *str, const char *handler, const char *variable );
#endif
