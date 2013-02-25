/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4
 * searchdb.c - database search routine.
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
#include <dbfunc.h>
#include <screen.h>
#include <dbio.h>
#include <dbsearch.h>
#include <util.h>
#ifdef TEST
#define  DBG_FINDRECORDS  1
#endif
extern char dbasedir[];		/* path to the INDEXDIR         */
static wchar_t wpattern[BUFSIZ] = { L"" };
/* The wchar string we recieve the searchpattern in.
   soon converted into re_pattern */
static UChar *re_pattern = NULL;
/* contains the regexp that we feed into icu. 
   malloced and freed every time. NOW.
*/

static int inverted_match=0;
/* set through setter */
static int igncase = 0;		/* non-zero if -i flag given        */
/* set through setter */

/* Called from main and the parsing of commandline */
void
set_ignore_case(void)
{
	igncase = 1;
}

/* Called from main and the parsing of commandline */
void
set_inverted(void) 
{
    inverted_match=1 ;
}
/* just a getter for the pattern                            */
UChar *pattern(void)
{
	return re_pattern;
}

/* called from finish of main.c for the case that 
   something went wrong in the regexp
*/
void releasePattern(void)
{
	if (re_pattern != NULL)
		free(re_pattern);
	re_pattern = NULL;
}
/* Just returns the pattern */
UChar *utf8Pattern(void)
{
	return re_pattern;
}
/* says so if we have a pattern */
int hasPattern(void)
{
	if (re_pattern == NULL) {
		return 0;
	} else {
		return 1;
	}
}

/* makes a unicode pattern edible by ICU, by a pattern gotten from the commandline. */
int32_t patternFromUtf8(char *utf8pattern)
{
	size_t slen = strlen(utf8pattern);

	int32_t ucslen = 0, uchCap = slen + 1;

	re_pattern = (UChar *) ymalloc(((size_t) uchCap * sizeof(UChar)),"patternFromUtf8","re_pattern");
	ucslen = unicodeFromUTF8(re_pattern, uchCap, utf8pattern, slen);
	return ucslen;
}

/* makes a unicode pattern edible by ICU, by a pattern gotten from the search screen. */
int32_t patternFromWcs(wchar_t * wcharPattern)
{
	size_t wlen = wcslen(wcharPattern);

	int32_t uchCap = wlen * 4 + 1, ucslen = 0;

	re_pattern = (UChar *) ymalloc(((size_t) uchCap * sizeof(UChar)),"patternFromWcs","re_pattern");
	ucslen = unicodeFromWcs(re_pattern, uchCap, wcharPattern, wlen);
	return ucslen;

}

/* transforms a regexp  from the commandline into unicode   */
/* search for entries using a RE                            */
/* Updated: Gives msg about malformed RE can re-edit RE.   */

/* TODO: KEEP prev searchstring at all times for re-edit.   */

void find_entry(void)
{
	register int i;

	int srch_res;
	int pat_result = -1;
    initHeading() ;

	/* Clear screen and prompt for pattern to search for.    */
	/* Creates the line buffer to edit in here... */

	clear();
    paintHeading(" SEARCH ") ;

	while (pat_result == -1) {
		prompt_str(LINES / 2, 0, "Pattern to search for: ", wpattern);
        if (wcslen(wpattern) == 0 ) {
            continue ;
        }
         

		int32_t ucslen = patternFromWcs(wpattern);

		/* search_db will set DB_PRINT in matches.              */
		/* and return non-zero if anything matched.             */
		srch_res = 0;
		if ((srch_res = search_db(re_pattern, ucslen)) == 1) {
			pat_result = 0;
			/* Display entries that matched.                    */
			disp_entries();

			/* Clear DB_PRINT flags for next search.            */
			for (i = 0; i < dbentries; i++) {
				db[i].db_flag &= ~DB_PRINT;
            }
		} else if (srch_res == -1) {

			/* There was an error in the RE.                     */
			prompt_char(LINES / 2, 0,
				    "There was an error in the reg-exp pattern, type RETURN to continue: ",
				    "\n\r");
            move(LINES/2,0) ;
            clrtoeol() ;
			/* TODO: something about try again? henvisning til man pages. */
		} else {
			/* Nothing matched.  Tell user.                     */
			pat_result = 0;
			prompt_char(LINES / 2, 0,
				    "No entries match pattern, type RETURN to continue: ",
				    "\n\r");
		}
	}
    free(re_pattern ) ;
    re_pattern = NULL ;
	wpattern[0] = (wchar_t) '\0';
}

/*
 * search_db - search the database for the pattern.
 *
 * #2) I have made the search_db() function return -1 when
 * nothing is found due to a malformed reg_exp(), this
 * enables catching the case, and editing The expression in
 * the db_menu() function.
 *
 */

/* TODO: if there is something you don't want to be searchable
 * put a '#' in front of it. (Documentation)
 *
 *  usage of the icu library's regexp function.
 *
 *  We start by initiating a new regexp.
 *  We bail out one way or another if errant regexp pattern.
 *  We feed the regexp some text.
 *  We check for matches.
 *
 *
 */
int search_db(UChar * src_pattern, int32_t plen)
{
	uint32_t flags = 0;

	UErrorCode status;

	static URegularExpression *new_regex = NULL;

	UParseError pe;		/* position in pattern with error                   */

	int hadMatch = 0;

	register int i, j;

	if (igncase == 1)
		flags |= UREGEX_CASE_INSENSITIVE;

	/* Comile the regular expression.                               */
	status = U_ZERO_ERROR;
	new_regex = uregex_open(src_pattern, plen, flags, &pe, &status);
	if (U_FAILURE(status)) {
        return -1 ; /* Returns with -1 to signal an error in find entry */
	}
	/* For all entries...                                               */

	for (i = 0; i < dbentries; i++) {
		/* For each line in the entry...                                */

		if (db[i].db_flag & DB_PRINT) {
			db[i].db_flag &= 0XFFFD;
		}

		for (j = 0; j < idx.idx_nlines; j++) {
			/* If this line is not to be searched, skip it.             */
			if (idx.idx_search[j] == 0)
				continue;

			/* If we get a match, mark this entry * as printable.       */

			if (db[i].db_lens[j] != 0) {	/* is something in the string */
				status = U_ZERO_ERROR;
				uregex_setText(new_regex, db[i].db_lines[j], -1,
					       &status);
				if (!U_SUCCESS(status)) {
                        y_icuSimpleError(
						"Index: something erred during  SETTING TEXT  of regexp" ,status);
				} else {
					if (uregex_find(new_regex, 0, &status)
					    == TRUE) {
						/* if( U_SUCCESS(status) ) { */
                        if (!inverted_match) {
						    db[i].db_flag |= DB_PRINT;
						    hadMatch |= 1;
                        }
					} else if (inverted_match) {
					    db[i].db_flag |= DB_PRINT;
					    hadMatch |= 1;
                    }
				}
			}
		}
	}
	uregex_setText(new_regex, 0, 0, &status);
	uregex_close(new_regex);
	new_regex = NULL;
	/* Return whether or not we found anything.                         */
	return (hadMatch);
}
