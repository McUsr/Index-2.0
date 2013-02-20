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
										     /* extern dbrecord *db;   *//* array of database entries    */
									   /* extern idxfile idx; *//* index definition structure   */
/* TODO: bare ha en error message string på say 256 chars  */
extern char dbasedir[];		/* path to the INDEXDIR         */
static wchar_t wpattern[BUFSIZ] = { L"" };

static UChar *re_pattern = NULL;

static int inverted_match=0;
static int igncase = 0;		/* non-zero if -i flag given        */

void
set_ignore_case(void)
{
	igncase = 1;
}

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

void releasePattern(void)
{
	if (re_pattern != NULL)
		free(re_pattern);
	re_pattern = NULL;
}

UChar *utf8Pattern(void)
{
	return re_pattern;
}

int hasPattern(void)
{
	if (re_pattern == NULL) {
		return 0;
	} else {
		return 1;
	}
}

/* setter from UTF-8                                        */
int32_t patternFromUtf8(char *utf8pattern)
{
	size_t slen = strlen(utf8pattern);

	int32_t ucslen = 0, uchCap = slen + 1;

	re_pattern = (UChar *) ymalloc(((size_t) uchCap * sizeof(UChar)),"patternFromUtf8","re_pattern");
	ucslen = unicodeFromUTF8(re_pattern, uchCap, utf8pattern, slen);
	return ucslen;
}

/* setter from wide characters.                             */
int32_t patternFromWcs(wchar_t * wcharPattern)
{
	size_t wlen = wcslen(wcharPattern);

	int32_t uchCap = wlen * 2 + 1, ucslen = 0;

	re_pattern = (UChar *) ymalloc(((size_t) uchCap * sizeof(UChar)),"patternFromWcs","re_pattern");
	ucslen = unicodeFromWcs(re_pattern, uchCap, wcharPattern, wlen);
	return ucslen;

}

/* TODO: fill in details. */
/* transforms a regexp  from the commandline into unicode   */
/* Hvordan gjør vi dette?
 * Kanskje det er best å bare ha en variabel vi putter ting inn i,
 * fra begge kanter?
 *
 * Vi henter over konvertering vi har klar ifra biblioteket
 * For det som kommer fra kommandolinje.
 *
 *
 * Vi initialiserer variablen til NULL til å begynne med.
 * Når vi skal gjøre ting, så freer vi den før vi setter nytt innhold
 * free rutinen er en egen free rutine, som vi også kan kalle opp når
 * vi skal avslutte?
 */

/* search for entries using a RE                            */
/* Updated: Gives msg about malformed RE, can re-edit RE.   */

/* TODO: KEEP prev searchstring at all times for re-edit.   */
/* TODO: Deliver search as UChar *                          */
/* TODO: Ta hensyn til case sensitive flag. */

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

		int32_t ucslen = patternFromWcs(wpattern);

		/* search_db will set DB_PRINT in matches.              */
		/* and return non-zero if anything matched.             */
		srch_res = 0;
		if ((srch_res = search_db(re_pattern, ucslen)) == 1) {
			pat_result = 0;
			/* Display entries that matched.                    */
			disp_entries();

			/* Clear DB_PRINT flags for next search.            */
			for (i = 0; i < dbentries; i++)
				db[i].db_flag &= ~DB_PRINT;
		} else if (srch_res == -1) {

			/* There was an error in the RE.                     */
			mvaddwstr((LINES / 2) - 2, 0, wpattern);
			prompt_char(LINES / 2, 0,
				    "There was an error in the reg-exp pattern, type RETURN to continue: ",
				    "\n\r");
			/* TODO: something about try again? henvisning til man pages. */
		} else {
			/* Nothing matched.  Tell user.                     */
			pat_result = 0;
			prompt_char(LINES / 2, 0,
				    "No entries match pattern, type RETURN to continue: ",
				    "\n\r");
		}
	}
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
/* bug i edit entry når står i nullte posisjon og trykker backspace. */
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
		fprintf(stderr,
			"something erred during compilation of regexp\n");
		return -1;
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

					fprintf(stderr,
						"something erred during  SETTING TEXT  of regexp\n");
					/* icuFunctionError(p, "uregex_setText", status); */
					return -1;
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
