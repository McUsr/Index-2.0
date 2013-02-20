#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/searchdb.c,v 1.1 89/08/09 11:06:59 davy Exp $";
#endif
/*
 * searchdb.c - database search routine.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	searchdb.c,v $
 * Revision 1.1  89/08/09  11:06:59  davy
 * Initial revision
 * 
 */
#include <curses.h>
#include <ctype.h>
#include <stdio.h>
#include "defs.h"

/*
 * search_db - search the database for the pattern.
 */
search_db(pattern)
char *pattern;
{
	int code = 0;
	char *re_comp();
	char buf[BUFSIZ];
	register int i, j;
	register char *p, *q;

	/*
	 * If we're ignoring case, convert the pattern
	 * to all lower case.
	 */
	if (igncase) {
		for (p = pattern; *p != NULL; p++) {
			if (isupper(*p))
				*p = tolower(*p);
		}
	}

	/*
	 * Compile the regular expression.
	 */
	if (re_comp(pattern) != NULL)
		return(0);

	/*
	 * For all entries...
	 */
	for (i=0; i < dbentries; i++) {
		/*
		 * For each line in the entry...
		 */
		for (j=0; j < idx.idx_nlines; j++) {
			/*
			 * If this line is not to be searched,
			 * skip it.
			 */
			if (idx.idx_search[j] == 0)
				continue;

			/*
			 * If ignoring case, copy the line an
			 * convert it to lower case.  Otherwise,
			 * use it as is.
			 */
			if (igncase) {
				p = db[i].db_lines[j];
				q = buf;

				while (*p != NULL) {
					*q++ = isupper(*p) ? tolower(*p) : *p;
					p++;
				}

				*q = '\0';

				/*
				 * If we get a match, mark this entry as
				 * printable.
				 */
				if (re_exec(buf)) {
					db[i].db_flag |= DB_PRINT;
					code = 1;
				}
			}
			else {
				/*
				 * If we get a match, mark this entry
				 * as printable.
				 */
				if (re_exec(db[i].db_lines[j])) {
					db[i].db_flag |= DB_PRINT;
					code = 1;
				}
			}
		}
	}

	/*
	 * Return whether or not we found anything.
	 */
	return(code);
}

