/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4
 * dbio.c - database input/output routines.
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
 */
#include <common.h>
#include <initcurses.h>
#include <unicodeToUtf8.h>
#include <dbsearch.h>
#include <dbcreate.h>
#include <defs.h>
#include <util.h>
#include <dbio.h>
#include <dbfilename.h>
#ifdef TESTING
#include <locale.h>
#include <dbselect.h>
#include "regsort.h"
#endif

dbrecord *db;			/* array of database entries                */

idxfile idx;			/* description of the database file         */

UCollator *compareCollator = NULL;

int dbmodified = 0;		/* non-zero if database needs saving    */

int dbentries, dbsize;		/* number of entries, size of db array  */

static int onlyspaces( char * buf ) ;
static int reverse_data=0 ; /* governs reverse order of db */

/* TODO: sjekk om disse kan deklareres static.
 * tror det blir minst convertering hvis jeg beholder dbasename
 * som utf8
 * Mens jeg holder på nå, så bytter jeg navn til label file.
 *
 * Jeg skiller ut usage funksjoner og slikt i service modul?
 */

#ifdef TESTING
static void findUtf8OrDie(void);

static void utf8Errmsg(char *prgName, char *curLocale);
/* heavily outdated */
int main(int argc, char **argv)
{
	wchar_t *database = mbstowcs_alloc("test_file");

	if (database == NULL)
		memerrmsg("main in dbio.c");

	/* REMEMBER: set the INDEXDIR environment variable. */
	setProgramName(*argv);
	findUtf8OrDie();
	set_dbase_dir();
	/* Testing with book.idx and db, renamed, then edited sending output to
	   stdout, so I can compare! */
	fprintf(stderr, "Reading indexfile\n");
	read_idxfile(database);
	fprintf(stderr, "Reading databasefile\n");
	read_database(database);
	print_db(database, NULL);
	dbmodified = 1;
	save_db();
	release_idx();
	release_db();
	return 0;
}

static void
findUtf8OrDie(void)
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

static void utf8Errmsg(char *prgName, char *curLocale)
{
	fprintf(stderr,
		"%s: An error has occured, I can't find that an encoding of UTF-8 have been set (%s).\nThe locale must be specified so that your language can be translated properly by\n%s.\nYou can read about LC_ALL by entering  \"man bash\", or \"man sh\" in your terminal window.\nYou have to specify LC_ALL for your language, setting  LC_CTYPE isn't enough. You can\nget a list of all locales installed on your system (usually in /usr/lib/locale/) with\nthe command locale -a.Be sure to choose one that ends with UTF-8!\n",
		prgName, curLocale, prgName);
}

#endif

void
set_reverse(void) {
    reverse_data=1 ; /* governs reverse order of db */
}
/* read the database description file.
 * those file are kept internally in wide character format.*/
/*
 * Trenger begrenset lengde på feltnavn: 100 tegn * 6 for multibyte.
 * Dette skal la oss ende på 100 widechar tegn.
 *
 * Så vidt jeg har skjønt er sammenhengene slik:
 *
 * http://www.unicode.org/faq/utf_bom.html
 * utf8:  1-4 bytes
 * utf16: 2-4 bytes ( to 16 bit verdier)
 * utf32  4 bytes ( 1 32 bit verdi) widechar. ncurses jobber bra med widechars på 32 bit.
 *
 * Dette betyr at: jeg trenger 400 bytes satt av for å lagre hva som helst på 100 tegn.

 * New business rules, in the advent of ability of creation of db files from the commandline.
   This is the entry point for that functionality.

   if the databasefile, is merely specified, AND the file doesn't yet exist, AND we don't find
   any label file. (open_label_file() returns NULL ), THEN we create a new label file for the
   non-existant db file. Of course, should we be so happy to find one along the path then
   we'll use this one?

 * */
void
read_labelfile(void)
{
    const char procname[]="read_labelfile" ;
	FILE *fp;

	register int len;

	char buf[BUFSIZ];	/* 1024 characters per line on Mac Os X */
    /* we have gotten the name when we come here */
    fp = open_label_file() ;
    if ((fp == NULL ) && (!hasPattern()) && (shouldCreateMissingLabelFile()|| dbFileHasNoPath())) {
       newLabelFilePath() ; 
       create_db() ;
        char *labelfn =  getFullLabelFileName() ; 
        if ( file_is_empty(labelfn) || (!labelFileOkAfterLinting(labelfn))) {
            fprintf(stderr,"Index: User redecided creating a new database and thereby quitted.\n") ;
            finish(0) ;
            exit(0) ;
        } 
        /* we remove any trailing line-endings here */
        setLabelFileCreated() ;
        fp = fopen(labelfn,"r") ;
 /*       if (dbFileHasNoPath() ) {

        } */
        
    } else if (fp == NULL ) {
           char *labelfn =  getFullLabelFileName() ; 
           yerror(YFILE_FINDF_ERR,procname,labelfn,YX_EXTERNAL_CAUSE ) ; 
    }
    

	/* Zero out structure.                                      */
    memset(&idx,(int)0, sizeof(idxfile) ) ;
	/* Read lines from file.                                    */
	while (idx.idx_nlines < MAXDBLINES) {
		/* End of file.                                         */
		if (fgets(buf, (int)BUFSIZ , fp) == NULL)
			break;

		/* Strip newline. replace with a colon                  */
		len = strlen(buf) - 1;
		if (!len) {
			buf[len] = '\0';
		} else {	/* check for a colon! */
			if (buf[(len - 1)] == ':') {
				buf[len] = '\0';
			} else {	/* no colon, so we insert one! */
				buf[len] = ':';
			}
		}
		/* If first char is '!', then this line should not      */
		/* participate in searches.  Save stuff after the '!'.  */
		/* Otherwise this line does participate in searches,    */
		/* save  the whole line.                                */
		if (*buf == '!') {
			idx.idx_lines[idx.idx_nlines] = mbstowcs_alloc(&buf[1]);
			idx.idx_search[idx.idx_nlines] = 0;
			len--;
		} else {
			idx.idx_lines[idx.idx_nlines] = mbstowcs_alloc(buf);
			idx.idx_search[idx.idx_nlines] = 1;
		}
		/* Increment number of lines.                           */
		idx.idx_nlines++;

		/* Save the length of the longest field name.           */
		if (len > idx.idx_maxlen)
			idx.idx_maxlen = len;
	}

	/* Close file.                                              */
	fclose(fp);
}
/*
returns 1 if there is only spaces in a line
*/
static int
onlyspaces( char * buf )
{
    register int i,res=1 ;
    size_t bufend = strlen(buf) - 1 ;
    for (i = bufend;i>-1;i-- ) {
        if (!isspace(buf[i] )) {
            res=0 ;
            break ;
        }
    }
    return res ;
}
/*
Removes any empty lines to avoid "invisible labels", the users comfort,
Returns 1 if the file still has contents,after "linting".
We don't remove any white space if the line has other contents before or after
text.
It is called from: 
*/
int 
labelFileOkAfterLinting(char *labelfn) 
{
    const char procname[]="labelFileOkAfterLinting" ;
	char buf[BUFSIZ];	/* 1024 characters per line on Mac Os X */
    FILE *fin=NULL, *fout=NULL;
    char *lblBckFname = makeBackupName(labelfn ) ;
	int	ret = rename(labelfn, lblBckFname);
    if (ret) {
        yerror(YFILE_RENMV_ERR,procname,lblBckFname,YX_EXTERNAL_CAUSE ) ; 
    }
    /* open the backup file for reading */
	if ((fin = fopen(lblBckFname, "r")) == NULL )  {
        yerror(YFILE_OREAD_ERR,procname,lblBckFname,YX_EXTERNAL_CAUSE ) ; 
    }
    /* reopen the label file for writing */
	if ((fout = fopen(labelfn, "w")) == NULL )  {
        yerror(YFILE_CREAT_ERR,procname,labelfn,YX_EXTERNAL_CAUSE ) ; 
    }

    while (!feof(fin) ) {
        /* read it line by line */
		if (fgets(buf, (int)BUFSIZ , fin )== NULL )  {
            if (ferror(fin)) {
                    yerror(YFILE_FREAD_ERR,procname,lblBckFname,YX_EXTERNAL_CAUSE ) ; 
            }
            break ;
        }
        /* check if the line contains anything but whitespace */
        
        if (!onlyspaces(buf)) {
            if ( fputs(buf,fout) == EOF  ) { /* compatibility */
                    yerror(YFILE_FWRITE_ERR,procname,labelfn,YX_EXTERNAL_CAUSE ) ; 
            }
        }
    }
    fclose(fin) ;
    fclose(fout ) ;
	if ( file_is_empty(labelfn ) ) {
		return 0 ;
	} else {
    	return  1 ;
	}
}

/*
returns true if the file hasn't got any contents.
*/
int
file_is_empty( char *filename)
{
    struct stat fs;
    
    if (stat(filename,&fs) >= 0 ) {
        if ( (long) fs.st_size  == 0L ) {
            return 1 ;
         } else { 
             return 0 ;
        }
   } else {
       return -1 ; /* non -existent */
   }
}


/* For the case that we have given a filename on the commandline
   that doesn't exist, we'll create it, if we exit without saving
   anything in it, or abort the creation of an index file for it,
   then we bluntly remove the file we didn't care to use, during
   the exit.
*/
void
remove_unwanted_file( void )
{
    char * dbfileinUse = getFullDbName() ;
    if (file_is_empty(dbfileinUse) ) {
        if (justCreated() && (!labelFileIsJustCreated())) {
            unlink(dbfileinUse) ;
            /* TODO: this is too easy really has to probe the operation. */ 
        }
    }
    dbfileinUse = NULL ; 
}


void
release_idx(void)
{
	register int i;

	for (i = 0; i < idx.idx_nlines; i++)
		free(idx.idx_lines[i]);
}

int
compare_legacy(const void **string1, const void **string2)
{
	if (compareCollator != NULL) {
		UCollationResult res =
		    ucol_strcoll(compareCollator, (UChar *) * string1, -1,
				 (UChar *) * string2, -1);
		if (res == UCOL_LESS) {
			return -1;
		} else if (res == UCOL_GREATER) {
			return 1;
		} else {
			return 0;
		}
	} else {
		return 0;
	}
}

/* IMPORTANT: The collator must have been opened before     */
/* using this one.                                          */
int
reverse_dbsort(dbrecord * a, dbrecord * b)
{
	register int i, n;

	/* Sort invalid entries to the end.                     */
	if ((a->db_flag & DB_VALID) == 0) {
		if ((b->db_flag & DB_VALID) == 0)
			return (0);

		return (1);
	}

	if ((b->db_flag & DB_VALID) == 0)
		return (-1);

	/* Sort on first field, then try secondary fields.      */
	n = 0;
	if (compareCollator != NULL) {
		for (i = 0; (i < idx.idx_nlines) && (n == 0); i++) {
			UCollationResult res =
			    ucol_strcoll(compareCollator,
					 (UChar *) a->db_lines[i], -1,
					 (UChar *) b->db_lines[i], -1);

			if (res == UCOL_LESS) {
				n = 1;
			} else if (res == UCOL_GREATER) {
				n = -1;
			} else {
				n = 0;
			}
		}
	} else {
		char emi[BUFSIZ];

		sprintf(emi, "%s: dbsort: compareCollator == NULL! ",
			getProgramName());
		reset_modes();
		perror(emi);
		exit(1);
	}
	return (n);
}

/* IMPORTANT: The collator must have been opened before     */
/* using this one.                                          */
int
dbsort(dbrecord * a, dbrecord * b)
{
	register int i, n;

	/* Sort invalid entries to the end.                     */
	if ((a->db_flag & DB_VALID) == 0) {
		if ((b->db_flag & DB_VALID) == 0)
			return (0);

		return (1);
	}

	if ((b->db_flag & DB_VALID) == 0)
		return (-1);

	/* Sort on first field, then try secondary fields.      */
	n = 0;
	if (compareCollator != NULL) {
		for (i = 0; (i < idx.idx_nlines) && (n == 0); i++) {
			UCollationResult res =
			    ucol_strcoll(compareCollator,
					 (UChar *) a->db_lines[i], -1,
					 (UChar *) b->db_lines[i], -1);

			if (res == UCOL_LESS) {
				n = -1;
			} else if (res == UCOL_GREATER) {
				n = 1;
			} else {
				n = 0;
			}
		}
	} else {
		char emi[BUFSIZ];

		sprintf(emi, "%s: dbsort: compareCollator == NULL! ",
			getProgramName());
		reset_modes();
		perror(emi);
		exit(1);
	}
	return (n);
}

/* read database file itself.                                   */
/* The "idx" file must have been openend and read  up front.    */
/* This version creates records stored in uniocde internally.   */
void
read_database()
{
	FILE *fp;

	register int i;

	char mbsbuf[BUFSIZ] ;	/* 1024 characters per
							   line on Mac Os X */
	/* Det ser mere ut som om det er en db-entry som er på 1024 tegn!i den må
	   bli litt større 256 tegn per linje er passe stort nok! 64 tegn per felt
	   skulle også holde, også! */
    
    /* If a file didn't exist from the commandline, an index file has been
    created if we come here. there is more, if we then choose not to make
    a single entry before we quit, then the file will be deleted in the
    finish() handler. */
    
    fp = open_db() ;
	/* Allocate some entries in array.  16 is  arbitrary nr.    */
	dbsize = MAXDBLINES;
	dbentries = 0;

	if ((db = (dbrecord *)calloc((size_t) dbsize ,sizeof(dbrecord))) == NULL)
		memerrmsg("read_database couldn't calloc memory."); /* TODO */

	/* Until we hit end of file...                              */
	while (!feof(fp)) {
		/* If we need to, allocate more entries.                */
		if (dbentries >= dbsize) {
			dbsize *= 2;
			db = yrealloc(db, dbsize * sizeof(dbrecord),"read_database","db");

		}
		/* Read in one entry at a time.                         */
		for (i = 0; i < idx.idx_nlines; i++) {	/* GLOBAL */
			/* If we hit end of file before getting complete    */
			/* entry, then toss this one.                       */
			if (fgets(mbsbuf, (int)(BUFSIZ * sizeof(char)), fp) ==
			    NULL)
				goto out;
			register size_t mbuflen = (strlen(mbsbuf) - 1);

			/* Save length of line                              */
			mbsbuf[mbuflen] = '\0';	/* remove newline           */

			/* hva gjør vi med tomme linjer?? */
			if (mbuflen > 0) {
				int32_t reslen = 0;
                /* TODO: we also need to improve errors in uniocFromUTF8_alloc
                   that would be the simplest so we never check for null upon 
                   return from it.(can be too many causes for errors to analyze
                   hindsightly. */
				db[dbentries].db_lines[i] =
				    unicodeFromUTF8_alloc(&reslen, mbsbuf,
							  (size_t) mbuflen);
				if (db[dbentries].db_lines[i] == NULL)
					memerrmsg("read_database");

				db[dbentries].db_lens[i] = reslen;
			} else {
				db[dbentries].db_lines[i] = NULL;
				db[dbentries].db_lens[i] = 0;
			}

		}
		/* Mark entry as valid, increase number of entries.     */
		db[dbentries].db_flag = DB_VALID;
		dbentries++;
	}
 out:
	fclose(fp);
	UErrorCode status = U_ZERO_ERROR;

	compareCollator = ucol_open(getCollation(), &status);
	if (U_SUCCESS(status)) {
        if (!reverse_data) { 
		qsort(db, (size_t) dbentries, sizeof(dbrecord),
		      (compFunc) dbsort);
        } else {
		    qsort(db, (size_t) dbentries, sizeof(dbrecord),
		      (compFunc) reverse_dbsort);
            
        }
	} else {
		fprintf(stderr,
			"%s : read_database: couldn't create a collator.\n",
			getProgramName());
		exit(1);
	}
	ucol_close(compareCollator);

	return;
}
#pragma GCC diagnostic ignored "-Wconversion"
/* Don't know how to remove the warning for fchmod              */
/* save the database to disk.                                   */
void save_db(void)
{
	FILE *fp;

	struct stat st;

	register int i, j;

	char *realfile=NULL, *bakfile=NULL;
    /* vi oppretter bakfile første gangen vi kaller rutinen
    hvis bakfile ikke er opprettet? 
    */
	char buf[BUFSIZ];

	/* If it doesn't need saving, never mind.                   */
	if (!dbmodified)
		return;

	/* Create name of file and a backup file.                   */
    realfile = getFullDbName() ; 
    bakfile = getDbBackupFileName()  ;
	/* Default creation mode.                                   */
	/* TODO: into header with this one                         */
	st.st_mode = 0400;
	/* better still use ... UMOD? */

	/* If file already exists, rename it to backup file name.   */
	if (stat(realfile, &st) == 0) {
	    int	ret = rename(realfile, bakfile);
        if (ret) {
           yerror(YFILE_RENMV_ERR,"save_db",bakfile,YX_EXTERNAL_CAUSE ) ; 
        }
    }
	/* Open new file.                                           */
	if ((fp = fopen(realfile, "w")) == NULL)
        yerror(YFILE_CREAT_ERR,"save_db",realfile,YX_EXTERNAL_CAUSE ) ; 
	/* Make sure database is sorted.                             */
    if (dbentries > 1 ) {
	    UErrorCode status = U_ZERO_ERROR;

	    compareCollator = ucol_open(getCollation(), &status);
	    if (U_SUCCESS(status)) {
		    qsort(db, (size_t) dbentries, sizeof(dbrecord),
	    	      (compFunc) dbsort);
	    } else {
		    fprintf(stderr,
			    "%s : read_database: couldn't create a collator.\n",
			    getProgramName());
		    exit(1);
	    }
	    ucol_close(compareCollator);
    }
	/* START: */
	/* Write out entries.                                       */
	/* BUG: det som skjer er at feltet er tomt, så det får ikke noe nytt
	   innhold! */
	for (i = 0; i < dbentries; i++) {
		if ((db[i].db_flag & DB_VALID) == 0)
			continue;

		for (j = 0; j < idx.idx_nlines; j++) {
			if (db[i].db_lens[j] > 0) {
				/* if (usz > 0 ) { */
				(void)utf8FromUnicode(buf, (int32_t) BUFSIZ,
						      db[i].db_lines[j],
						      u_strlen(db[i].
							       db_lines[j]));
				fprintf(fp, "%s\n", buf);
			} else {
				fprintf(fp, "\n");
			}

		}
	}

	/* Set file mode to mode of orig file. Mark db as unmodified. */
	fchmod(fileno(fp), (mode_t) (st.st_mode & (mode_t) 0777));
	dbmodified = 0;

	fclose(fp);
}

#pragma GCC diagnostic warning "-Wconversion"

void release_db(void)
{
	register int i, j;

	for (i = 0; i < dbentries; i++)
		for (j = 0; j < idx.idx_nlines; j++)
			free(db[i].db_lines[j]);
}
