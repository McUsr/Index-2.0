#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/main.c,v 1.1 89/08/09 11:06:42 davy Exp $";
#endif
/*
 * main.c - main routine for index program.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	main.c,v $
 * Revision 1.1  89/08/09  11:06:42  davy
 * Initial revision
 * 
 */
#include <sys/param.h>
#include <curses.h>
#include <stdio.h>
#include "defs.h"

int	igncase = 0;			/* non-zero if -i flag given	*/
int	verbose = 0;			/* non-zero if -v flag given	*/

char	*pname;				/* program name			*/
char	dbasedir[MAXPATHLEN];		/* path to database directory	*/

main(argc, argv)
char **argv;
int argc;
{
	char *database, *filter, *pattern;

	pname = *argv;
	database = filter = pattern = NULL;

	/*
	 * Process arguments.
	 */
	while (--argc) {
		if (**++argv == '-') {
			switch (*++*argv) {
			case 'f':			/* filter	*/
				if (--argc <= 0)
					usage();

				filter = *++argv;
				continue;
			case 'i':			/* ignore case	*/
				igncase++;
				continue;
			case 'v':			/* verbose	*/
				verbose++;
				continue;
			}
		}

		/*
		 * database argument is first.
		 */
		if (database == NULL) {
			database = *argv;
			continue;
		}

		/*
		 * pattern argument is next.
		 */
		if (pattern == NULL) {
			pattern = *argv;
			continue;
		}

		usage();
	}

	/*
	 * Get the path of the database directory.
	 */
	set_dbase_dir();

	/*
	 * If they didn't specify a database, put them in
	 * the selection routine.
	 */
	if (database == NULL)
		database = select_db();

	/*
	 * Open the database and read it in.
	 */
	read_idxfile(database);
	read_dbfile(database);

	/*
	 * If they didn't specify a pattern, go to the
	 * main menu.  Otherwise, search the database
	 * for the pattern, and print the results.
	 */
	if (pattern == NULL) {
		main_menu(database);
		reset_modes();
	}
	else {
		search_db(pattern);
		print_db(database, filter);
	}

	exit(0);
}
