/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4:ro
 *
 * main.c - main routine for index program.
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
#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <samples_dir.h>
#include "common.h"
#include "initcurses.h"
#include "defs.h"
#include "unicodeToUtf8.h"
#include "dbselect.h"
#include "dbfunc.h"		/* to make screen.h compile */
#include "screen.h"
#include "util.h"
#include "dbsearch.h"
#include "dbio.h"
#include "dbprint.h"
#include "dbfilename.h"


static void main_menu(void);

static void help(void);

static void usage(void);

static void utf8Errmsg( char *curLocale);

static void findUtf8OrDie(void);

static void print_copyright(void);

static void print_version(void) ;

int
main(int argc, char **argv)
{
    int wants_reverse = 0 ;
    int return_value = 0 ;

	UErrorCode status = U_ZERO_ERROR;

	int32_t ucslen = 0;

	setProgramName();

	findUtf8OrDie();
	/* sets and  checks locale, dies when locale isnt UTF-8 */
	u_init(&status);
	if (!U_SUCCESS(status)) {
        y_icuSimpleError("Index: Something erred during initialization of ICU",status);
	}

/*	databasearg = dbfilter = NULL; */
	int ch;
/* http://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html */
	static struct option longopts[] = {
		{"filter", required_argument,NULL, 'f'},
		{"ignore-case", no_argument, NULL, 'i'},
		{"verbose", no_argument, NULL, 'b'},
		{"no-labels", no_argument, NULL, 's'},
		{"header", no_argument, NULL, 'H'},
        {"inverted",no_argument,NULL, 'v'},
        {"reverse",no_argument,NULL, 'r' },
		{"nosql", no_argument, NULL, 'n'},
		{"field-sep", required_argument, NULL, 'F'},
		{"csv", required_argument, NULL, 'c'},
		{"quiet", no_argument, NULL, 'q'},
		{"help", no_argument, NULL, 'h'},
		{"usage", no_argument, NULL, 'u'},
		{"version", no_argument, NULL, 'V'},
		{"copyright", no_argument, NULL, 'C'},
		{NULL, 0, NULL, 0},
	};
    int option_index = 0 ;
	while ((ch = getopt_long(argc, argv, "f:ibsHvRnF:cqhuCV", longopts, &option_index)) != -1)
		switch (ch) {
		case 'f':
            /* setting the filter was taken care of directly by
                get_longopt */
            /* what if an int follows? we'll try open it as a file! */ 
            set_dbfilter( optarg ) ;
			break;
		case 'i':
            set_ignore_case() ;
			break;
        case 'b':
            set_verbose() ;
            break;
		case 's':
			set_suppress_labels();
			break;
		case 'H':
			set_header();
			break;
        case 'v':
            set_inverted() ; /*todo program */
            break ;
        case 'r':
            wants_reverse = 1 ;
            break ;
		case 'n':
			set_nosql();
			break;
		case 'F':
			set_fieldsep((char)optarg[0]);
			break;
        case 'c':
            set_csv() ;
            break;
		case 'q':
			set_quiet();
			break;
		case 'h':
            help() ;
            break;
		case 'u':
			usage();
			break;
        case 'V':
            print_version() ;
            break;
		case 'C':
			print_copyright();
			break;
		default:
			usage();
			break;
		}
	while (optind<argc) {

		/* database argument is first.                     */
		if (no_dbase_name()) {
			set_dbase_name(argv[optind++]);
			continue;
		}

		/* pattern argument is next.                        */
		if (!hasPattern()) {
			ucslen = patternFromUtf8(argv[optind++]);
			continue;
		}
		/* ends up here if we have superfluos arguments */
		usage();
	}
    
     if ( hasPattern() || has_dbfilter()) {
        if (wants_reverse ) {
            set_reverse() ;
        }
     }
     if ( hasPattern() && no_dbase_name() ) {
         ysimpleError("Index: I need a databasename when a pattern is specified.",YX_ALL_WELL ) ; 
         usage() ;
     }
     
     if ( has_dbfilter() && no_dbase_name() ) {
         ysimpleError("Index: I need a databasename when a filter is specified.",YX_ALL_WELL ) ; 
         usage() ;
     }

     if ( has_dbfilter() && (!hasPattern())) {
         ysimpleError("Index: I need a regexp even just \".*\" when a filter is specified.",YX_ALL_WELL ) ; 
         usage() ;

     }

	(void)signal(SIGINT, finish);
	(void)signal(SIGQUIT, SIG_DFL);
	/* arrange interrupts to terminate                      */
    collect_dbase_dirs() ;
	/* Get the path of the database directory.              */
	/* no database given, enters the selection routine.     */
    
	if ((no_dbase_name()) == 1) {
		select_db();
	}

	/* Open the database and read it in.                    */
	read_labelfile();
	read_database();

	/* No pattern, goes to the  main menu.                  */
	/* Otherwise, search the database  for the pattern,     */
	/* print the results.                                   */
	if (!hasPattern()) {
		main_menu();
	} else {
		search_db(utf8Pattern(), ucslen);
		return_value = print_db();
	}

	finish(return_value);
}

/* we dispatch to database functions from here.                 */
static void
main_menu(void)
{
	register unsigned int c;
    
    static wchar_t *addEntryLine=NULL ;
    static wchar_t *findEntryLine=NULL ;
    static wchar_t *readEntryLine=NULL ;
    static wchar_t *saveEntryLine=NULL ;
    static wchar_t *saveEntryQuitLine=NULL;
    static wchar_t *exitFromMainLine=NULL ;

    if (addEntryLine == NULL ) {
        addEntryLine =mbstowcs_alloc("a - Add new entry to database");
    }
    if (findEntryLine == NULL ) {
       findEntryLine = mbstowcs_alloc("f - Find entry in database");
    }
    if (readEntryLine == NULL ) {
       readEntryLine = mbstowcs_alloc("r - Read database entry by entry");
    }
    if ( saveEntryLine == NULL ) {
        saveEntryLine = mbstowcs_alloc ("s - Save modifications, do not exit");
    }
    if ( saveEntryQuitLine == NULL ) {
        saveEntryQuitLine = mbstowcs_alloc("q - Save modifications, exit");
    }
    if ( exitFromMainLine == NULL ) {
        exitFromMainLine =  mbstowcs_alloc("x - Exit");
    }
	/* Set tty modes.                                           */
	set_modes();
    initHeading() ;
	/* Print name of the database and number of entries.    */
	for (;;) {
		/* Clear the screen.                                    */
		clear();
        initEntryLine() ;
		/* Mention whether or not it's modified.                */
		if (dbmodified) {
            paintHeading(" MAIN MENU [modfied]") ;
        } else {
            paintHeading(" MAIN MENU") ;
        }
		/* Print the choices.                                   */
		mvaddwstr(STARTROW, 8,addEntryLine ) ;
		mvaddwstr(1+STARTROW, 8,findEntryLine ) ;
		mvaddwstr(2+STARTROW, 8,readEntryLine ) ;
		mvaddwstr(3+STARTROW, 8,saveEntryLine ) ;
		mvaddwstr(4+STARTROW, 8,saveEntryQuitLine ) ; 
		mvaddwstr(5+STARTROW, 8,exitFromMainLine ) ;

		/* Get one of the choices.                              */
		c = prompt_char(7+STARTROW, 0, "Command: ", "aAfFqQrRsSxX\014");

		/* Dispatch the choice to the proper database function.  */
		switch (c) {
		case 'a':
		case 'A':
			add_entry();
			break;
		case 'f':
		case 'F':
			find_entry();
			break;
		case 'q':
		case 'Q':
			if (!save_bye()) {
				break;
			} else {
				goto _end;
			}
		case 'r':
		case 'R':
			read_db();
			break;
		case 's':
		case 'S':
			save_db();
			break;
		case 'x':
		case 'X':
			if (!byebye()) {
				break;
			} else {
				goto _end;
			}
		case CTRL('l'):	/* Redraw screen                */
			wrefresh(curscr);
			break;
		}
	}
 _end:
    free(addEntryLine) ;
    addEntryLine = NULL ;
    free(readEntryLine ) ;
    readEntryLine = NULL ;
    free(saveEntryLine ) ;
    saveEntryLine = NULL ;
    free(saveEntryQuitLine ) ;
    saveEntryQuitLine = NULL ;
    free(exitFromMainLine ) ;
    exitFromMainLine = NULL ;
	return;
}

static void
findUtf8OrDie(void)
{
	char *old_locale, *localeRes;

	old_locale = setlocale(LC_ALL, "");
	localeRes = strstr(old_locale, "UTF");
	if (localeRes == NULL) {
		utf8Errmsg(old_locale);
		exit(1);
	} else {
		char *collation = setlocale(LC_COLLATE, "");

		setCollation(collation);
	}
}

static void
print_version(void)
{
    fprintf(stdout,"index version:  %s\n",PACKAGE_VERSION) ;
    fprintf(stdout,"Copyright by David A. Curry  and Tommy Bollman © 1989-2013\n") ;
    fprintf(stdout,"Index uses the ICU Library copyright Icu-project.org see index --copyright.\n") ;
    fprintf(stderr,"Sample files were installed at: %s\n",SAMPLES_DIR) ;
    exit(0) ;
}

static void
print_copyright(void)
{
    fprintf(stdout,"index version:  %s\n",PACKAGE_VERSION) ;
    fprintf(stdout,"Copyright by David A. Curry  and Tommy Bollman © 1989-2013 and put into Public Domain\n") ;
    fprintf(stdout,"This does not pertain the libraries used, where the copyright and usage terms are set by their proprietors.\n") ;
    fprintf(stdout,"Index uses the ICU Library copyright Icu-project.org see index --copyright.\n\n") ;
    fprintf(stdout,"Index uses also the  Ncurses library version 5.9, copyright GNU\n\n") ;
	fprintf(stderr,"ICU library version 49.1.2\n\nCopyright (c) 1995-2012 International Business Machines Corporation and others\n") ;
    fprintf(stderr,"        All rights reserved.\n") ;
    fprintf(stderr,"Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated\n");
    fprintf(stderr,"documentation files the \"Software\"), to deal in the Software without restriction, including without limitation\n") ;
    fprintf(stderr,"the rights to use, copy, modify, merge, publish, distribute, and/or sell copies of the Software, and to permit\n") ;
    fprintf(stderr,"persons to whom the Software is furnished to do so, provided that the above copyright notice(s) and this\n") ;
    fprintf(stderr," permission notice appear in all copies of the Software and that both the above copyright notice(s)\n");
    fprintf(stderr,"and this permission notice appear in supporting documentation.\n\n");
    fprintf(stderr,"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED\n");
    fprintf(stderr,"TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.\n");
    fprintf(stderr,"IN NO EVENT SHALL THE COPYRIGHT HOLDER OR HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY SPECIAL\n") ;
    fprintf(stderr,"INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER\n") ;
    fprintf(stderr,"IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR\n");
    fprintf(stderr,"PERFORMANCE OF THIS SOFTWARE.\n\n");
    fprintf(stderr,"       Except as contained in this notice, the name of a copyright holder shall not be used in advertising or\n") ;
    fprintf(stderr,"otherwise to promote the sale, use or other dealings in this Software without prior written authorization of\n");
    fprintf(stderr,"the copyright holder.\n");
    exit(0) ;
}

/* TODO: just use basename for programname. */
static void utf8Errmsg( char *curLocale)
{
	fprintf(stderr,
		"Index 2.0: An error has occured, I can't find that an encoding of UTF-8 have been set (%s).\nThe locale must be specified so that your language can be translated properly byindex.\nYou can read about LC_ALL by entering  \"man bash\", or \"man sh\" in your terminal window.\nYou have to specify LC_ALL for your language, setting  LC_CTYPE isn't enough. You can\nget a list of all locales installed on your system (usually in /usr/lib/locale/) with\nthe command locale -a.Be sure to choose one that ends with UTF-8!\n",
		 curLocale );
}

void
finish(int sig)
{
    if ( curses_active() ) {
	    reset_modes();
	    endwin();
    }
    remove_unwanted_file() ;
	release_idx();
	release_db();
	releasePattern();
	releaseCollation();
    release_filenames() ; 
	releaseProgramName();
	u_cleanup();
    if ( sig != 0 ) {
        exit( sig ) ;
    } 
}

/* TODO: additions to the usage messagee
   mail-address and project homepage.
   */
static void
usage(void)
{
	fprintf(stderr,"index -f [filter] -i -V -s -H -v -R -n -F [field separator] -q -h -u -C -V <file> <pattern>\n") ;
    fprintf(stderr,"index 2.0 uses UTF-8 text encoding, and your country's collation order for sorting.\n") ;
    fprintf(stderr," See \"man index\" for further help.\n");
	exit(0);
}
/* print a usage message. Called before initializing curses.*/
static void help(void)
{
	fprintf(stderr,"index -f [filter] -i -V -s -H -v -R -n -F [field separator] -q -h -u -C -V <file> <pattern>\n") ;
    fprintf(stderr,"\t-f, --filter [file]\n") ;
    fprintf(stderr,"\t-i, --ignore-case\n");
    fprintf(stderr,"\t-b, --verbose\n") ;
    fprintf(stderr,"\t-s, --no-labels\n") ;
    fprintf(stderr,"\t-H, --header\n") ;
    fprintf(stderr,"\t-v, --inverted\n") ;
    fprintf(stderr,"\t-r, --reverse\n") ;
    fprintf(stderr,"\t-n, --nosql\n") ;
    fprintf(stderr,"\t-F, --field-sep\n") ;
    fprintf(stderr,"\t-c, --csv\n") ;
    fprintf(stderr,"\t-q, --quiet\n") ;
    fprintf(stderr,"\t-h, --help\n") ;
    fprintf(stderr,"\t-V, --version\n") ;
    fprintf(stderr,"\t-C, --copyright\n\n") ;
    fprintf(stderr,"index is a program that lets you make and maintain small structured collections of information.\n") ;
    fprintf(stderr,"index uses UTF-8 text encoding, and your Country's collation order for sorting.\n") ;
    fprintf(stderr,"Sample files were installed at: %s\n",SAMPLES_DIR) ;
    fprintf(stderr," See \"man index\" for further help.\n");
    fprintf(stderr,"Please report any bugs to %s\n",PACKAGE_BUGREPORT) ;
    
	exit(0);
}
