#ifndef __DBSEARCH__
#define __DBSEARCH__
void 		set_ignore_case(void) ;
void 		set_inverted(void) ;

void        setIgnCase(void) ;
UChar   *   pattern(void) ;
int32_t     patternFromUtf8( char *utf8Pattern ) ;
int32_t     patternFromWcs( wchar_t *wcharPattern ) ;
int         hasPattern(void) ;
UChar       *utf8Pattern(void) ;
void        releasePattern(void) ;
void        find_entry(void) ;
int         search_db(UChar *pattern, int32_t plen) ;
#endif
