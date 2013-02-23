/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4:ro
 *
 * dbselect.c - database selection routines.
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
#include <dbio.h>
#include <screen.h>
#include <dbcreate.h>
#include <util.h>
#include <dbfilename.h>
#include <dbselect.h>


extern UCollator *compareCollator;

static int load_dblist(wchar_t * dblist[MAXDBFILES]);

#ifdef TESTING
int main(void)
{
	char *myLocale = setlocale(LC_ALL, "");

	wchar_t *selectedDB = NULL;

	setCollation(myLocale);
	printf("Hello World\n");
	/* Set tty modes.                                       */
	set_dbase_dir();
	selectedDB = select_db();
	reset_modes();
	return 0;
}
#endif
/*allow user to select from list of dbs's or create new     */
void select_db(void)
{
    wchar_t dbasename[MAXPATHLEN] = { L"" };
	wchar_t *dblist[MAXDBFILES];

	register int ndbs, i, row, col, spread;

	assert(__APPLE__ == 1);
	/* Apples HFS filename is incase sensitive, so we have to convert the users
	   input from widecharacters to lowercase we use icu to do so, so the
	   characters are converted accordingly to the locale                            */
	/* Load the list of databases the user has.             */
	ndbs = load_dblist(dblist);
	/* utf-8 */
	spread = (ndbs + 3) / 4;

	/* Set tty modes, clear screen.                         */
	set_modes();

	clear();
	/* Print the list of databases in four columns.         */
	mvaddstr(0, 0, "Index");
	for (row = 0; row < spread; row++) {
		for (col = 0; col < 4; col++) {
			i = col * spread + row;
            
			if (i < ndbs) {
		        UChar *lowerExistingBase = lowercasedUString( dblist[i] ) ;
                wchar_t *lowerWcs = wcsStringFromUnicode(lowerExistingBase ) ;
				mvaddwstr(row + 3, (col * COLS / 4) + 5,
					  lowerWcs);
                free(lowerExistingBase ) ;
                lowerExistingBase = NULL ;
                free(lowerWcs ) ;
                lowerWcs = NULL ;
			}
			/* utf-8 to widechar.                       */
		}
	}
    	*dbasename = '\0'; 
    /* TODO: clean up this, remove it, as I don't need it, but a set
    dbshort name in dbfname
    */
	/* Prompt for the name of a database.                   */
	while (*dbasename == '\0')
		prompt_str(spread + 2 + 3, 0, "Select a database: ", dbasename);

	size_t lastcharpos = wcslen(dbasename) - 1;
	while (dbasename[lastcharpos--] == (wchar_t) ' ') {
		dbasename[lastcharpos] = (wchar_t) '\0';
	}
    /* has to figure out where it is stored, if it is. */
    UChar *lowerExistingBase = lowercasedUString( dbasename ) ;
    wchar_t *lowerWcs = wcsStringFromUnicode(lowerExistingBase ) ;
    free(lowerExistingBase) ;
    lowerExistingBase = NULL ;
	/* TODO: it can't have a suffix if we arrive here. */
    if (labelFileExists(dbasename)) {
        set_dbase_shortname(lowerWcs) ;
        free(lowerWcs) ;
        lowerWcs = NULL ;
    }   else { 
        free(lowerWcs) ;
        lowerWcs = NULL ;
	    /* should be named create_dbrecord                        */
        set_dbase_shortname(dbasename) ;
        /* we didn't find any name, so we'll create a new db */
         newLabelFilePath() ;  /* has to set the full path as well */
	    create_db();
        /* This is the place we now *could* create the db file, provided
        we *did* create the index in the first place. See: creating new
        files on the man page. */
        char *labelfn =  getFullLabelFileName() ; 
        if ( file_is_empty(labelfn) || (!labelFileOkAfterLinting(labelfn))) {
            ysimpleError("Index: User defaulted creating a new database. We exit.!",YX_ALL_WELL) ;
        } 
        setLabelFileCreated() ;    
    }
	/* frees the memory */
	for (i = 0; i < ndbs; i++) {
		free(dblist[i]);
		dblist[i] = NULL;
	}
}

/* load up a list of user's databases                       */
/* Antar at det er ok å lese inn filnavn først som en
 * streng  av chars.
 * Men jeg må konverertere denne til utf16?
 *
 *Both files are UTF-16 with little-endian byte mark        */

static int load_dblist(wchar_t * dblist[MAXDBFILES])
{
    const char procname[]="load_dblist" ;
	DIR *dp;
	int ndbs;
	char *dbdir;
	char tmpFn[MAXPATHLEN];
	register char *s = NULL;
	register UChar *ucs = NULL;
	register struct direct *d;
	size_t slen = 0;
	int32_t ucsz = 0, reslen = 0;
	UChar *uchList[MAXDBFILES];

	/* TODO: needs some error messages, when just the index file is found and
	   no database file is found, maybe it is so. */

	ndbs = 0;
	dbdir = get_dbase_dir();
	/* Open db directory.                                   */
	if ((dp = opendir(dbdir)) == NULL) {
		/* This is happening  before we are calling curses  */
        yerror(YDIR_OPEN_ERR,procname,dbdir,YX_EXTERNAL_CAUSE ) ;
	}
	while ((d = readdir(dp)) != NULL) {
		/* Search for a "." in name, which marks suffix.    */
		if ((s = rindex(d->d_name, '.')) == NULL)
			continue;

		/* If this is an index def file, save its name.      */
		if (!strcmp(s, IDXFILE_SUFFIX)) {
			if (ndbs < MAXDBFILES) {
				/* *s = '\0' ; */
			    s = NULL;	/* changed from *s = '\0' ; */
				slen =
				    (strlen(d->d_name) -
				     strlen(IDXFILE_SUFFIX));
				strncpy(tmpFn, d->d_name, slen);
				tmpFn[slen] = '\0';

				ucsz = (int32_t) slen * 4;

				ucs =
				    (UChar *) ymalloc(((size_t) ucsz * sizeof(UChar)),procname,"ucs");

                reslen = unicodeFromUTF8(ucs, ucsz, tmpFn, slen);
                if (reslen >= 1) {
                    ucs = yrealloc(ucs, (reslen * sizeof(UChar)),procname,"ucs");
                    uchList[ndbs++] = ucs;
                } else {
                    /*TODO: kan vi polle ICU ERROR code for dette? , -> invalid bytesequence? */
                    ysimpleError("Index: load_dblist: cannot convert  a filename to unicode.",YX_EXTERNAL_CAUSE ) ;
                }
			}
		}
	}

	closedir(dp);
	UErrorCode status = U_ZERO_ERROR;

	compareCollator = ucol_open(getCollation(), &status);

	if (U_SUCCESS(status)) {
		qsort(uchList, (size_t) ndbs, sizeof(UChar *),
		      (compFunc) compare_legacy);
	} else {
        y_icuerror( YICU_CRECOLL_ERR, procname, "compareCollator", status ) ;
	}
	ucol_close(compareCollator);

	for (int i = 0; i < ndbs; i++) {

		int32_t ul = u_strlen(uchList[i]);

		size_t buflen = 0 ;
	    dblist[i] = wcsFromUnicode_alloc(&buflen, uchList[i], ul);
		if (buflen >= 1) {
			free(uchList[i]);
			uchList[i] = NULL;
		} else {
            ysimpleError("Index: load_dblist: cannot convert  a filename to wcs.",YX_EXTERNAL_CAUSE ) ;
		}
	}
	dblist[ndbs] = NULL;
	return (ndbs);
}
