/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4
 *
 * dbfunc.c - database functions selected from the main menu.
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
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "common.h"
#include "unicodeToUtf8.h"
#include "defs.h"
#include "initcurses.h"
#include "util.h"
#include "dbio.h"
#include "screen.h"
#include "dbfunc.h"
#include "dbfilename.h"
#ifdef TESTING
#include "locale.h"
#include "dbselect.h"
#include "regsort.h"
#endif

extern UCollator *compareCollator;

#ifdef TESTING
static void findUtf8OrDie(void);

static void utf8Errmsg(char *prgName, char *curLocale);

int main(int argc, char **argv)
{
	wchar_t *database = mbstowcs_alloc("test_file");

	/* REMEMBER: set the INDEXDIR environment variable. */
	setProgramName(*argv);
	findUtf8OrDie();
	set_dbase_dir();
	/* Testing with book.idx and db, renamed, then edited sending output to
	   stdout, so I can compare! */
	set_modes();

	read_idxfile(database);
	read_database(database);
	/*
	   fprintf(stderr,"Adding Entry\n");
	   add_entry() ;
	   print_db(database,NULL) ;
	   save_db(database) ;
	 */
	disp_entries();
	reset_modes();
	return 0;
}

static void findUtf8OrDie(void)
{
	char *old_locale, *localeRes;

	old_locale = setlocale(LC_ALL, "");
	localeRes = strstr(old_locale, "UTF");
	if (localeRes == NULL) {
		utf8Errmsg(getProgramName(), old_locale);
		exit(1);
	} else {
		char *collation = setlocale(LC_COLLATE, "");

		setCollation(collation);
	}
}

/* TODO: just use basename for programname. */
static void utf8Errmsg(char *prgName, char *curLocale)
{
	fprintf(stderr,
		"%s: An error has occured, I can't find that an encoding of UTF-8 have been set (%s).\nThe locale must be specified so that your language can be translated properly by\n%s.\nYou can read about LC_ALL by entering  \"man bash\", or \"man sh\" in your terminal window.\nYou have to specify LC_ALL for your language, setting  LC_CTYPE isn't enough. You can\nget a list of all locales installed on your system (usually in /usr/lib/locale/) with\nthe command locale -a.Be sure to choose one that ends with UTF-8!\n",
		prgName, curLocale, prgName);
}

#endif

/* Menu used by disp_entries routine.                           */
static const char *disp_menu[] = {
      "<CR>  = next entry              \"d\" = delete this entry\n",
      "\"+\"   = next entry              \"e\" = edit this entry\n",
      "\"-\"   = previous entry          \"q\" = return to main menu",
	0
};

/* display all database entries with DB_PRINT set, and  let the */
/* user choose what to do with the entry.                       */
void disp_entries(void)
{
    const char procname[]="disp_entries" ;
	int first, save = -1;
	register int i, j;
	register wchar_t ch;

	dbbuffer tmp;
	/* Print name of the database and number of entries.    */
    initHeading() ;

	/* Clear the screen.                                */
 top:	clear(); 
    initEntryLine() ;
    paintHeading("BROWSE Entries") ;
	/* Print the names of the fields.                       */
	for (i = STARTROW; i < (idx.idx_nlines+STARTROW); i++) {
		mvaddwstr(i, 0, idx.idx_lines[i-STARTROW]);
	}

	/* Print the menu.                                      */
	for (i = 0; disp_menu[i] != NULL; i++)
		mvaddstr(idx.idx_nlines + STARTROW + i + 2, 10, disp_menu[i]);

	/* Find first printable entry, save its index in first. */
	for (first = 0; first < dbentries; first++) {
		if (db[first].db_flag & DB_PRINT)
			break;
	}

	/* Set current entry to either first one or  saved one. */
	if (save < 0)
		i = first;
	else
		i = save;
	/* Until we run out of entries...                       */
	while (i < dbentries) {
		/* Print entry. The record                          */
		for (j = 0; j < idx.idx_nlines; j++) {
			move(j+STARTROW, idx.idx_maxlen + 2);
			clrtoeol();

			/* FIXi moves the stuff over to db here. */

			if (db[i].db_lens[j] > 0) {
				tmp.db_lines[j] =
                     wcsFromUnicode_alloc((size_t *) &tmp.db_lens[j], db[i].db_lines [j],
                         db[i].db_lens [j]);
				if (tmp.db_lines[j] == NULL) {
                    /* this may be something different from the layer below */
                    yerror(YMALLOC_ERR,procname,"tmp.db_lines[j]",YX_EXTERNAL_CAUSE ) ;
				}
			} else {
				tmp.db_lens[j] = 0;
				tmp.db_lines[j] = NULL;
			}
			/* Save the length of entry, zero the memory.   */
			/* addwstr(db[i].db_lines[j]); */
			addwstr(tmp.db_lines[j]);
		}

		/* Print current/total entry numbers.               */
		mvprintw(idx.idx_nlines + 7 + STARTROW, COLS - 15, "%5d/%-5d", i + 1,
			 dbentries);
		/* See what they want to do with this entry.        */
		ch = prompt_char(idx.idx_nlines + STARTROW + 6, 0, "Command: ",
				 "-+deq\n\014\r");

		/* Dispatch command...                               */
		switch (ch) {
        case '+':
		case '\r':
		case '\n':	/* go to next entry     */
			/* Save this one.                               */
			save = i;

			/* Advance to next printable one.               */
			for (i++; i < dbentries; i++) {
				if (db[i].db_flag & DB_PRINT)
					break;
			}

			break;
		case '-':	/* go to previous entry */
			/* Save this one.                               */
			save = i;

			/* Hunt for the previous printable one.         */
			for (i--; i >= 0; i--) {
				if (db[i].db_flag & DB_PRINT)
					break;
			}

			if (i < 0)
				i = first;

			break;
		case 'd':	/* delete entry         */

			/* If really want to delete it:                 */
			/* If so, mark it as not valid, and  re-sort db */
			/* Because of save variable, next time through  */
			/* we'll print the last one they looked at.     */

			if (del_entry()) {
				db[i].db_flag &= ~(DB_VALID | DB_PRINT);

				UErrorCode status = U_ZERO_ERROR;

				compareCollator = ucol_open(getCollation(),
							    &status);
				if (U_SUCCESS(status)) {
					qsort(db, (size_t) dbentries,
					      sizeof(dbrecord),
					      (compFunc) dbsort);
				} else {
                    y_icuerror( YICU_CRECOLL_ERR, procname, "compareCollator", status ) ;
				}
				ucol_close(compareCollator);

				dbmodified = 1;
				dbentries--;

				/* Frees fields with display text  here.    */
				for (j = 0; j < idx.idx_nlines; j++) {
					if (tmp.db_lines[j] != NULL) {
						tmp.db_lens[j] = 0;
						free(tmp.db_lines[j]);
					}
				}
				goto top;
			}

			save = i;
			/* Frees fields with display text  here.        */
			for (j = 0; j < idx.idx_nlines; j++) {
				if (tmp.db_lines[j] != NULL) {
					tmp.db_lens[j] = 0;
					free(tmp.db_lines[j]);
				}
			}
			goto top;
		case 'e':	/* edit entry           */
			/* Let them edit the entry.                     */
			if (edit_entry(&db[i], " EDIT modified","this"))
				dbmodified = 1;

			save = i;
			/* Frees fields with display text  here.        */
			for (j = 0; j < idx.idx_nlines; j++) {
				if (tmp.db_lines[j] != NULL) {
					tmp.db_lens[j] = 0;
					free(tmp.db_lines[j]);
				}
			}
			goto top;
		case 'q':	/* return to main menu. */
		    /* Frees fields with display text  here.            */
		    for (j = 0; j < idx.idx_nlines; j++) {
			    if (tmp.db_lines[j] != NULL) {
				    tmp.db_lens[j] = 0;
				    free(tmp.db_lines[j]);
			    }
		    }
			return;
            
		case '\014':	/* redraw screen        */
			break;
		}
		/* Frees fields with display text  here.            */
		for (j = 0; j < idx.idx_nlines; j++) {
			if (tmp.db_lines[j] != NULL) {
				tmp.db_lens[j] = 0;
				free(tmp.db_lines[j]);
			}
		}
	}
	/* Frees fields with display text  here.            */
	for (j = 0; j < idx.idx_nlines; j++) {
		if (tmp.db_lines[j] != NULL) {
			tmp.db_lens[j] = 0;
			free(tmp.db_lines[j]);
		}
	}
}

/* TODO: sette et merke alle steder det skal være strenger
   med meldinger og feilmeldinger samt mottak av data.
   opp en litt mere formell plan for gjennomføring.            */

/* add an entry to database.                                */
void add_entry(void)
{
    static const char procname[]="add_entry" ;
    static const char collatorname[]="compareCollator" ;
    static const char operationDesc[]=" EDIT New Entry";
    static const char recordDesc[]="new" ;
	register int i;
	/* Search for an empty entry in array.                  */
	for (i = 0; i < dbsize; i++) {
		/* Found one; use it.                               */
		if ((db[i].db_flag & DB_VALID) == 0) {
			/* Clear out any old junk.                       */
            memset(&db[i],(int) 0,sizeof(dbrecord)) ;
			/* Let user edit the entry.                     */
			if (edit_entry(&db[i],operationDesc,recordDesc)) {
				/* Mark it valid, mark db as modified,      */
				db[i].db_flag |= DB_VALID;
				dbmodified = 1;
				dbentries++;
				/* Increase number of entries.              */
				/* Sort array, to get entry in proper place. */
                if ( dbentries > 1 ) {
				    UErrorCode status = U_ZERO_ERROR;

				    compareCollator =
				        ucol_open(getCollation(), &status);
				    if (U_SUCCESS(status)) {
					    qsort(db, (size_t) dbentries,
					      sizeof(dbrecord),
					      (compFunc) dbsort);
				    } else {
                        y_icuerror( YICU_CRECOLL_ERR, procname, collatorname, status ) ;
				    }
				    ucol_close(compareCollator);
                }
			}

			return;
		}
	}
	/* Didn't find empty slot, have to allocate more.        */
	dbsize *= 2;

	if ((db = realloc(db, dbsize * sizeof(dbrecord))) == NULL) {
        yerror(YREALLOC_ERR,procname,"db",YX_EXTERNAL_CAUSE ) ;
	}
    memset(&db[dbentries],(int) 0, sizeof(dbrecord)); 
	/* Let user edit new entry.                              */
	if (edit_entry(&db[dbentries],operationDesc,recordDesc)) {
		/* Mark entry as valid, mark db as modified,        */
		/* increase number of entries.                      */
		db[dbentries].db_flag |= DB_VALID;
		dbmodified = 1;
		dbentries++;

		UErrorCode status = U_ZERO_ERROR;

		compareCollator = ucol_open(getCollation(), &status);
		if (U_SUCCESS(status)) {
			qsort(db, (size_t) dbentries, sizeof(dbrecord),
			      (compFunc) dbsort);
		} else {
            y_icuerror( YICU_CRECOLL_ERR, procname,collatorname, status ) ;
		}
		ucol_close(compareCollator);
	}
}

/* delete an entry from db.                                 */
int
del_entry(void)
{
    static const char deleteConfirmation[]="Really delete this entry? (y/n)";
	char c;

	int x, y;


	/* Prompt user for confirmation.                        */
	getyx(curscr, y, x);
	c = prompt_char(y, 0,deleteConfirmation, "YyNn");

	/* Return status of confirmation.                       */
	switch (c) {
	case 'Y':
	case 'y':
		return (1);
	case 'N':
	case 'n':
		return (0);
	default:
		return (0);	/* removes compiler warning             */
	}
}

/* run through database entry by entry.                     */
void read_db(void)
{
	register int i;

	/* Sort db, so  it's in order.                           */
	UErrorCode status = U_ZERO_ERROR;

	compareCollator = ucol_open(getCollation(), &status);
	if (U_SUCCESS(status)) {
		qsort(db, (size_t) dbentries, sizeof(dbrecord),
		      (compFunc) dbsort);
	} else {
        y_icuerror( YICU_CRECOLL_ERR, "read_db", "compareCollator", status ) ;
	}
	ucol_close(compareCollator);

	/* Set DB_PRINT in all entries.                          */
	for (i = 0; i < dbentries; i++) {
		if (db[i].db_flag & DB_VALID)
			db[i].db_flag |= DB_PRINT;
	}

	/* Display all entries.                                 */
	disp_entries();

	/* Clear DB_PRINT.                                      */
	for (i = 0; i < dbentries; i++)
		db[i].db_flag &= ~DB_PRINT;
}

/*save database and exit.                                   */
int save_bye()
{
	int ret = 0;

	save_db();
	ret = byebye();
	return ret;
}

