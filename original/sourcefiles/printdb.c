#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/printdb.c,v 1.1 89/08/09 11:06:47 davy Exp $";
#endif
/*
 * printdb.c - print entries from the database.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	printdb.c,v $
 * Revision 1.1  89/08/09  11:06:47  davy
 * Initial revision
 * 
 */
#include <sys/file.h>
#include <stdio.h>
#include "defs.h"

/*
 * print_db - print out entries marked DB_PRINT in the database.
 */
print_db(dbname, filter)
char *dbname, *filter;
{
	FILE *pp;
	FILE *popen();
	char buf[BUFSIZ];
	register int i, j;
	register char *tab;

	/*
	 * If no filter was specified, we just spit the entries out,
	 * with their field names, to standard output.
	 */
	if (filter == NULL) {
		for (i=0; i < dbentries; i++) {
			if ((db[i].db_flag & DB_VALID) == 0)
				continue;
			if ((db[i].db_flag & DB_PRINT) == 0)
				continue;

			for (j=0; j < idx.idx_nlines; j++) {
				if (!verbose) {
					if (db[i].db_lines[j][0] == '\0')
						continue;
				}

				sprintf(buf, "%s%s", idx.idx_lines[j],
					idx.idx_lines[j][0] ? ":" : "");
				printf("%-*s%s\n", idx.idx_maxlen + 2,
				       buf, db[i].db_lines[j]);
			}

			putchar('\n');
		}

		return;
	}

	/*
	 * Otherwise, we set up a pipe to the filter, and print
	 * first the field names, and then the fields.  We do
	 * this one entry per line, with fields separated by
	 * tabs.
	 */

	/*
	 * Create the path to a formatting program in the database
	 * directory.
	 */
	sprintf(buf, "%s/%s%s", dbasedir, filter, FMTFILE_SUFFIX);

	/*
	 * If that's not there, then assume they gave us some
	 * program name (like "more" or something), and just
	 * stick it in there.
	 */
	if (access(buf, X_OK) < 0)
		strcpy(buf, filter);

	/*
	 * Open the pipe.
	 */
	if ((pp = popen(buf, "w")) == NULL) {
		error("%s: cannot execute \"%s\".\n", pname, filter, 0);
		exit(1);
	}

	/*
	 * Print the field names, separated by tabs.
	 */
	tab = "";
	for (i=0; i < idx.idx_nlines; i++) {
		fprintf(pp, "%s%s", tab, idx.idx_lines[i]);
		tab = "\t";
	}

	putc('\n', pp);

	/*
	 * Print the entries, with fields separated
	 * by tabs.
	 */
	for (i=0; i < dbentries; i++) {
		if ((db[i].db_flag & DB_VALID) == 0)
			continue;
		if ((db[i].db_flag & DB_PRINT) == 0)
			continue;

		tab = "";
		for (j=0; j < idx.idx_nlines; j++) {
			fprintf(pp, "%s%s", tab, db[i].db_lines[j]);
			tab = "\t";
		}

		putc('\n', pp);
	}

	/*
	 * Close the pipe.
	 */
	pclose(pp);
}
