/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4:ro
 *
 * util.c - utility routines for index program.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * Version 2.0
 * Updated/Ported to Mac Os X 10.6.8 Snow Leopard and onwards.
 * This version acknowledges Mac OsX's unicode filenames,
 * Utf-8, and sorting with respect to locale.
 * Tommy Bollman/McUsr 2013.02.06
 *
 */
#include <common.h>
#include <unicodeToUtf8.h>
#include <initcurses.h>
#include <defs.h>
#include <util.h>

static int hasError=0;
static const char *errstrings[] = {
    "%s: Can't allocate  memory in : \"%s\" for \"%s\".",
    "%s: Can't reallocate  memory in : \"%s\" for \"%s\"."
    "%s: Problems converting with ICU in : \"%s\" for \"%s\"."
    "%s: Problems converting between mbs and wcs characters in : \"%s\" for \"%s\"."
    } ;

static char *pname = NULL;	/* program name     */

/* static char *    curLocale; */
static char *curCollation = NULL;	/* The collation   */

static char *myLocale = NULL;

char
*getCollation(void)
{
	return curCollation;
}

void
releaseCollation(void)
{
	if (curCollation != NULL) {
		free(curCollation);
		curCollation = NULL;
	}
}

char
*getMyLocale(void)
{
	return myLocale;
}

/* Todo these strings should  be freed */
void
setCollation(char *theLocale)
{
	char *b = NULL, *t = NULL;
    /* The full locale, we don't take care
    of any locally set collations. */
	myLocale = theLocale;
	b = savestr(theLocale,"setCollation","b");
	t = b;
	while (*(t++) != '.') ;
	*t = '\0';
	curCollation = savestr(b,"setCollation","curCollation");
	free(b);
	b = t = NULL;
}

/* needs a list over what to free in here */
/* void freeData() {

}
*/
char
*getProgramName(void)
{
	return pname;
}

void
releaseProgramName(void)
{
	pname = NULL;
}
/* 
	We do print the program name when we interrupt the usual flow of the program.
*/
void yerror( int strIndex, const char *handler, const char *variable, int ourErrCode )
{
	char errl1[BUFSIZ];
	char errl2[BUFSIZ];
    if (!hasError) {
        hasError = 1 ;
	    sprintf(errl1,(const char *)errstrings[strIndex] ,pname,handler, variable) ; 
	    sprintf(errl2,"%s: error number : %d, message: %s.",pname,errno,strerror(errno) ) ;
        /* here is the place we call stuff if curses is inited */
        if (curses_active() ) {
            ;
            /* we take down curses here */
        }         
        fprintf(stderr,"%s\n",errl1) ;
        fprintf(stderr,"%s\n",errl2) ;

        finish(ourErrCode) ; 

   }
	
}

void
setProgramName(void)
{
    char prgname[] = "index 2.0" ;
	pname = savestr(prgname,"setProgramName","pname");
}

wchar_t
*savewstr(const wchar_t * wstr,const char *handler, const char *variable)
{

	wchar_t *w;
    size_t newwcslen = (wcslen(wstr) + 1 ) * sizeof(wchar_t) ;

	w = (wchar_t *) ymalloc(newwcslen,handler,variable) ; 
	wcscpy(w, wstr);
	return (w);
}

wchar_t
*makewstr(const char *handler, const char *variable)
{
    size_t newwcslen = BUFSIZ * sizeof(wchar_t) ;
	wchar_t *w = (wchar_t *) ymalloc(newwcslen,handler,variable);
    memset(w,0,newwcslen ) ;
	return w;
}

/* save a string in dynamically allocated memory.           */
char
*savestr(const char *str, const char *handler, const char *variable)
{
	char *s;
    
	s = (char *)ymalloc((strlen(str) + 1),handler,variable); 

	strcpy(s, str);
	return (s);
}

void *
yrealloc( void *ptr, size_t sz, const char *fromHandler, const char * forVariable )
{
   void *p = realloc(ptr, sz ) ;
   if (p == NULL ) {
        yerror( YREALLOC_ERR, fromHandler, forVariable, YX_EXTERNAL_CAUSE ) ;
   }
   return p ;
}

/* allocates memory.
    if the request for memory can't be satisified
    then depending on whether we are in the ui or not,
    a failure message will be printed there.
    A failure message will be printed at exit anyway.
*/

void *
ymalloc( size_t sz,const char *fromHandler, const char *forVariable )
{
   void *p = malloc(sz ) ;
   if (p == NULL ) {
        yerror( YMALLOC_ERR, fromHandler, forVariable, YX_EXTERNAL_CAUSE ) ;
   }
   return p ;
}

/* prints an error message about lacking  memory in a       */
/* function, and dies.                                      */
/* TODO:
 remove exit code for conformity.
 check if curses is loaded
*/
void
memerrmsg(const char *func_name)
{
	char emi[BUFSIZ];
	sprintf(emi, "%s: Out of memory in : \"%s\":\n", getProgramName(),
		func_name);
    if (curses_active()) {
	    reset_modes();
    }
	perror(emi);
	exit(1);
}

void
factionerrmsg(const char *file_name, const char *faction)
{
	char emi[BUFSIZ];

	sprintf(emi, "%s: cannot %s \"%s\":", getProgramName(), faction,
		file_name);
    if (curses_active()) {
	    reset_modes();
    }
	perror(emi);
	exit(1);
}
