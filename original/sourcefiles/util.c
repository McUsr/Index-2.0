#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/util.c,v 1.1 89/08/09 11:07:12 davy Exp $";
#endif
/*
 * util.c - utility routines for index program.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	util.c,v $
 * Revision 1.1  89/08/09  11:07:12  davy
 * Initial revision
 * 
 */
#include <curses.h>
#include <stdio.h>
#include "defs.h"

/*
 * set_dbase_dir - set the path to the database directory.
 */
set_dbase_dir()
{
	char *s;
	char *getenv();

	/*
	 * Look for it in the environment.
	 */
	if ((s = getenv("INDEXDIR")) != NULL) {
		strcpy(dbasedir, s);
		return;
	}

	/*
	 * Otherwise, it's in the home directory.
	 */
	if ((s = getenv("HOME")) == NULL) {
		fprintf(stderr, "%s: cannot get home directory.\n", pname);
		exit(1);
	}

	/*
	 * Make the name.
	 */
	sprintf(dbasedir, "%s/%s", s, INDEXDIR);
}

/*
 * dbsort - comparison routine for qsort of database entries.
 */
dbsort(a, b)
struct dbfile *a, *b;
{
	register int i, n;

	/*
	 * Sort invalid entries to the end.
	 */
	if ((a->db_flag & DB_VALID) == 0) {
		if ((b->db_flag & DB_VALID) == 0)
			return(0);

		return(1);
	}

	if ((b->db_flag & DB_VALID) == 0)
		return(-1);

	/*
	 * Sort on first field, then try secondary fields.
	 */
	n = 0;
	for (i=0; (i < idx.idx_nlines) && (n == 0); i++)
		n = strcmp(a->db_lines[i], b->db_lines[i]);
	
	return(n);
}

/*
 * error - reset tty modes and print an error message.
 */
error(fmt, arg1, arg2, arg3)
char *fmt, *arg1, *arg2, *arg3;
{
	reset_modes();

	fprintf(stderr, fmt, arg1, arg2, arg3);
}

/*
 * savestr - save a string in dynamically allocated memory.
 */
char *
savestr(str)
char *str;
{
	char *s;
	char *malloc();

	if ((s = malloc(strlen(str) + 1)) == NULL) {
		reset_modes();

		fprintf(stderr, "%s: out of memory.\n", pname);
		exit(1);
	}

	strcpy(s, str);
	return(s);
}

/*
 * byebye - exit.
 */
byebye()
{
	register char c;
	register int x, y;

	/*
	 * If the database is modified, see if they really
	 * mean to exit without saving.
	 */
	if (dbmodified) {
		getyx(curscr, y, x);
		c = prompt_char(y, 0,
				"Really exit without saving? ",
				"YyNn");

		if ((c == 'n') || (c == 'N'))
			return;
	}

	/*
	 * Reset tty modes and exit.
	 */
	reset_modes();
	exit(0);
}

/*
 * usage - print a usage message.
 */
usage()
{
	fprintf(stderr, "Usage: %s [-f filter] [-i] [database] [pattern]\n",
		pname);
	exit(1);
}
