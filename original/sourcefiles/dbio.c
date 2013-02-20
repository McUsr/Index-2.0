#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/dbio.c,v 1.1 89/08/09 11:06:36 davy Exp $";
#endif
/*
 * dbio.c - database input/output routines.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	dbio.c,v $
 * Revision 1.1  89/08/09  11:06:36  davy
 * Initial revision
 * 
 */
#include <sys/param.h>
#include <sys/stat.h>
#include <curses.h>
#include <stdio.h>
#include "defs.h"

struct	dbfile *db;		/* array of database entries		*/
struct	idxfile idx;		/* description of the database file	*/

int	dbmodified = 0;		/* non-zero if database needs saving	*/
int	dbentries, dbsize;	/* number of entries, size of db array	*/

/*
 * read_idxfile - read the database description file.
 */
read_idxfile(dbname)
char *dbname;
{
	FILE *fp;
	register int len;
	char buf[BUFSIZ], idxfile[MAXPATHLEN];

	/*
	 * Construct the file name.
	 */
	sprintf(idxfile, "%s/%s%s", dbasedir, dbname, IDXFILE_SUFFIX);

	/*
	 * Open the file.
	 */
	if ((fp = fopen(idxfile, "r")) == NULL) {
		error("%s: cannot open \"%s\".\n", pname, idxfile, 0);
		exit(1);
	}

	/*
	 * Zero out the structure.
	 */
	bzero(&idx, sizeof(struct idxfile));

	/*
	 * Read lines from the file.
	 */
	while (idx.idx_nlines < MAXDBLINES) {
		/*
		 * End of file.
		 */
		if (fgets(buf, sizeof(buf), fp) == NULL)
			break;

		/*
		 * Strip the newline.
		 */
		len = strlen(buf) - 1;
		buf[len] = '\0';

		/*
		 * If the first character is '!', then this line
		 * should not participate in searches.  Save the
		 * stuff after the '!'.  Otherwise this line does
		 * participate in searches, save the whole line.
		 */
		if (*buf == '!') {
			idx.idx_lines[idx.idx_nlines] = savestr(buf+1);
			idx.idx_search[idx.idx_nlines] = 0;
			len--;
		}
		else {
			idx.idx_lines[idx.idx_nlines] = savestr(buf);
			idx.idx_search[idx.idx_nlines] = 1;
		}

		/*
		 * Increment the number of lines.
		 */
		idx.idx_nlines++;

		/*
		 * Save the length of the longest field name.
		 */
		if (len > idx.idx_maxlen)
			idx.idx_maxlen = len;
	}

	/*
	 * Close the file.
	 */
	fclose(fp);
}

/*
 * read_dbfile - read the database file itself.
 */
read_dbfile(dbname)
char *dbname;
{
	FILE *fp;
	register int i;
	struct dbfile *malloc(), *realloc();
	char buf[BUFSIZ], dbfile[MAXPATHLEN];

	/*
	 * Allocate some entries in the array.  16 is just an
	 * arbitrary number.
	 */
	dbsize = 16;
	dbentries = 0;

	if ((db = malloc(dbsize * sizeof(struct dbfile))) == NULL) {
		error("%s: out of memory.\n", pname, 0, 0);
		exit(1);
	}

	/*
	 * Construct the name of the file.
	 */
	sprintf(dbfile, "%s/%s%s", dbasedir, dbname, DBFILE_SUFFIX);

	/*
	 * Open the file.
	 */
	if ((fp = fopen(dbfile, "r")) == NULL)
		return;

	/*
	 * Until we hit end of file...
	 */
	while (!feof(fp)) {
		/*
		 * If we need to, allocate some more entries.
		 */
		if (dbentries >= dbsize) {
			dbsize *= 2;
			db = realloc(db, dbsize * sizeof(struct dbfile));

			if (db == NULL) {
				error("%s: out of memory.\n", pname, 0, 0);
				exit(1);
			}
		}

		/*
		 * Read in one entry at a time.
		 */
		for (i = 0; i < idx.idx_nlines; i++) {
			/*
			 * If we hit end of file before getting a
			 * complete entry, toss this one.
			 */
			if (fgets(buf, sizeof(buf), fp) == NULL)
				goto out;

			/*
			 * Save the length of the line, strip the
			 * newline, and save the line.
			 */
			db[dbentries].db_lens[i] = strlen(buf) - 1;
			buf[db[dbentries].db_lens[i]] = '\0';

			db[dbentries].db_lines[i] = savestr(buf);
		}

		/*
		 * Mark this entry as valid and increase the
		 * number of entries.
		 */
		db[dbentries].db_flag = DB_VALID;
		dbentries++;
	}

out:
	/*
	 * Make sure what we've got is sorted.
	 */
	qsort(db, dbentries, sizeof(struct dbfile), dbsort);

	fclose(fp);
}

/*
 * save_db - save the database to disk.
 */
save_db(dbname)
char *dbname;
{
	FILE *fp;
	struct stat st;
	register int i, j;
	char realfile[MAXPATHLEN], bakfile[MAXPATHLEN];

	/*
	 * If it doesn't need saving, never mind.
	 */
	if (!dbmodified)
		return;

	/*
	 * Create the name of the file and a backup file.
	 */
	sprintf(realfile, "%s/%s%s", dbasedir, dbname, DBFILE_SUFFIX);
	sprintf(bakfile, "%s/#%s%s", dbasedir, dbname, DBFILE_SUFFIX);

	/*
	 * Default creation mode.
	 */
	st.st_mode = 0400;

	/*
	 * If the file already exists, rename it to the
	 * backup file name.
	 */
	if (stat(realfile, &st) == 0)
		rename(realfile, bakfile);

	/*
	 * Open the new file.
	 */
	if ((fp = fopen(realfile, "w")) == NULL) {
		error("%s: cannot create \"%s\".\n", pname, realfile);
		exit(1);
	}

	/*
	 * Make sure the database is sorted.
	 */
	qsort(db, dbentries, sizeof(struct dbfile), dbsort);

	/*
	 * Write out the entries.
	 */
	for (i=0; i < dbentries; i++) {
		if ((db[i].db_flag & DB_VALID) == 0)
			continue;

		for (j=0; j < idx.idx_nlines; j++)
			fprintf(fp, "%s\n", db[i].db_lines[j]);
	}

	/*
	 * Set the file mode to the mode of the original
	 * file.  Mark the database as unmodified.
	 */
	fchmod(fileno(fp), st.st_mode & 0777);
	dbmodified = 0;

	fclose(fp);
}
