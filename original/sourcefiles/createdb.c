#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/createdb.c,v 1.1 89/08/09 11:06:21 davy Exp $";
#endif
/*
 * createdb.c - handle creating a new database.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	createdb.c,v $
 * Revision 1.1  89/08/09  11:06:21  davy
 * Initial revision
 * 
 */
#include <sys/param.h>
#include <curses.h>
#include <stdio.h>
#include "defs.h"

/*
 * The message we'll print to explain what's happening.
 */
static struct message {
	char	*m_line;
	char	*m_arg;
} message[] = {
  { "You will now be placed into an editor so that you can create the\n",
	0 },
  { "database description file.  This file will be used by the program\n",
	0 },
  { "to prompt you for new items to be inserted into the database.\n",
	0 },
  { "\n",
	0 },
  { "Each line in this file is the name of a field.  It may be as long as\n",
	0 },
  { "you want, and may contain spaces.  The order of the lines in the file\n",
	0 },
  { "is the order you will be prompted for new information when inserting\n",
	0 },
  { "into the database.  You may leave blank lines in the file; this allows\n",
	0 },
  { "multiple-line entries for items such as addesses.  You may have a\n",
	0 },
  { "total of %d lines in the file.\n",
	(char *) MAXDBLINES },
  { "\n",
	0 },
  { "By default, all lines in an entry will be examined when searching for\n",
	0 },
  { "a pattern.  To make the program ignore a line, start that line with an\n",
	0 },
  { "exclamation point (!).\n",
	0 },
  { "\n",
	0 },
  { "The database is always sorted into ASCII collating sequence based on\n",
	0 },
  { "the contents of the first field.\n",
	0 },
  { "\n",
	0 },
  { "When you are finished, save the file and exit the editor.  You will\n",
	0 },
  { "then be placed in the main menu, where you can select other operations\n",
	0 },
  { "on the database.\n",
	0 },
  { NULL, 0 }
};

/*
 * create_db - execute an editor to allow the person to create the
 *	       index definition file.
 */
create_db(dbname)
char *dbname;
{
	int pid;
	char *editor;
	char *getenv();
	register int row;
	char buf[BUFSIZ], fname[MAXPATHLEN];

	/*
	 * Clear the screen and move to the top.
	 */
	clear();
	move(0, 0);

	/*
	 * Print out the explanatory message.
	 */
	for (row=0; message[row].m_line != NULL; row++)
		printw(message[row].m_line, message[row].m_arg);

	/*
	 * Give the user a chance to read it.  Wait till they
	 * type a carriage return before proceeding.
	 */
	prompt_char(++row, 0, "Type RETURN to continue: ", "\n");

	/*
	 * Use the editor the user prefers, or EDITOR if
	 * he doesn't have anything set.
	 */
	if ((editor = getenv("EDITOR")) == NULL)
		editor = EDITOR;

	/*
	 * Construct the file name.
	 */
	sprintf(fname, "%s/%s%s", dbasedir, dbname, IDXFILE_SUFFIX);

	/*
	 * Go back to normal tty modes.
	 */
	reset_modes();

	/*
	 * Spawn a child process.
	 */
	if ((pid = fork()) < 0) {
		error("%s: cannot fork.\n", pname, 0, 0);
		exit(1);
	}

	/*
	 * Execute the editor.
	 */
	if (pid == 0) {
		execl(editor, editor, fname, 0);
		perror(editor);
		exit(1);
	}

	/*
	 * Wait for the editor to finish.
	 */
	while (wait((int *) 0) != pid)
		;

	/*
	 * Set the tty modes up again.
	 */
	set_modes();
}
