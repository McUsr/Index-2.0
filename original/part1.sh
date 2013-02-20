#! /bin/sh
# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 1 (of 2)."
# Contents:  MANIFEST Makefile README createdb.c dbfunc.c dbio.c defs.h
#   index.1 main.c printdb.c samples samples/books.fmt
#   samples/books.idx samples/bphone.idx samples/cdlist.fmt
#   samples/cdlist.idx samples/cdwantlist.fmt samples/cdwantlist.idx
#   samples/pphone.fmt samples/pphone.idx searchdb.c selectdb.c util.c
# Wrapped by rsalz@papaya.bbn.com on Tue Oct 24 12:09:01 1989
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f 'MANIFEST' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'MANIFEST'\"
else
echo shar: Extracting \"'MANIFEST'\" \(840 characters\)
sed "s/^X//" >'MANIFEST' <<'END_OF_FILE'
X   File Name		Archive #	Description
X-----------------------------------------------------------
X MANIFEST                   1	
X Makefile                   1	
X README                     1	
X createdb.c                 1	
X dbfunc.c                   1	
X dbio.c                     1	
X defs.h                     1	
X index.1                    1	
X main.c                     1	
X printdb.c                  1	
X samples                    1	
X samples/books.fmt          1	
X samples/books.idx          1	
X samples/bphone.idx         1	
X samples/cdlist.fmt         1	
X samples/cdlist.idx         1	
X samples/cdwantlist.fmt     1	
X samples/cdwantlist.idx     1	
X samples/pphone.fmt         1	
X samples/pphone.idx         1	
X screen.c                   2	
X searchdb.c                 1	
X selectdb.c                 1	
X util.c                     1	
END_OF_FILE
if test 840 -ne `wc -c <'MANIFEST'`; then
    echo shar: \"'MANIFEST'\" unpacked with wrong size!
fi
# end of 'MANIFEST'
fi
if test -f 'Makefile' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'Makefile'\"
else
echo shar: Extracting \"'Makefile'\" \(928 characters\)
sed "s/^X//" >'Makefile' <<'END_OF_FILE'
X#
X# $Header: /u5/davy/progs/index/RCS/Makefile,v 1.1 89/08/09 11:06:00 davy Exp $
X#
X# Makefile for the "index" program.
X#
X# David A. Curry
X# Research Institute for Advanced Computer Science
X# Mail Stop 230-5
X# NASA Ames Research Center
X# Moffett Field, CA 94035
X# davy@riacs.edu
X#
X# $Log:	Makefile,v $
X# Revision 1.1  89/08/09  11:06:00  davy
X# Initial revision
X# 
X#
XCFLAGS=	-O
XLIBS=	-lcurses -ltermcap
X
XSRCS=	createdb.c dbfunc.c dbio.c main.c printdb.c screen.c \
X	searchdb.c selectdb.c util.c
XOBJS=	createdb.o dbfunc.o dbio.o main.o printdb.o screen.o \
X	searchdb.o selectdb.o util.o
X
Xindex: $(OBJS)
X	$(CC) $(CFLAGS) -o index $(OBJS) $(LIBS)
X
Xclean:
X	rm -f a.out core index $(OBJS) \#*
X
Xcreatedb.o:	createdb.c defs.h
Xdbfunc.o:	dbfunc.c defs.h
Xdbio.o:		dbio.c defs.h
Xmain.o:		main.c defs.h
Xprintdb.o:	printdb.c defs.h
Xscreen.o:	screen.c defs.h
Xsearchdb.o:	searchdb.c defs.h
Xselectdb.o:	selectdb.c defs.h
Xutil.o:		util.c defs.h
END_OF_FILE
if test 928 -ne `wc -c <'Makefile'`; then
    echo shar: \"'Makefile'\" unpacked with wrong size!
fi
# end of 'Makefile'
fi
if test -f 'README' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'README'\"
else
echo shar: Extracting \"'README'\" \(3220 characters\)
sed "s/^X//" >'README' <<'END_OF_FILE'
X							August 9, 1989
X
XThis is "index", Version 1.0.
X
XOver the years, I've accumulated a number of files in my directory which
Xhold lists of various things.  I have a list of business addresses, no
Xless than three personal address and phone lists, a list of all the books
XI own, a list of my compact discs, and so on.  Each of these files has a
Xdifferent format, has to be maintained manually with a text editor, can
Xonly be searched with "grep", and is difficult to keep nicely sorted.
X
XWell, I got sick and tired of this.  So, I sat down and started hacking,
Xand came up with "index".  Index allows you to maintain multiple
Xdatabases of textual information, each with a different format.  With
Xeach database, index allows you to:
X
X	- add entries
X	- delete entries
X	- edit existing entries
X	- search for entries using full regular expressions
X	- control what parts of an entry are searched
X	- print out entries matching a pattern
X	- run all or part of the database through an arbitrary
X	  formatting program
X
XThe databases index maintains are stored as simple lines of text.  Each
Xfield of an entry is a line of text, and each entry in the database is
Xmade up of a fixed number of lines.  For each database, you tell index
Xwhat each field's (line's) name is.  You can have multiple-line fields by
Xleaving that field's name blank.  There's no fancy storage algorithm,
Xthings are just stored sequentially.  But for the biggest database I've
Xgot, about 500 5-line entries, performance is just fine.
X
XIndex uses the Berkeley curses library.  It has been tested on a Sun-3
Xunder SunOS 4.0.1 (4.3BSD curses) and on a Sequent Balance under Dynix
X3.0.14 (4.2BSD curses).  It should be fairly easy to port to System V -
Xyou'll need to modify screen.c to resolve any curses incompatibilities,
Xand you'll need to change the calls to the regular expression library
Xroutines in searchdb.c to use the System V style routines.
X
XTo compile index, just say "make".  Then make a directory in your home
Xdirectory called ".index" (or set the environment variable INDEXDIR to
Xpoint somewhere else).  Now, just say "index", and you can create your
Xfirst database description file.  The directory "samples" contains some
Xsample database description files and formatting scripts:
X
X	books.idx	- for keeping a list of books sorted by author.
X	books.fmt	- formats the database into a tbl input file
X			  which prints in two columns in landscape
X			  mode.
X
X	bphone.idx	- for a business phone/address database.
X
X	cdlist.idx	- for keeping a list of compact discs.
X	cdlist.fmt	- formats the database into a wide landscape-mode
X			  tbl input file.
X
X	pphone.idx	- for keeping a personal phone/address database
X			  with home and work addresses and phone numbers
X			  for your friends.
X	pphone.fmt	- formats the database into a troff file which
X			  lists each person along with their home and
X			  work addresses.  Useful for keeping by the
X			  phone or sending Xmas cards..
X
XSince moving all my lists into the index program, I've found things a lot
Xeasier to keep track of.  I hope you'll find it as useful as I do.
X
XDavid A. Curry
XResearch Institute for Advanced Computer Science
XMail Stop 230-5
XNASA Ames Research Center
XMoffett Field, CA 94035
X
END_OF_FILE
if test 3220 -ne `wc -c <'README'`; then
    echo shar: \"'README'\" unpacked with wrong size!
fi
# end of 'README'
fi
if test -f 'createdb.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'createdb.c'\"
else
echo shar: Extracting \"'createdb.c'\" \(3325 characters\)
sed "s/^X//" >'createdb.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/createdb.c,v 1.1 89/08/09 11:06:21 davy Exp $";
X#endif
X/*
X * createdb.c - handle creating a new database.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	createdb.c,v $
X * Revision 1.1  89/08/09  11:06:21  davy
X * Initial revision
X * 
X */
X#include <sys/param.h>
X#include <curses.h>
X#include <stdio.h>
X#include "defs.h"
X
X/*
X * The message we'll print to explain what's happening.
X */
Xstatic struct message {
X	char	*m_line;
X	char	*m_arg;
X} message[] = {
X  { "You will now be placed into an editor so that you can create the\n",
X	0 },
X  { "database description file.  This file will be used by the program\n",
X	0 },
X  { "to prompt you for new items to be inserted into the database.\n",
X	0 },
X  { "\n",
X	0 },
X  { "Each line in this file is the name of a field.  It may be as long as\n",
X	0 },
X  { "you want, and may contain spaces.  The order of the lines in the file\n",
X	0 },
X  { "is the order you will be prompted for new information when inserting\n",
X	0 },
X  { "into the database.  You may leave blank lines in the file; this allows\n",
X	0 },
X  { "multiple-line entries for items such as addesses.  You may have a\n",
X	0 },
X  { "total of %d lines in the file.\n",
X	(char *) MAXDBLINES },
X  { "\n",
X	0 },
X  { "By default, all lines in an entry will be examined when searching for\n",
X	0 },
X  { "a pattern.  To make the program ignore a line, start that line with an\n",
X	0 },
X  { "exclamation point (!).\n",
X	0 },
X  { "\n",
X	0 },
X  { "The database is always sorted into ASCII collating sequence based on\n",
X	0 },
X  { "the contents of the first field.\n",
X	0 },
X  { "\n",
X	0 },
X  { "When you are finished, save the file and exit the editor.  You will\n",
X	0 },
X  { "then be placed in the main menu, where you can select other operations\n",
X	0 },
X  { "on the database.\n",
X	0 },
X  { NULL, 0 }
X};
X
X/*
X * create_db - execute an editor to allow the person to create the
X *	       index definition file.
X */
Xcreate_db(dbname)
Xchar *dbname;
X{
X	int pid;
X	char *editor;
X	char *getenv();
X	register int row;
X	char buf[BUFSIZ], fname[MAXPATHLEN];
X
X	/*
X	 * Clear the screen and move to the top.
X	 */
X	clear();
X	move(0, 0);
X
X	/*
X	 * Print out the explanatory message.
X	 */
X	for (row=0; message[row].m_line != NULL; row++)
X		printw(message[row].m_line, message[row].m_arg);
X
X	/*
X	 * Give the user a chance to read it.  Wait till they
X	 * type a carriage return before proceeding.
X	 */
X	prompt_char(++row, 0, "Type RETURN to continue: ", "\n");
X
X	/*
X	 * Use the editor the user prefers, or EDITOR if
X	 * he doesn't have anything set.
X	 */
X	if ((editor = getenv("EDITOR")) == NULL)
X		editor = EDITOR;
X
X	/*
X	 * Construct the file name.
X	 */
X	sprintf(fname, "%s/%s%s", dbasedir, dbname, IDXFILE_SUFFIX);
X
X	/*
X	 * Go back to normal tty modes.
X	 */
X	reset_modes();
X
X	/*
X	 * Spawn a child process.
X	 */
X	if ((pid = fork()) < 0) {
X		error("%s: cannot fork.\n", pname, 0, 0);
X		exit(1);
X	}
X
X	/*
X	 * Execute the editor.
X	 */
X	if (pid == 0) {
X		execl(editor, editor, fname, 0);
X		perror(editor);
X		exit(1);
X	}
X
X	/*
X	 * Wait for the editor to finish.
X	 */
X	while (wait((int *) 0) != pid)
X		;
X
X	/*
X	 * Set the tty modes up again.
X	 */
X	set_modes();
X}
END_OF_FILE
if test 3325 -ne `wc -c <'createdb.c'`; then
    echo shar: \"'createdb.c'\" unpacked with wrong size!
fi
# end of 'createdb.c'
fi
if test -f 'dbfunc.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'dbfunc.c'\"
else
echo shar: Extracting \"'dbfunc.c'\" \(3726 characters\)
sed "s/^X//" >'dbfunc.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/dbfunc.c,v 1.1 89/08/09 11:06:31 davy Exp $";
X#endif
X/*
X * dbfunc.c - database functions selected from the main menu.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	dbfunc.c,v $
X * Revision 1.1  89/08/09  11:06:31  davy
X * Initial revision
X * 
X */
X#include <curses.h>
X#include <stdio.h>
X#include "defs.h"
X
X/*
X * add_entry - add an entry to the database.
X */
Xadd_entry()
X{
X	register int i;
X	struct dbfile *realloc();
X
X	/*
X	 * Search for an empty entry in the array.
X	 */
X	for (i=0; i < dbsize; i++) {
X		/*
X		 * Found one; use it.
X		 */
X		if ((db[i].db_flag & DB_VALID) == 0) {
X			/*
X			 * Clear out any old junk.
X			 */
X			bzero(&db[i], sizeof(struct dbfile));
X
X			/*
X			 * Let the user edit the entry.
X			 */
X			if (edit_entry(&db[i], "new")) {
X				/*
X				 * Mark it valid, mark the database
X				 * as modified, and increase the
X				 * number of entries.
X				 */
X				db[i].db_flag |= DB_VALID;
X				dbmodified = 1;
X				dbentries++;
X
X				/*
X				 * Sort the array, to get this
X				 * entry into its proper place.
X				 */
X				qsort(db, dbentries, sizeof(struct dbfile),
X				      dbsort);
X			}
X
X			return;
X		}
X	}
X
X	/*
X	 * Didn't find an empty slot, so we have to allocate
X	 * some more.
X	 */
X	dbsize *= 2;
X
X	if ((db = realloc(db, dbsize * sizeof(struct dbfile))) == NULL) {
X		error("%s: out of memory.\n", pname, 0, 0);
X		exit(1);
X	}
X
X	bzero(&db[dbentries], sizeof(struct dbfile));
X
X	/*
X	 * Let the user edit the new entry.
X	 */
X	if (edit_entry(&db[dbentries], "new")) {
X		/*
X		 * Mark the entry as valid, mark the
X		 * database as modified, and increase
X		 * the number of entries.
X		 */
X		db[dbentries].db_flag |= DB_VALID;
X		dbmodified = 1;
X		dbentries++;
X
X		qsort(db, dbentries, sizeof(struct dbfile), dbsort);
X	}
X}
X
X/*
X * del_entry - delete an entry from the database.
X */
Xdel_entry(entry)
Xstruct dbfile *entry;
X{
X	char c;
X	int x, y;
X
X	/*
X	 * Prompt the user for confirmation.
X	 */
X	getyx(curscr, y, x);
X	c = prompt_char(y, 0, "Really delete this entry? ", "YyNn");
X
X	/*
X	 * Return the status of the confirmation.
X	 */
X	switch (c) {
X	case 'Y':
X	case 'y':
X		return(1);
X	case 'N':
X	case 'n':
X		return(0);
X	}
X}
X
X/*
X * find_entry - search for entries using a regular expression.
X */
Xfind_entry()
X{
X	register int i;
X	char pattern[BUFSIZ];
X
X	/*
X	 * Clear the screen and prompt for the pattern to
X	 * search for.
X	 */
X	clear();
X	prompt_str(LINES/2, 0, "Pattern to search for: ", pattern);
X
X	/*
X	 * Search.  search_db will set DB_PRINT in the entries
X	 * which match, and return non-zero if anything matched.
X	 */
X	if (search_db(pattern)) {
X		/*
X		 * Display the entries that matched.
X		 */
X		disp_entries();
X
X		/*
X		 * Clear the DB_PRINT flags.
X		 */
X		for (i=0; i < dbentries; i++)
X			db[i].db_flag &= ~DB_PRINT;
X	}
X	else {
X		/*
X		 * Nothing matched.  Tell the user.
X		 */
X		prompt_char(LINES/2, 0,
X			"No entries match pattern, type RETURN to continue: ",
X			"\n");
X	}
X}
X
X/*
X * read_db - run through the database entry by entry.
X */
Xread_db()
X{
X	register int i;
X
X	/*
X	 * Sort the database, so we're sure it's in order.
X	 */
X	qsort(db, dbentries, sizeof(struct dbfile), dbsort);
X
X	/*
X	 * Set DB_PRINT in all entries.
X	 */
X	for (i=0; i < dbentries; i++) {
X		if (db[i].db_flag & DB_VALID)
X			db[i].db_flag |= DB_PRINT;
X	}
X
X	/*
X	 * Display the entries.
X	 */
X	disp_entries();
X
X	/*
X	 * Clear DB_PRINT.
X	 */
X	for (i=0; i < dbentries; i++)
X		db[i].db_flag &= ~DB_PRINT;
X}
X
X/*
X * save_bye - save the database and exit.
X */
Xsave_bye(dbname)
Xchar *dbname;
X{
X	/*
X	 * Save the database.
X	 */
X	save_db(dbname);
X
X	/*
X	 * Exit.
X	 */
X	byebye();
X}
END_OF_FILE
if test 3726 -ne `wc -c <'dbfunc.c'`; then
    echo shar: \"'dbfunc.c'\" unpacked with wrong size!
fi
# end of 'dbfunc.c'
fi
if test -f 'dbio.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'dbio.c'\"
else
echo shar: Extracting \"'dbio.c'\" \(5120 characters\)
sed "s/^X//" >'dbio.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/dbio.c,v 1.1 89/08/09 11:06:36 davy Exp $";
X#endif
X/*
X * dbio.c - database input/output routines.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	dbio.c,v $
X * Revision 1.1  89/08/09  11:06:36  davy
X * Initial revision
X * 
X */
X#include <sys/param.h>
X#include <sys/stat.h>
X#include <curses.h>
X#include <stdio.h>
X#include "defs.h"
X
Xstruct	dbfile *db;		/* array of database entries		*/
Xstruct	idxfile idx;		/* description of the database file	*/
X
Xint	dbmodified = 0;		/* non-zero if database needs saving	*/
Xint	dbentries, dbsize;	/* number of entries, size of db array	*/
X
X/*
X * read_idxfile - read the database description file.
X */
Xread_idxfile(dbname)
Xchar *dbname;
X{
X	FILE *fp;
X	register int len;
X	char buf[BUFSIZ], idxfile[MAXPATHLEN];
X
X	/*
X	 * Construct the file name.
X	 */
X	sprintf(idxfile, "%s/%s%s", dbasedir, dbname, IDXFILE_SUFFIX);
X
X	/*
X	 * Open the file.
X	 */
X	if ((fp = fopen(idxfile, "r")) == NULL) {
X		error("%s: cannot open \"%s\".\n", pname, idxfile, 0);
X		exit(1);
X	}
X
X	/*
X	 * Zero out the structure.
X	 */
X	bzero(&idx, sizeof(struct idxfile));
X
X	/*
X	 * Read lines from the file.
X	 */
X	while (idx.idx_nlines < MAXDBLINES) {
X		/*
X		 * End of file.
X		 */
X		if (fgets(buf, sizeof(buf), fp) == NULL)
X			break;
X
X		/*
X		 * Strip the newline.
X		 */
X		len = strlen(buf) - 1;
X		buf[len] = '\0';
X
X		/*
X		 * If the first character is '!', then this line
X		 * should not participate in searches.  Save the
X		 * stuff after the '!'.  Otherwise this line does
X		 * participate in searches, save the whole line.
X		 */
X		if (*buf == '!') {
X			idx.idx_lines[idx.idx_nlines] = savestr(buf+1);
X			idx.idx_search[idx.idx_nlines] = 0;
X			len--;
X		}
X		else {
X			idx.idx_lines[idx.idx_nlines] = savestr(buf);
X			idx.idx_search[idx.idx_nlines] = 1;
X		}
X
X		/*
X		 * Increment the number of lines.
X		 */
X		idx.idx_nlines++;
X
X		/*
X		 * Save the length of the longest field name.
X		 */
X		if (len > idx.idx_maxlen)
X			idx.idx_maxlen = len;
X	}
X
X	/*
X	 * Close the file.
X	 */
X	fclose(fp);
X}
X
X/*
X * read_dbfile - read the database file itself.
X */
Xread_dbfile(dbname)
Xchar *dbname;
X{
X	FILE *fp;
X	register int i;
X	struct dbfile *malloc(), *realloc();
X	char buf[BUFSIZ], dbfile[MAXPATHLEN];
X
X	/*
X	 * Allocate some entries in the array.  16 is just an
X	 * arbitrary number.
X	 */
X	dbsize = 16;
X	dbentries = 0;
X
X	if ((db = malloc(dbsize * sizeof(struct dbfile))) == NULL) {
X		error("%s: out of memory.\n", pname, 0, 0);
X		exit(1);
X	}
X
X	/*
X	 * Construct the name of the file.
X	 */
X	sprintf(dbfile, "%s/%s%s", dbasedir, dbname, DBFILE_SUFFIX);
X
X	/*
X	 * Open the file.
X	 */
X	if ((fp = fopen(dbfile, "r")) == NULL)
X		return;
X
X	/*
X	 * Until we hit end of file...
X	 */
X	while (!feof(fp)) {
X		/*
X		 * If we need to, allocate some more entries.
X		 */
X		if (dbentries >= dbsize) {
X			dbsize *= 2;
X			db = realloc(db, dbsize * sizeof(struct dbfile));
X
X			if (db == NULL) {
X				error("%s: out of memory.\n", pname, 0, 0);
X				exit(1);
X			}
X		}
X
X		/*
X		 * Read in one entry at a time.
X		 */
X		for (i = 0; i < idx.idx_nlines; i++) {
X			/*
X			 * If we hit end of file before getting a
X			 * complete entry, toss this one.
X			 */
X			if (fgets(buf, sizeof(buf), fp) == NULL)
X				goto out;
X
X			/*
X			 * Save the length of the line, strip the
X			 * newline, and save the line.
X			 */
X			db[dbentries].db_lens[i] = strlen(buf) - 1;
X			buf[db[dbentries].db_lens[i]] = '\0';
X
X			db[dbentries].db_lines[i] = savestr(buf);
X		}
X
X		/*
X		 * Mark this entry as valid and increase the
X		 * number of entries.
X		 */
X		db[dbentries].db_flag = DB_VALID;
X		dbentries++;
X	}
X
Xout:
X	/*
X	 * Make sure what we've got is sorted.
X	 */
X	qsort(db, dbentries, sizeof(struct dbfile), dbsort);
X
X	fclose(fp);
X}
X
X/*
X * save_db - save the database to disk.
X */
Xsave_db(dbname)
Xchar *dbname;
X{
X	FILE *fp;
X	struct stat st;
X	register int i, j;
X	char realfile[MAXPATHLEN], bakfile[MAXPATHLEN];
X
X	/*
X	 * If it doesn't need saving, never mind.
X	 */
X	if (!dbmodified)
X		return;
X
X	/*
X	 * Create the name of the file and a backup file.
X	 */
X	sprintf(realfile, "%s/%s%s", dbasedir, dbname, DBFILE_SUFFIX);
X	sprintf(bakfile, "%s/#%s%s", dbasedir, dbname, DBFILE_SUFFIX);
X
X	/*
X	 * Default creation mode.
X	 */
X	st.st_mode = 0400;
X
X	/*
X	 * If the file already exists, rename it to the
X	 * backup file name.
X	 */
X	if (stat(realfile, &st) == 0)
X		rename(realfile, bakfile);
X
X	/*
X	 * Open the new file.
X	 */
X	if ((fp = fopen(realfile, "w")) == NULL) {
X		error("%s: cannot create \"%s\".\n", pname, realfile);
X		exit(1);
X	}
X
X	/*
X	 * Make sure the database is sorted.
X	 */
X	qsort(db, dbentries, sizeof(struct dbfile), dbsort);
X
X	/*
X	 * Write out the entries.
X	 */
X	for (i=0; i < dbentries; i++) {
X		if ((db[i].db_flag & DB_VALID) == 0)
X			continue;
X
X		for (j=0; j < idx.idx_nlines; j++)
X			fprintf(fp, "%s\n", db[i].db_lines[j]);
X	}
X
X	/*
X	 * Set the file mode to the mode of the original
X	 * file.  Mark the database as unmodified.
X	 */
X	fchmod(fileno(fp), st.st_mode & 0777);
X	dbmodified = 0;
X
X	fclose(fp);
X}
END_OF_FILE
if test 5120 -ne `wc -c <'dbio.c'`; then
    echo shar: \"'dbio.c'\" unpacked with wrong size!
fi
# end of 'dbio.c'
fi
if test -f 'defs.h' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'defs.h'\"
else
echo shar: Extracting \"'defs.h'\" \(2232 characters\)
sed "s/^X//" >'defs.h' <<'END_OF_FILE'
X/*
X * $Header: /u5/davy/progs/index/RCS/defs.h,v 1.1 89/08/09 11:06:09 davy Exp $
X *
X * defs.h - definitions for the index program.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	defs.h,v $
X * Revision 1.1  89/08/09  11:06:09  davy
X * Initial revision
X * 
X */
X#define PATCHLEVEL	0		/* level of patches applied	*/
X
X#define MAXDBFILES	64		/* max. no. of database files	*/
X#define MAXDBLINES	16		/* max. no. of fields in dbase	*/
X
X#define EDITOR		"/usr/ucb/vi"	/* editor to use when creating	*/
X
X#define INDEXDIR	".index"	/* directory where stuff is	*/
X#define DBFILE_SUFFIX	".db"		/* database file suffix		*/
X#define FMTFILE_SUFFIX	".fmt"		/* format program suffix	*/
X#define IDXFILE_SUFFIX	".idx"		/* index definition suffix	*/
X
X/*
X * Values for db_flag.
X */
X#define DB_VALID	0x01
X#define DB_PRINT	0x02
X
X/*
X * For 4.2 curses.
X */
X#ifndef cbreak
X#define cbreak() crmode()
X#endif
X#ifndef nocbreak
X#define nocbreak() nocrmode()
X#endif
X
X/*
X * Usually defined in ttychars.h.
X */
X#ifndef CTRL
X#define CTRL(c)		('c' & 037)
X#endif
X
X/*
X * Structure to hold the contents of the index definition.
X */
Xstruct idxfile {
X	int	idx_maxlen;		/* longest field length		*/
X	int	idx_nlines;		/* number of lines per entry	*/
X	char	idx_search[MAXDBLINES];	/* non-zero if field searchable	*/
X	char	*idx_lines[MAXDBLINES];	/* strings naming the fields	*/
X};
X
X/*
X * Structure to hold a database entry.
X */
Xstruct dbfile {
X	int	db_flag;		/* flag, see above		*/
X	int	db_lens[MAXDBLINES];	/* lengths of line contents	*/
X	char	*db_lines[MAXDBLINES];	/* lines in the entry		*/
X};
X
Xextern char	*pname;			/* program name			*/
X
Xextern int	dbsize;			/* size of db array		*/
Xextern int	igncase;		/* non-zero if -i switch given	*/
Xextern int	verbose;		/* non-zero if -v switch given	*/
Xextern int	dbentries;		/* number of entries in db	*/
Xextern int	dbmodified;		/* non-zero if db needs saving	*/
X
Xextern char	dbasedir[];		/* path to the INDEXDIR		*/
X
Xextern struct	dbfile *db;		/* database entries array	*/
Xextern struct	idxfile idx;		/* index definition structure	*/
X
Xchar		*savestr();
Xchar		*select_db();
X
Xint		byebye();
Xint		dbsort();
END_OF_FILE
if test 2232 -ne `wc -c <'defs.h'`; then
    echo shar: \"'defs.h'\" unpacked with wrong size!
fi
# end of 'defs.h'
fi
if test -f 'index.1' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'index.1'\"
else
echo shar: Extracting \"'index.1'\" \(7844 characters\)
sed "s/^X//" >'index.1' <<'END_OF_FILE'
X.\"
X.\" $Header: /u5/davy/progs/index/RCS/index.1,v 1.1 89/08/09 11:09:42 davy Exp $
X.\"
X.\" David A. Curry
X.\" Research Institute for Advanced Computer Science
X.\" Mail Stop 230-5
X.\" NASA Ames Research Center
X.\" Moffett Field, CA 94035
X.\"
X.\" $Log:	index.1,v $
XRevision 1.1  89/08/09  11:09:42  davy
XInitial revision
X
X.\"
X.TH INDEX 1 "27 July 1989" LOCAL
X.SH NAME
Xindex \- maintain simple databases
X.SH SYNOPSIS
Xindex
X[
X.B \-f
X.I filter
X] [
X.B \-i
X] [
X.B \-v
X] [
X.I database
X] [
X.I pattern
X]
X.SH DESCRIPTION
X.PP
X.I Index
Xis used to maintain simple databases such as address lists,
Xlists of books or compact discs,
Xand so on.
XAll databases are stored as simple text files in a single directory.
XBy default,
Xthis directory resides in your home directory,
Xand is called
X.IR \&.index .
XYou can specify a different path name for the directory by setting
Xthe environment variable
X.B \s-1INDEXDIR\s0
Xto the directory's full path name.
X.PP
XWhen invoked with no arguments,
X.I index
Xwill present you with a list of the databases you have,
Xand ask you to select the one you want to work with.
XTo create a new database,
Xsimply type the name of a non-existent database to the prompt.
XThe name of an existing database may also be specified on the command line,
Xbypassing this step and going directly to the main menu.
X.SH CREATING A NEW DATABASE
X.PP
XWhen you specify the name of a non-existent database to the database
Xselection prompt,
Xyou will be placed in a text editor to create the database description
Xfile.
XThis file is simply a list of the field names for the database,
Xone per line.
XYou may have up to 16 fields in each database.
XBlank lines may be used for continuation lines in multiple-line fields,
Xsuch as addresses.
X.PP
XThe database is always sorted by the first field.
XWhen searching the database,
Xthe default is to search all fields for the pattern.
XTo specify that a field should be ignored in searching,
Xyou should place an exclamation point (!) in the first position on
Xthat line.
X.PP
XWhen you have created the database description file,
Xsave the file and exit the editor.
XYou will then be placed in the main menu,
Xwhere you can manipulate the database.
X.SH THE MAIN MENU
X.PP
XThe main menu is the point from which the database can be manipulated.
XThis menu provides you with several commands:
X.IP \fBa\fP
XAdd a new entry to the database.
XYou will be presented with a list of the fields in a database entry,
Xand allowed to fill them in.
XAs you type,
Xcharacters are inserted at the current cursor location.
XThe editing commands available are a subset of those provided by the
X\s-1EMACS\s0 text editor:
X.RS
X.IP \fB^A\fP
XMove the cursor to the beginning of the line.
X.IP \fB^B\fP
XMove the cursor backward one character.
X.IP \fB^D\fP
XDelete the character under the cursor.
X.IP \fB^E\fP
XMove the cursor to the end of the line.
X.IP \fB^F\fP
XMove the cursor forward one character.
X.IP \fB^H\fP
XBackspace,
Xdeleting the character in front of the cursor.
XThe \s-1DEL\s0 key also performs this function.
X.IP \fB^K\fP
XDelete from the cursor position to the end of the line.
X.IP \fB^L\fP
XRedraw the screen.
X.IP \fB<\s-1RET\s0>
XPressing \s-1RETURN\s0 moves to the next line,
Xcolumn one.
XIf you're on the last line,
Xthis wraps around to the first line.
X.IP \fB^N\fP
XMove to the next line,
Xwithout moving to column one.
XIf you're on the last line,
Xthis wraps around to the first line.
X.IP \fB^P\fP
XMove to the previous line.
XIf you're on the first line,
Xthis wraps around to the last line.
X.IP \fB<\s-1ESC\s0>\fP
XPressing the \s-1ESCAPE\s0 key tells
X.I index
Xthat you're done editing the entry.
XYou will be asked whether you want to save the entry in the database.
XIf you say yes,
Xit will be saved.
XIf you say no,
Xthe data you just entered will be discarded.
XIf you press \s-1RETURN\s0,
Xyou will be returned to the editing mode.
X.RE
X.IP \fBf\fP
XFind an entry in the database.
XYou will be prompted for a pattern to search for,
Xand then all entries which match the pattern will be displayed,
Xone at a time.
XThe pattern may be any regular expression,
Xas described in
X.IR ed (1).
XCase is distinguished unless the
X.B \-i
Xoption was given on the command line.
XSee the description of the ``\fBr\fP'' command for the options available
Xto you with each entry displayed.
X.IP \fBr\fP
XRead the database entry by entry.
XEach entry in the database is printed on the screen,
Xalong with two numbers indicating the number of entries in the database,
Xand the sequential index number of this entry (e.g., ``123/500'').
XAs each entry is printed,
Xyou will be allowed to execute the following commands:
X.RS
X.IP \fB<\s-1RET\s0>\fP
XPressing the \s-1RETURN\s0 key will move to the next database entry.
X.IP \fB\-\fP
XReturn to the previous database entry.
X.IP \fBd\fP
XDelete this entry from the database.
XYou will be prompted to confirm this operation.
X.IP \fBe\fP
XEdit this database entry.
XSee the description of the main menu ``\fBa\fP'' command for a list
Xof the editing commands available.
XAfter you press \s-1ESCAPE\s0 and indicate whether you wish to save what
Xyou have edited,
Xyou will be returned to this mode again.
X.IP \fBq\fP
XReturn to the main menu without looking at the rest of the entries.
X.IP \fB^L\fP
XRedraw the screen.
X.RE
X.IP \fBs\fP
XSave any modifications to the database.
X.IP \fBq\fP
XSave any modifications to the database,
Xand exit
X.IR index .
X.IP \fBx\fP
XExit
X.I index
Xwithout saving the database.
XIf the database has been modified,
Xyou will be asked to confirm this operation.
X.SH SEARCHING FROM THE COMMAND LINE
X.PP
XIf a database name and pattern are both specified on the command line,
Xthe pattern will be searched for in the database,
Xand any matching entries will be printed on the standard output.
XEach entry will be printed one field per line,
Xpreceded by its field name.
X.PP
XThe pattern may be any valid regular expression,
Xas defined by
X.IR ed (1).
XCase is significant,
Xunless the
X.B \-i
Xoption is given.
XTo match all entries in the database,
Xuse the regular expression ``.'' (matches any character).
X.PP
XBy default,
X.I index
Xwill not print any blank lines in the entry,
Xin order to make the output more readable.
XBy specifying the
X.B \-v
Xoption,
Xyou can tell
X.I index
Xto print all lines in the entry,
Xeven if they have nothing on them.
X.SH FILTERS
X.PP
XIf the
X.B \-f
Xoption is specified with the name of a filter,
Xthen when a database name and pattern are also given,
Xall matching entries will be sent through the filter program instead
Xof to the standard output.
X.PP
XThe first line of output will contain the field names for the database,
Xseparated by tabs.
XEach following line will contain one database entry,
Xwith fields separated by tabs.
XThis format is easily dealt with using programs such as
X.IR awk (1).
X.PP
XAs a convenience,
Xfiltering programs may be stored in the database directory with a
X``.fmt'' extension in their file name.
XThe program is first searched for here (by adding the name extension),
Xand if it is not found,
Xit is then searched for (without the extension) in the standard search
Xpath.
X.SH EXAMPLE
X.PP
XThe following database description file implements a business phone number
Xlist.
XIt allows three lines for the company address,
Xand two lines for electronic mail addresses.
XThe extra fields ``Product'' and ``Keywords'' can be used to provide
Xadditional patterns to search for (e.g.,
Xyou might want to search for all disk vendors).
XThe ``!'' character inhibits searching the title,
Xaddress,
Xand telephone number for patterns.
X.sp
X.nf
XName
X!Title
XCompany
X!Address
X!
X!
X!Phone
XElectronic Mail
X
XProduct
XKeywords
X.fi
X.SH LIMITATIONS
X.PP
XEach database may have no more than 16 fields.
X.PP
XYou may not have more than 64 separate databases.
X.PP
XA database may contain as many entries as you can allocate memory for.
X.SH SEE ALSO
X.IR awk (1),
X.IR ed (1)
X.SH AUTHOR
XDavid A. Curry, Research Institute for Advanced Computer Science
END_OF_FILE
if test 7844 -ne `wc -c <'index.1'`; then
    echo shar: \"'index.1'\" unpacked with wrong size!
fi
# end of 'index.1'
fi
if test -f 'main.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'main.c'\"
else
echo shar: Extracting \"'main.c'\" \(1996 characters\)
sed "s/^X//" >'main.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/main.c,v 1.1 89/08/09 11:06:42 davy Exp $";
X#endif
X/*
X * main.c - main routine for index program.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	main.c,v $
X * Revision 1.1  89/08/09  11:06:42  davy
X * Initial revision
X * 
X */
X#include <sys/param.h>
X#include <curses.h>
X#include <stdio.h>
X#include "defs.h"
X
Xint	igncase = 0;			/* non-zero if -i flag given	*/
Xint	verbose = 0;			/* non-zero if -v flag given	*/
X
Xchar	*pname;				/* program name			*/
Xchar	dbasedir[MAXPATHLEN];		/* path to database directory	*/
X
Xmain(argc, argv)
Xchar **argv;
Xint argc;
X{
X	char *database, *filter, *pattern;
X
X	pname = *argv;
X	database = filter = pattern = NULL;
X
X	/*
X	 * Process arguments.
X	 */
X	while (--argc) {
X		if (**++argv == '-') {
X			switch (*++*argv) {
X			case 'f':			/* filter	*/
X				if (--argc <= 0)
X					usage();
X
X				filter = *++argv;
X				continue;
X			case 'i':			/* ignore case	*/
X				igncase++;
X				continue;
X			case 'v':			/* verbose	*/
X				verbose++;
X				continue;
X			}
X		}
X
X		/*
X		 * database argument is first.
X		 */
X		if (database == NULL) {
X			database = *argv;
X			continue;
X		}
X
X		/*
X		 * pattern argument is next.
X		 */
X		if (pattern == NULL) {
X			pattern = *argv;
X			continue;
X		}
X
X		usage();
X	}
X
X	/*
X	 * Get the path of the database directory.
X	 */
X	set_dbase_dir();
X
X	/*
X	 * If they didn't specify a database, put them in
X	 * the selection routine.
X	 */
X	if (database == NULL)
X		database = select_db();
X
X	/*
X	 * Open the database and read it in.
X	 */
X	read_idxfile(database);
X	read_dbfile(database);
X
X	/*
X	 * If they didn't specify a pattern, go to the
X	 * main menu.  Otherwise, search the database
X	 * for the pattern, and print the results.
X	 */
X	if (pattern == NULL) {
X		main_menu(database);
X		reset_modes();
X	}
X	else {
X		search_db(pattern);
X		print_db(database, filter);
X	}
X
X	exit(0);
X}
END_OF_FILE
if test 1996 -ne `wc -c <'main.c'`; then
    echo shar: \"'main.c'\" unpacked with wrong size!
fi
# end of 'main.c'
fi
if test -f 'printdb.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'printdb.c'\"
else
echo shar: Extracting \"'printdb.c'\" \(2557 characters\)
sed "s/^X//" >'printdb.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/printdb.c,v 1.1 89/08/09 11:06:47 davy Exp $";
X#endif
X/*
X * printdb.c - print entries from the database.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	printdb.c,v $
X * Revision 1.1  89/08/09  11:06:47  davy
X * Initial revision
X * 
X */
X#include <sys/file.h>
X#include <stdio.h>
X#include "defs.h"
X
X/*
X * print_db - print out entries marked DB_PRINT in the database.
X */
Xprint_db(dbname, filter)
Xchar *dbname, *filter;
X{
X	FILE *pp;
X	FILE *popen();
X	char buf[BUFSIZ];
X	register int i, j;
X	register char *tab;
X
X	/*
X	 * If no filter was specified, we just spit the entries out,
X	 * with their field names, to standard output.
X	 */
X	if (filter == NULL) {
X		for (i=0; i < dbentries; i++) {
X			if ((db[i].db_flag & DB_VALID) == 0)
X				continue;
X			if ((db[i].db_flag & DB_PRINT) == 0)
X				continue;
X
X			for (j=0; j < idx.idx_nlines; j++) {
X				if (!verbose) {
X					if (db[i].db_lines[j][0] == '\0')
X						continue;
X				}
X
X				sprintf(buf, "%s%s", idx.idx_lines[j],
X					idx.idx_lines[j][0] ? ":" : "");
X				printf("%-*s%s\n", idx.idx_maxlen + 2,
X				       buf, db[i].db_lines[j]);
X			}
X
X			putchar('\n');
X		}
X
X		return;
X	}
X
X	/*
X	 * Otherwise, we set up a pipe to the filter, and print
X	 * first the field names, and then the fields.  We do
X	 * this one entry per line, with fields separated by
X	 * tabs.
X	 */
X
X	/*
X	 * Create the path to a formatting program in the database
X	 * directory.
X	 */
X	sprintf(buf, "%s/%s%s", dbasedir, filter, FMTFILE_SUFFIX);
X
X	/*
X	 * If that's not there, then assume they gave us some
X	 * program name (like "more" or something), and just
X	 * stick it in there.
X	 */
X	if (access(buf, X_OK) < 0)
X		strcpy(buf, filter);
X
X	/*
X	 * Open the pipe.
X	 */
X	if ((pp = popen(buf, "w")) == NULL) {
X		error("%s: cannot execute \"%s\".\n", pname, filter, 0);
X		exit(1);
X	}
X
X	/*
X	 * Print the field names, separated by tabs.
X	 */
X	tab = "";
X	for (i=0; i < idx.idx_nlines; i++) {
X		fprintf(pp, "%s%s", tab, idx.idx_lines[i]);
X		tab = "\t";
X	}
X
X	putc('\n', pp);
X
X	/*
X	 * Print the entries, with fields separated
X	 * by tabs.
X	 */
X	for (i=0; i < dbentries; i++) {
X		if ((db[i].db_flag & DB_VALID) == 0)
X			continue;
X		if ((db[i].db_flag & DB_PRINT) == 0)
X			continue;
X
X		tab = "";
X		for (j=0; j < idx.idx_nlines; j++) {
X			fprintf(pp, "%s%s", tab, db[i].db_lines[j]);
X			tab = "\t";
X		}
X
X		putc('\n', pp);
X	}
X
X	/*
X	 * Close the pipe.
X	 */
X	pclose(pp);
X}
END_OF_FILE
if test 2557 -ne `wc -c <'printdb.c'`; then
    echo shar: \"'printdb.c'\" unpacked with wrong size!
fi
# end of 'printdb.c'
fi
if test ! -d 'samples' ; then
    echo shar: Creating directory \"'samples'\"
    mkdir 'samples'
fi
if test -f 'samples/books.fmt' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/books.fmt'\"
else
echo shar: Extracting \"'samples/books.fmt'\" \(616 characters\)
sed "s/^X//" >'samples/books.fmt' <<'END_OF_FILE'
X#!/bin/sh
X#
X# books.fmt - format the books database into a nice troff-able list
X#
X
X#
X# Put out troff header (set point size, etc.)
X#
Xcat << EOF
X.\"
X.\" Run me off with "tbl | troff -ms"
X.\" Then use "psdit -p/land.pro"
X.\"
X.nr LL 10.25i
X.nr PO .25i
X.nr HM .5i
X.nr FM .5i
X.nr PS 6
X.nr VS 8
X.ll 10.25i
X.po .25i
X.pl 8.5i
X.ps 6
X.vs 8
X.OH "''%'\*(DY'"
X.EH "''%'\*(DY'"
X.ND
X.P1
X.2C
X.TS H
Xc c c c c
Xl l l l c.
XEOF
X
X#
X# Let awk handle reformatting.
X#
Xawk    'BEGIN	{ FS = "\t" }
X		{ printf "%s\t%s\t%s\t%s\t%s\n", $1, $2, $3, $4, $5
X		  if (NR == 1)
X			printf "=\n.TH\n"
X		}'
X
X#
X# Put out troff footer.
X#
Xcat << EOF
X.TE
XEOF
END_OF_FILE
if test 616 -ne `wc -c <'samples/books.fmt'`; then
    echo shar: \"'samples/books.fmt'\" unpacked with wrong size!
fi
chmod +x 'samples/books.fmt'
# end of 'samples/books.fmt'
fi
if test -f 'samples/books.idx' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/books.idx'\"
else
echo shar: Extracting \"'samples/books.idx'\" \(46 characters\)
sed "s/^X//" >'samples/books.idx' <<'END_OF_FILE'
XAuthor
XTitle
X!Addt'l Author(s)
XSeries
X!Status
END_OF_FILE
if test 46 -ne `wc -c <'samples/books.idx'`; then
    echo shar: \"'samples/books.idx'\" unpacked with wrong size!
fi
# end of 'samples/books.idx'
fi
if test -f 'samples/bphone.idx' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/bphone.idx'\"
else
echo shar: Extracting \"'samples/bphone.idx'\" \(74 characters\)
sed "s/^X//" >'samples/bphone.idx' <<'END_OF_FILE'
XName
X!Title
XCompany
X!Address
X!
X!
X!Phone
XElectronic Mail
X
XProduct
XKeywords
END_OF_FILE
if test 74 -ne `wc -c <'samples/bphone.idx'`; then
    echo shar: \"'samples/bphone.idx'\" unpacked with wrong size!
fi
# end of 'samples/bphone.idx'
fi
if test -f 'samples/cdlist.fmt' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/cdlist.fmt'\"
else
echo shar: Extracting \"'samples/cdlist.fmt'\" \(635 characters\)
sed "s/^X//" >'samples/cdlist.fmt' <<'END_OF_FILE'
X#!/bin/sh
X#
X# cdlist.fmt - format the cdlist database into a nice troff-able list
X#
X#
X
X#
X# Put out troff header (set point size, etc.)
X#
Xcat << EOF
X.\"
X.\" Run me off with "tbl | troff -ms"
X.\" Then use "psdit -p/land.pro"
X.\"
X.nr LL 10i
X.nr PO .5i
X.nr HM .5i
X.nr FM .5i
X.nr PS 9
X.nr VS 11
X.ll 10i
X.po 0.5i
X.pl 8.5i
X.ps 9
X.vs 11
X.TS H
Xcenter, expand;
Xc s s s s s
Xc c c c c c
Xl l l l l l.
XCompact Disc List - \*(DY
XEOF
X
X#
X# Let awk handle reformatting.
X#
Xawk    'BEGIN	{ FS = "\t" }
X		{ printf "%s\t%s\t%s\t%s\t%s\t%s\n", $1, $2, $3, $4, $5, $6
X		  if (NR == 1)
X			printf "=\n.TH\n"
X		}'
X
X#
X# Put out troff footer.
X#
Xcat << EOF
X.TE
XEOF
END_OF_FILE
if test 635 -ne `wc -c <'samples/cdlist.fmt'`; then
    echo shar: \"'samples/cdlist.fmt'\" unpacked with wrong size!
fi
chmod +x 'samples/cdlist.fmt'
# end of 'samples/cdlist.fmt'
fi
if test -f 'samples/cdlist.idx' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/cdlist.idx'\"
else
echo shar: Extracting \"'samples/cdlist.idx'\" \(54 characters\)
sed "s/^X//" >'samples/cdlist.idx' <<'END_OF_FILE'
XArtist
XTitle
X!Orchestra
XClassification
X!Label
X!Number
END_OF_FILE
if test 54 -ne `wc -c <'samples/cdlist.idx'`; then
    echo shar: \"'samples/cdlist.idx'\" unpacked with wrong size!
fi
# end of 'samples/cdlist.idx'
fi
if test -f 'samples/cdwantlist.fmt' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/cdwantlist.fmt'\"
else
echo shar: Extracting \"'samples/cdwantlist.fmt'\" \(650 characters\)
sed "s/^X//" >'samples/cdwantlist.fmt' <<'END_OF_FILE'
X#!/bin/sh
X#
X# cdwantlist.fmt - format the cdwantlist database into a nice troff-able list
X#
X#
X
X#
X# Put out troff header (set point size, etc.)
X#
Xcat << EOF
X.\"
X.\" Run me off with "tbl | troff -ms"
X.\" Then use "psdit -p/land.pro"
X.\"
X.nr LL 10i
X.nr PO .5i
X.nr HM .5i
X.nr FM .5i
X.nr PS 9
X.nr VS 11
X.ll 10.0i
X.po 0.5i
X.pl 8.5i
X.ps 9
X.vs 11
X.TS H
Xcenter, expand;
Xc s s s s s
Xc c c c c c
Xl l l l l l.
XCompact Disc Want List - \*(DY
XEOF
X
X#
X# Let awk handle reformatting.
X#
Xawk    'BEGIN	{ FS = "\t" }
X		{ printf "%s\t%s\t%s\t%s\t%s\t%s\n", $1, $2, $3, $4, $5, $6
X		  if (NR == 1)
X			printf "=\n.TH\n"
X		}'
X
X#
X# Put out troff footer.
X#
Xcat << EOF
X.TE
XEOF
END_OF_FILE
if test 650 -ne `wc -c <'samples/cdwantlist.fmt'`; then
    echo shar: \"'samples/cdwantlist.fmt'\" unpacked with wrong size!
fi
chmod +x 'samples/cdwantlist.fmt'
# end of 'samples/cdwantlist.fmt'
fi
if test -f 'samples/cdwantlist.idx' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/cdwantlist.idx'\"
else
echo shar: Extracting \"'samples/cdwantlist.idx'\" \(54 characters\)
sed "s/^X//" >'samples/cdwantlist.idx' <<'END_OF_FILE'
XArtist
XTitle
X!Orchestra
XClassification
X!Label
X!Number
END_OF_FILE
if test 54 -ne `wc -c <'samples/cdwantlist.idx'`; then
    echo shar: \"'samples/cdwantlist.idx'\" unpacked with wrong size!
fi
# end of 'samples/cdwantlist.idx'
fi
if test -f 'samples/pphone.fmt' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/pphone.fmt'\"
else
echo shar: Extracting \"'samples/pphone.fmt'\" \(1453 characters\)
sed "s/^X//" >'samples/pphone.fmt' <<'END_OF_FILE'
X#!/bin/sh
X#
X# pphone.fmt - format the pphone database into a nice troff-able
X#	       phone list of the format
X#
X#	Name in bold face
X#	   Home Address		Work Address
X#	   Home Phone		Work Phone
X#				Electronic Mail Address
X#
X
X#
X# Put out troff header (set point size, etc.)
X#
Xcat << EOF
X.\"
X.\" Run me off with "troff -ms"
X.\"
X.nr LL 6.5i
X.nr PO 1i
X.nr PS 12
X.nr VS 14
X.ll 6.5i
X.po 1i
X.ps 12
X.vs 14
X.nf
XEOF
X
X#
X# Let awk handle reformatting.  Basically, we want to print out, for
X# each entry:
X#
X#	.ne 6v		<-- makes sure whole entry fits on page
X#	.B "Name"
X#	.mk
X#	.in .5i
X#	Home Address
X#	Home Phone
X#	.rt
X#	.in 3i
X#	Work Address
X#	Work Phone
X#	Electronic Address
X#	.sp
X#
X# We have special stuff to handle blank lines in the home and/or work
X# address, and then at the end we have to put in some blank lines to
X# make sure the work address is at least as long as the home address,
X# since we're using marks/returns.
X#
Xawk    'BEGIN	{ FS = "\t" }
X		{ if (NR > 1) {
X			home = ""
X			homen = 0
X			for (i=2; i <= 4; i++) {
X				if (length($i) > 0) {
X					home = home $i "\n"
X					homen++
X				}
X			}
X
X			work = ""
X			workn = 0
X			for (i=5; i <= 9; i++) {
X				if (length($i) > 0) {
X					work = work $i "\n"
X					workn++
X				}
X			}
X
X			printf ".ne 6v\n.B \"%s\"\n", $1
X			printf ".in .5i\n.mk\n"
X			printf "%s", home
X			printf ".rt\n.in 3i\n"
X			printf "%s", work
X
X			while (homen > workn) {
X				printf "\n"
X				homen--
X			}
X
X			printf ".sp\n.in 0\n"
X		  }
X		}'
END_OF_FILE
if test 1453 -ne `wc -c <'samples/pphone.fmt'`; then
    echo shar: \"'samples/pphone.fmt'\" unpacked with wrong size!
fi
chmod +x 'samples/pphone.fmt'
# end of 'samples/pphone.fmt'
fi
if test -f 'samples/pphone.idx' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'samples/pphone.idx'\"
else
echo shar: Extracting \"'samples/pphone.idx'\" \(89 characters\)
sed "s/^X//" >'samples/pphone.idx' <<'END_OF_FILE'
XName
X!Home Address
X!
X!Home Phone
X!Work Address
X!
X!
X!Work Phone
XElectronic Mail
X!Birthday
END_OF_FILE
if test 89 -ne `wc -c <'samples/pphone.idx'`; then
    echo shar: \"'samples/pphone.idx'\" unpacked with wrong size!
fi
# end of 'samples/pphone.idx'
fi
if test -f 'searchdb.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'searchdb.c'\"
else
echo shar: Extracting \"'searchdb.c'\" \(1943 characters\)
sed "s/^X//" >'searchdb.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/searchdb.c,v 1.1 89/08/09 11:06:59 davy Exp $";
X#endif
X/*
X * searchdb.c - database search routine.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	searchdb.c,v $
X * Revision 1.1  89/08/09  11:06:59  davy
X * Initial revision
X * 
X */
X#include <curses.h>
X#include <ctype.h>
X#include <stdio.h>
X#include "defs.h"
X
X/*
X * search_db - search the database for the pattern.
X */
Xsearch_db(pattern)
Xchar *pattern;
X{
X	int code = 0;
X	char *re_comp();
X	char buf[BUFSIZ];
X	register int i, j;
X	register char *p, *q;
X
X	/*
X	 * If we're ignoring case, convert the pattern
X	 * to all lower case.
X	 */
X	if (igncase) {
X		for (p = pattern; *p != NULL; p++) {
X			if (isupper(*p))
X				*p = tolower(*p);
X		}
X	}
X
X	/*
X	 * Compile the regular expression.
X	 */
X	if (re_comp(pattern) != NULL)
X		return(0);
X
X	/*
X	 * For all entries...
X	 */
X	for (i=0; i < dbentries; i++) {
X		/*
X		 * For each line in the entry...
X		 */
X		for (j=0; j < idx.idx_nlines; j++) {
X			/*
X			 * If this line is not to be searched,
X			 * skip it.
X			 */
X			if (idx.idx_search[j] == 0)
X				continue;
X
X			/*
X			 * If ignoring case, copy the line an
X			 * convert it to lower case.  Otherwise,
X			 * use it as is.
X			 */
X			if (igncase) {
X				p = db[i].db_lines[j];
X				q = buf;
X
X				while (*p != NULL) {
X					*q++ = isupper(*p) ? tolower(*p) : *p;
X					p++;
X				}
X
X				*q = '\0';
X
X				/*
X				 * If we get a match, mark this entry as
X				 * printable.
X				 */
X				if (re_exec(buf)) {
X					db[i].db_flag |= DB_PRINT;
X					code = 1;
X				}
X			}
X			else {
X				/*
X				 * If we get a match, mark this entry
X				 * as printable.
X				 */
X				if (re_exec(db[i].db_lines[j])) {
X					db[i].db_flag |= DB_PRINT;
X					code = 1;
X				}
X			}
X		}
X	}
X
X	/*
X	 * Return whether or not we found anything.
X	 */
X	return(code);
X}
X
END_OF_FILE
if test 1943 -ne `wc -c <'searchdb.c'`; then
    echo shar: \"'searchdb.c'\" unpacked with wrong size!
fi
# end of 'searchdb.c'
fi
if test -f 'selectdb.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'selectdb.c'\"
else
echo shar: Extracting \"'selectdb.c'\" \(2630 characters\)
sed "s/^X//" >'selectdb.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/selectdb.c,v 1.1 89/08/09 11:07:06 davy Exp $";
X#endif
X/*
X * selectdb.c - database selection routines.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	selectdb.c,v $
X * Revision 1.1  89/08/09  11:07:06  davy
X * Initial revision
X * 
X */
X#include <sys/param.h>
X#include <sys/dir.h>
X#include <curses.h>
X#include <stdio.h>
X#include "defs.h"
X
X/*
X * select_db - allow the user to select a database from the list of databases
X *	       he has, or to create a new database.
X */
Xchar *
Xselect_db()
X{
X	char dbname[MAXPATHLEN];
X	char *dblist[MAXDBFILES];
X	register int ndbs, i, row, col, spread;
X
X	/*
X	 * Load the list of databases the user has.
X	 */
X	ndbs = load_dblist(dblist);
X	spread = (ndbs + 3) / 4;
X
X	/*
X	 * Set tty modes, clear screen.
X	 */
X	set_modes();
X	clear();
X
X	/*
X	 * Print the list of databases in four columns.
X	 */
X	for (row = 0; row < spread; row++) {
X		for (col = 0; col < 4; col++) {
X			i = col * spread + row;
X
X			if (dblist[i])
X				mvaddstr(row, col * COLS/4, dblist[i]);
X		}
X	}
X
X	*dbname = '\0';
X
X	/*
X	 * Prompt for the name of a database.
X	 */
X	while (*dbname == '\0')
X		prompt_str(spread+2, 0, "Select a database: ", dbname);
X
X	/*
X	 * If the database exists, return its name.
X	 */
X	for (i = 0; i < ndbs; i++) {
X		if (!strcmp(dbname, dblist[i]))
X			return(savestr(dbname));
X	}
X
X	/*
X	 * Doesn't exist - create it.
X	 */
X	create_db(dbname);
X	return(savestr(dbname));
X}
X
X/*
X * load_dblist - load up a list of the databases the user has.
X */
Xload_dblist(dblist)
Xchar **dblist;
X{
X	DIR *dp;
X	int ndbs;
X	char *rindex();
X	register char *s;
X	extern int compare();
X	register struct direct *d;
X
X	ndbs = 0;
X
X	/*
X	 * Open the database directory.
X	 */
X	if ((dp = opendir(dbasedir)) == NULL) {
X		fprintf(stderr, "%s: cannot open \"%s\".\n", pname, dbasedir);
X		exit(1);
X	}
X
X	/*
X	 * Read entries from the directory...
X	 */
X	while ((d = readdir(dp)) != NULL) {
X		/*
X		 * Search for a "." in the name, which marks
X		 * the suffix.
X		 */
X		if ((s = rindex(d->d_name, '.')) == NULL)
X			continue;
X
X		/*
X		 * If this is an index definition file, save its
X		 * name.
X		 */
X		if (!strcmp(s, IDXFILE_SUFFIX)) {
X			if (ndbs < MAXDBFILES) {
X				*s = '\0';
X				dblist[ndbs++] = savestr(d->d_name);
X			}
X		}
X	}
X
X	/*
X	 * Sort the list.
X	 */
X	qsort(dblist, ndbs, sizeof(char *), compare);
X	closedir(dp);
X
X	return(ndbs);
X}
X
X/*
X * compare - comparis routine for qsort of dblist.
X */
Xstatic int
Xcompare(a, b)
Xchar **a, **b;
X{
X	return(strcmp(*a, *b));
X}
END_OF_FILE
if test 2630 -ne `wc -c <'selectdb.c'`; then
    echo shar: \"'selectdb.c'\" unpacked with wrong size!
fi
# end of 'selectdb.c'
fi
if test -f 'util.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'util.c'\"
else
echo shar: Extracting \"'util.c'\" \(2490 characters\)
sed "s/^X//" >'util.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/util.c,v 1.1 89/08/09 11:07:12 davy Exp $";
X#endif
X/*
X * util.c - utility routines for index program.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	util.c,v $
X * Revision 1.1  89/08/09  11:07:12  davy
X * Initial revision
X * 
X */
X#include <curses.h>
X#include <stdio.h>
X#include "defs.h"
X
X/*
X * set_dbase_dir - set the path to the database directory.
X */
Xset_dbase_dir()
X{
X	char *s;
X	char *getenv();
X
X	/*
X	 * Look for it in the environment.
X	 */
X	if ((s = getenv("INDEXDIR")) != NULL) {
X		strcpy(dbasedir, s);
X		return;
X	}
X
X	/*
X	 * Otherwise, it's in the home directory.
X	 */
X	if ((s = getenv("HOME")) == NULL) {
X		fprintf(stderr, "%s: cannot get home directory.\n", pname);
X		exit(1);
X	}
X
X	/*
X	 * Make the name.
X	 */
X	sprintf(dbasedir, "%s/%s", s, INDEXDIR);
X}
X
X/*
X * dbsort - comparison routine for qsort of database entries.
X */
Xdbsort(a, b)
Xstruct dbfile *a, *b;
X{
X	register int i, n;
X
X	/*
X	 * Sort invalid entries to the end.
X	 */
X	if ((a->db_flag & DB_VALID) == 0) {
X		if ((b->db_flag & DB_VALID) == 0)
X			return(0);
X
X		return(1);
X	}
X
X	if ((b->db_flag & DB_VALID) == 0)
X		return(-1);
X
X	/*
X	 * Sort on first field, then try secondary fields.
X	 */
X	n = 0;
X	for (i=0; (i < idx.idx_nlines) && (n == 0); i++)
X		n = strcmp(a->db_lines[i], b->db_lines[i]);
X	
X	return(n);
X}
X
X/*
X * error - reset tty modes and print an error message.
X */
Xerror(fmt, arg1, arg2, arg3)
Xchar *fmt, *arg1, *arg2, *arg3;
X{
X	reset_modes();
X
X	fprintf(stderr, fmt, arg1, arg2, arg3);
X}
X
X/*
X * savestr - save a string in dynamically allocated memory.
X */
Xchar *
Xsavestr(str)
Xchar *str;
X{
X	char *s;
X	char *malloc();
X
X	if ((s = malloc(strlen(str) + 1)) == NULL) {
X		reset_modes();
X
X		fprintf(stderr, "%s: out of memory.\n", pname);
X		exit(1);
X	}
X
X	strcpy(s, str);
X	return(s);
X}
X
X/*
X * byebye - exit.
X */
Xbyebye()
X{
X	register char c;
X	register int x, y;
X
X	/*
X	 * If the database is modified, see if they really
X	 * mean to exit without saving.
X	 */
X	if (dbmodified) {
X		getyx(curscr, y, x);
X		c = prompt_char(y, 0,
X				"Really exit without saving? ",
X				"YyNn");
X
X		if ((c == 'n') || (c == 'N'))
X			return;
X	}
X
X	/*
X	 * Reset tty modes and exit.
X	 */
X	reset_modes();
X	exit(0);
X}
X
X/*
X * usage - print a usage message.
X */
Xusage()
X{
X	fprintf(stderr, "Usage: %s [-f filter] [-i] [database] [pattern]\n",
X		pname);
X	exit(1);
X}
END_OF_FILE
if test 2490 -ne `wc -c <'util.c'`; then
    echo shar: \"'util.c'\" unpacked with wrong size!
fi
# end of 'util.c'
fi
echo shar: End of archive 1 \(of 2\).
cp /dev/null ark1isdone
MISSING=""
for I in 1 2 ; do
    if test ! -f ark${I}isdone ; then
	MISSING="${MISSING} ${I}"
    fi
done
if test "${MISSING}" = "" ; then
    echo You have unpacked both archives.
    rm -f ark[1-9]isdone
else
    echo You still need to unpack the following archives:
    echo "        " ${MISSING}
fi
##  End of shell archive.
exit 0
