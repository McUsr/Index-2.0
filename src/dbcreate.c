/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4
 *
 * dbcreate.c - handle creating a new database.
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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include "common.h"
#include "defs.h"
#include "util.h"
#include "dbfunc.h"
#include "initcurses.h"
#include "screen.h"
#include "dbfilename.h"
#include "dbcreate.h"
/* The message we'll print to explain what's happening.         */
/* It is hard to convert this, as long as the string contains   */
/* A format.                                                    */
/* TODO: must rewrite so that "MACROS" are expanded when the locale specifics are read in. */
static struct message {
	const char *m_line;
	char *m_arg;
	/* Todo: trenger conversion til unicode chars */
} message[] = {
	{
	"You will now be placed into an editor so that you can create the\n",
		    0}, {
	"database description file.  This file will be used by the program\n",
		    0}, {
	"to prompt you for new items to be inserted into the database.\n",
		    0}, {
	"\n", 0}, {
	"Each line in this file is the name of a field.  It may be as long as\n",
		    0}, {
	"you want, and may contain spaces.  The order of the lines in the file\n",
		    0}, {
	"is the order you will be prompted for new information when inserting\n",
		    0}, {
	"into the database.  You may leave blank lines in the file; this allows\n",
		    0}, {
	"multiple-line entries for items such as addesses.  You may have a\n",
		    0}, {
	"total of %d lines in the file.\n", (char *)MAXDBLINES}, {
	"\n", 0}, {
	"By default, all lines in an entry will be examined when searching for\n",
		    0}, {
	"a pattern.  To make the program ignore a line, start that line with an\n",
		    0}, {
	"exclamation point (!).\n", 0}, {
	"\n", 0}, {
	"The database is always sorted into ASCII collating sequence based on\n",
		    0}, {
	"the contents of the first field.\n", 0}, {
	"\n", 0}, {
	"When you are finished, save the file and exit the editor.  You will\n",
		    0}, {
	"then be placed in the main menu, where you can select other operations\n",
		    0}, {
	"on the database.\n", 0}, {
	NULL, 0}
};

/*
 * create_db - execute an editor to allow the person to create the
 *         index definition file.

 * The rules for creating the name of the database file, and the provider
 * of the new name resides in the function newlabelFilename that resides in
 * newlabelFilename 
 */
void
create_db(void)
{
	int pid;

	char *editor;

	register int row;

	char *fname=NULL; 
    set_modes() ;
	/* Clear the screen and move to the top. */
	clear();
	move(0, 0);

	/* Print out the explanatory message.  */
	for (row = 0; message[row].m_line != NULL; row++)
		printw(message[row].m_line, message[row].m_arg);

	/* Give the user a chance to read it.  Wait till they
	 * type a carriage return before proceeding.  */
	prompt_char(++row, 0, "Type RETURN to continue: ", "\n\r");

	/* Use the editor the user prefers, or EDITOR if
	 * he doesn't have anything set.  */
	if ((editor = getenv("EDITOR")) == NULL)
		editor = EDITOR;
	/* get the label file name.  */
    fname = getFullLabelFileName() ;

	/* Go back to normal tty modes.  */
	reset_modes();

	/* Spawn a child process.  */
	if ((pid = fork()) < 0) {
        yerror(YFORK_PROC_ERR,"create_db",editor,YX_EXTERNAL_CAUSE ) ;
	}
	/* Execute the editor. */
	if (pid == 0) {
		reset_modes();
		execl(editor, editor, fname, NULL);
		perror(editor);
		exit(1);
	}
	/* Wait for the editor to finish.  */
	while (wait((int *)0) != pid) ;
    /* loose the reference to the filename */
    fname = NULL; 
	/* Set the tty modes up again.  */
	set_modes();
}
