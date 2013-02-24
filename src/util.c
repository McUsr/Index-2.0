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
#include <screen.h> 
#include <util.h>

static int hasError=0;
static const char *errstrings[] = {
    "%s: Can't allocate  memory in : \"%s\" for \"%s\".",
    "%s: Can't reallocate  memory in : \"%s\" for \"%s\".",
    "%s: Problems converting with ICU in : \"%s\" for \"%s\".",
    "%s: Problems converting between mbs and wcs characters in : \"%s\" for \"%s\".",
    "%s: Path too long when getting the basename  in : \"%s\" for the \"%s\" variable.",
    "%s: Path is root when getting the basename  in : \"%s\" for the \"%s\" variable.",
    "%s: Path is $PWD when getting the basename  in : \"%s\" for the \"%s\" variable.",
    "%s: Cannot find System Variable in : \"%s\" for the \"%s\" variable.",
    "%s: Cannot create file in : \"%s\" at the path \"%s\".",
    "%s: Cannot rename file in : \"%s\" to the name \"%s\".",
    "%s: Cannot find file in : \"%s\" with the name \"%s\".",
    "%s: Cannot open file for reading in : \"%s\" with the name \"%s\".",
    "%s: Cannot read file in : \"%s\" with the name \"%s\".",
    "%s: Cannot write file in : \"%s\" with the name \"%s\".",
	"%s: ICU collater == NULL! ICU in %s for collator : %s",
	"%s: ICU collater can't be created   in %s for collator : %s",
    "%s: cannot open directory in \"%s\" for the path %s",
    "%s: in %s: cannot execute/find the program  \"%s\".",
	"%s: in %s: cannot fork a process for %s",
	"%s: ICU string conversion from UTF8 erred in %s for variable : %s",
	"%s: ICU preflight string conversion from UTF8 erred in %s for variable : %s",
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
/*
    gets the ICU ERROR CODE, and creates the string in question.
    I think the good way to do this is to set the ICU-error code
    before we call yerror, when yerror eyes the ERROR code, it knows
    it has to call us, and we'll provide the error string going with
    that code.
    TODO: explain how the look for/create database logic works.
*/
void
y_icuerror( int strIndex, const char *handler, const char *variable, UErrorCode errcode )
{
    char buf[80]="" ;
    strcpy(buf,variable );
    strcat (buf," ICU error: " ) ;
    strcat(buf, u_errorName(errcode)) ;
    yerror(strIndex, handler, buf,YX_EXTERNAL_CAUSE ) ;
}
void
y_icuSimpleError( const char *errmsg,  UErrorCode errcode )
{
    char buf[80]="";
    strcpy(buf,errmsg );
    strcat (buf," ICU error: " ) ;
    strcat(buf, u_errorName(errcode)) ;
    ysimpleError( buf, YX_EXTERNAL_CAUSE ) ;
    
}
/* 
	We do print the program name when we interrupt the usual flow of the program.
    Enhancements:

    Ugly, but some errors needs only one line, other errors, can provide just one
    variable, some doesn't need to provide a variable at all.

    Hence testing for NULL values as parameters.

    The errorcode must be supplied as such at all times.

    We'll also have to identify which error is supplied by ICU, through the error
    constants, and get those into the program as well.

    Some errors also needs one line, that is the NULL,NULL option.


    Other stuff the error system is taking care of, is  that we are governed by init curses
    as to whether we should send the error message to the screen and die after having gotten
    a confirmation, or if we are to just die with the error message, this is the functionality
    we'll have to adhere to no matter how we are going to implement the system.

*/
/*
TODO:
    take the ICU-ERROR CODES INTO ACCOUNT, AND NOT PRINT THE str_error message then
    OR: fork out a separate hander for this. or use the y_icu_simpleerror.
    --> this is cleaner.
*/

void
yerror( int strIndex, const char *handler, const char *variable, int ourErrCode )
{
	char errL1[BUFSIZ];
	char errL2[BUFSIZ];
    int errL = 2 ;
    if (!hasError) {
        hasError = 1 ;
	    sprintf(errL1,(const char *)errstrings[strIndex] ,pname,handler, variable) ; 
        if ((strIndex != YICU_CONV_ERR ) && (strIndex != YICU_COLLATO_ERR ) 
                && (strIndex != YICU_CRECOLL_ERR ) && (strIndex != YICU_CNVFUTF8_ERR )
                && (strIndex != YICU_CNVPREUTF8_ERR ) ) {
	        sprintf(errL2,"%s: error number : %d, message: %s.",pname,errno,strerror(errno) ) ;
            errL = 1 ;
        }
        /* here is the place we call stuff if curses is inited */
        if (curses_active() ) {
            register int y,x ;
		    getyx(curscr, y, x);
	        mvaddstr(y++, 5, errL1);
            if (errL==2) {
	            mvaddstr(y++, 5, errL2);
            }
           prompt_char(y,5,"Hit any key to exit Index",NULL) ; 
            /* we take down curses here */
        }         
        finish(0) ; 
        fprintf(stderr,"%s\n",errL1) ;
        if ( errL == 2 ) {
            fprintf(stderr,"%s\n",errL2) ;
        }
        exit(ourErrCode ) ;
   }
}
/* for very simple error messages: behaves like yerror, but there is no error
   table to look up into.
*/

void
ysimpleError( const char *errmsg, int ourErrCode )
{
   fprintf(stderr,"%s\n",errmsg) ;
   finish(0) ; 
    if (ourErrCode) {
        exit(ourErrCode ) ;
    }
}

