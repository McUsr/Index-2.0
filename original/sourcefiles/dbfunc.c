#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/dbfunc.c,v 1.1 89/08/09 11:06:31 davy Exp $";
#endif
/*
 * dbfunc.c - database functions selected from the main menu.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	dbfunc.c,v $
 * Revision 1.1  89/08/09  11:06:31  davy
 * Initial revision
 * 
 */
#include <curses.h>
#include <stdio.h>
#include "defs.h"

/*
 * add_entry - add an entry to the database.
 */
add_entry()
{
	register int i;
	struct dbfile *realloc();

	/*
	 * Search for an empty entry in the array.
	 */
	for (i=0; i < dbsize; i++) {
		/*
		 * Found one; use it.
		 */
		if ((db[i].db_flag & DB_VALID) == 0) {
			/*
			 * Clear out any old junk.
			 */
			bzero(&db[i], sizeof(struct dbfile));

			/*
			 * Let the user edit the entry.
			 */
			if (edit_entry(&db[i], "new")) {
				/*
				 * Mark it valid, mark the database
				 * as modified, and increase the
				 * number of entries.
				 */
				db[i].db_flag |= DB_VALID;
				dbmodified = 1;
				dbentries++;

				/*
				 * Sort the array, to get this
				 * entry into its proper place.
				 */
				qsort(db, dbentries, sizeof(struct dbfile),
				      dbsort);
			}

			return;
		}
	}

	/*
	 * Didn't find an empty slot, so we have to allocate
	 * some more.
	 */
	dbsize *= 2;

	if ((db = realloc(db, dbsize * sizeof(struct dbfile))) == NULL) {
		error("%s: out of memory.\n", pname, 0, 0);
		exit(1);
	}

	bzero(&db[dbentries], sizeof(struct dbfile));

	/*
	 * Let the user edit the new entry.
	 */
	if (edit_entry(&db[dbentries], "new")) {
		/*
		 * Mark the entry as valid, mark the
		 * database as modified, and increase
		 * the number of entries.
		 */
		db[dbentries].db_flag |= DB_VALID;
		dbmodified = 1;
		dbentries++;

		qsort(db, dbentries, sizeof(struct dbfile), dbsort);
	}
}

/*
 * del_entry - delete an entry from the database.
 */
del_entry(entry)
struct dbfile *entry;
{
	char c;
	int x, y;

	/*
	 * Prompt the user for confirmation.
	 */
	getyx(curscr, y, x);
	c = prompt_char(y, 0, "Really delete this entry? ", "YyNn");

	/*
	 * Return the status of the confirmation.
	 */
	switch (c) {
	case 'Y':
	case 'y':
		return(1);
	case 'N':
	case 'n':
		return(0);
	}
}

/*
 * find_entry - search for entries using a regular expression.
 */
find_entry()
{
	register int i;
	char pattern[BUFSIZ];

	/*
	 * Clear the screen and prompt for the pattern to
	 * search for.
	 */
	clear();
	prompt_str(LINES/2, 0, "Pattern to search for: ", pattern);

	/*
	 * Search.  search_db will set DB_PRINT in the entries
	 * which match, and return non-zero if anything matched.
	 */
	if (search_db(pattern)) {
		/*
		 * Display the entries that matched.
		 */
		disp_entries();

		/*
		 * Clear the DB_PRINT flags.
		 */
		for (i=0; i < dbentries; i++)
			db[i].db_flag &= ~DB_PRINT;
	}
	else {
		/*
		 * Nothing matched.  Tell the user.
		 */
		prompt_char(LINES/2, 0,
			"No entries match pattern, type RETURN to continue: ",
			"\n");
	}
}

/*
 * read_db - run through the database entry by entry.
 */
read_db()
{
	register int i;

	/*
	 * Sort the database, so we're sure it's in order.
	 */
	qsort(db, dbentries, sizeof(struct dbfile), dbsort);

	/*
	 * Set DB_PRINT in all entries.
	 */
	for (i=0; i < dbentries; i++) {
		if (db[i].db_flag & DB_VALID)
			db[i].db_flag |= DB_PRINT;
	}

	/*
	 * Display the entries.
	 */
	disp_entries();

	/*
	 * Clear DB_PRINT.
	 */
	for (i=0; i < dbentries; i++)
		db[i].db_flag &= ~DB_PRINT;
}

/*
 * save_bye - save the database and exit.
 */
save_bye(dbname)
char *dbname;
{
	/*
	 * Save the database.
	 */
	save_db(dbname);

	/*
	 * Exit.
	 */
	byebye();
}
