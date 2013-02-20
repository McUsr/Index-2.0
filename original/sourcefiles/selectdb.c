#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/selectdb.c,v 1.1 89/08/09 11:07:06 davy Exp $";
#endif
/*
 * selectdb.c - database selection routines.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	selectdb.c,v $
 * Revision 1.1  89/08/09  11:07:06  davy
 * Initial revision
 * 
 */
#include <sys/param.h>
#include <sys/dir.h>
#include <curses.h>
#include <stdio.h>
#include "defs.h"

/*
 * select_db - allow the user to select a database from the list of databases
 *	       he has, or to create a new database.
 */
char *
select_db()
{
	char dbname[MAXPATHLEN];
	char *dblist[MAXDBFILES];
	register int ndbs, i, row, col, spread;

	/*
	 * Load the list of databases the user has.
	 */
	ndbs = load_dblist(dblist);
	spread = (ndbs + 3) / 4;

	/*
	 * Set tty modes, clear screen.
	 */
	set_modes();
	clear();

	/*
	 * Print the list of databases in four columns.
	 */
	for (row = 0; row < spread; row++) {
		for (col = 0; col < 4; col++) {
			i = col * spread + row;

			if (dblist[i])
				mvaddstr(row, col * COLS/4, dblist[i]);
		}
	}

	*dbname = '\0';

	/*
	 * Prompt for the name of a database.
	 */
	while (*dbname == '\0')
		prompt_str(spread+2, 0, "Select a database: ", dbname);

	/*
	 * If the database exists, return its name.
	 */
	for (i = 0; i < ndbs; i++) {
		if (!strcmp(dbname, dblist[i]))
			return(savestr(dbname));
	}

	/*
	 * Doesn't exist - create it.
	 */
	create_db(dbname);
	return(savestr(dbname));
}

/*
 * load_dblist - load up a list of the databases the user has.
 */
load_dblist(dblist)
char **dblist;
{
	DIR *dp;
	int ndbs;
	char *rindex();
	register char *s;
	extern int compare();
	register struct direct *d;

	ndbs = 0;

	/*
	 * Open the database directory.
	 */
	if ((dp = opendir(dbasedir)) == NULL) {
		fprintf(stderr, "%s: cannot open \"%s\".\n", pname, dbasedir);
		exit(1);
	}

	/*
	 * Read entries from the directory...
	 */
	while ((d = readdir(dp)) != NULL) {
		/*
		 * Search for a "." in the name, which marks
		 * the suffix.
		 */
		if ((s = rindex(d->d_name, '.')) == NULL)
			continue;

		/*
		 * If this is an index definition file, save its
		 * name.
		 */
		if (!strcmp(s, IDXFILE_SUFFIX)) {
			if (ndbs < MAXDBFILES) {
				*s = '\0';
				dblist[ndbs++] = savestr(d->d_name);
			}
		}
	}

	/*
	 * Sort the list.
	 */
	qsort(dblist, ndbs, sizeof(char *), compare);
	closedir(dp);

	return(ndbs);
}

/*
 * compare - comparis routine for qsort of dblist.
 */
static int
compare(a, b)
char **a, **b;
{
	return(strcmp(*a, *b));
}
