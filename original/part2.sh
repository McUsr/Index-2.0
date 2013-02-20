#! /bin/sh
# This is a shell archive.  Remove anything before this line, then unpack
# it by saving it into a file and typing "sh file".  To overwrite existing
# files, type "sh file -c".  You can also feed this as standard input via
# unshar, or by typing "sh <file", e.g..  If this archive is complete, you
# will see the following message at the end:
#		"End of archive 2 (of 2)."
# Contents:  screen.c
# Wrapped by rsalz@papaya.bbn.com on Tue Oct 24 12:09:02 1989
PATH=/bin:/usr/bin:/usr/ucb ; export PATH
if test -f 'screen.c' -a "${1}" != "-c" ; then 
  echo shar: Will not clobber existing file \"'screen.c'\"
else
echo shar: Extracting \"'screen.c'\" \(14727 characters\)
sed "s/^X//" >'screen.c' <<'END_OF_FILE'
X#ifndef lint
Xstatic char *RCSid = "$Header: /u5/davy/progs/index/RCS/screen.c,v 1.1 89/08/09 11:06:53 davy Exp $";
X#endif
X/*
X * screen.c - screen management routines.
X *
X * David A. Curry
X * Research Institute for Advanced Computer Science
X * Mail Stop 230-5
X * NASA Ames Research Center
X * Moffett Field, CA 94035
X * davy@riacs.edu
X *
X * $Log:	screen.c,v $
X * Revision 1.1  89/08/09  11:06:53  davy
X * Initial revision
X * 
X */
X#include <sys/param.h>
X#include <signal.h>
X#include <curses.h>
X#include <stdio.h>
X#include "defs.h"
X
Xstatic int	screen_inited = 0;	/* for use with set/reset modes	*/
X
X/*
X * Menu used by disp_entries routine.
X */
Xstatic char	*disp_menu[] = {
X	"<RET> = next entry                   \"d\" = delete this entry\n",
X	"\"-\" = previous entry                 \"e\" = edit this entry\n",
X	"\"q\" = return to main menu",
X	0
X};
X
X/*
X * prompt_str - prompt the user for a string.
X */
Xprompt_str(row, col, promptstr, answer)
Xchar *promptstr, *answer;
Xint row, col;
X{
X	char *line;
X	int len, col0;
X	register int c, i, j;
X
X	/*
X	 * Print the prompt at (row,col).
X	 */
X	mvaddstr(row, col, promptstr);
X	refresh();
X
X	/*
X	 * Calculate "column zero", which is at the right
X	 * end of the prompt.
X	 */
X	col += strlen(promptstr);
X	line = answer;
X	col0 = col;
X	len = 0;
X
X	/*
X	 * Read characters till we get what we want.  The user
X	 * is allowed basic EMACS-style line editing.
X	 */
X	while ((c = getchar()) != EOF) {
X		switch (c) {
X		case CTRL(a):			/* beginning of line	*/
X			col = col0;
X			break;
X		case CTRL(b):			/* back character	*/
X			if (col > col0)
X				col--;
X			break;
X		case CTRL(d):			/* delete character	*/
X			/*
X			 * If there's stuff in the string,
X			 * delete this character.
X			 */
X			if (len) {
X				/*
X				 * Calculate string position of
X				 * character to delete.
X				 */
X				i = col - col0;
X
X				/*
X				 * Shuffle the string "left" one
X				 * place.
X				 */
X				while (i < len) {
X					line[i] = line[i+1];
X					i++;
X				}
X
X				/*
X				 * Delete the character on the screen.
X				 */
X				len -= 1;
X				delch();
X			}
X
X			break;
X		case CTRL(e):			/* end of line		*/
X			col = col0 + len;
X			break;
X		case CTRL(f):			/* forward character	*/
X			if ((col - col0) < len)
X				col++;
X			break;
X		case CTRL(h):			/* backspace delete	*/
X		case '\177':
X			/*
X			 * If there's stuff in the string,
X			 * delete the character.
X			 */
X			if (len) {
X				/*
X				 * Calculate position in string of
X				 * character to be deleted.
X				 */
X				i = col - col0 - 1;
X
X				/*
X				 * Shuffle the string "left" one place.
X				 */
X				while (i < len) {
X					line[i] = line[i+1];
X					i++;
X				}
X
X				len -= 1;
X
X				/*
X				 * Delete the character on the screen.
X				 */
X				move(row, --col);
X				delch();
X			}
X			break;
X		case CTRL(k):			/* kill line		*/
X			/*
X			 * Clear the string.
X			 */
X			if (len) {
X				i = col - col0;
X
X				line[i] = '\0';
X				len = i;
X
X				clrtoeol();
X			}
X			break;
X		case CTRL(l):			/* redraw screen	*/
X			wrefresh(curscr);
X			break;
X		case '\n':			/* return the string	*/
X			line[len] = '\0';
X			return;
X		default:			/* regular character	*/
X			/*
X			 * If it's the user's line-kill character,
X			 * we'll accept that to kill the line too.
X			 */
X			if (c == _tty.sg_kill) {
X				move(row, col0);
X				clrtoeol();
X				col = col0;
X
X				*line = '\0';
X				len = 0;
X			}
X			else {
X				/*
X				 * If it's a printable character,
X				 * insert it into the string.
X				 */
X				if ((c >= ' ') && (c < '\177')) {
X					/*
X					 * Calculate position of character
X					 * in string.
X					 */
X					i = col - col0;
X
X					/*
X					 * If we have to, move the
X					 * string "right" one place
X					 * to insert the character.
X					 */
X					if (i < len) {
X						for (j = len; j >= i; j--)
X							line[j+1] = line[j];
X					}
X
X					line[i] = c;
X					len += 1;
X					col++;
X
X					/*
X					 * Insert the character on the
X					 * screen.
X					 */
X					insch(c);
X
X				}
X			}
X			break;
X		}
X
X		/*
X		 * Move the cursor.
X		 */
X		move(row, col);
X		refresh();
X	}
X}
X
X/*
X * prompt_char - prompt the user for a single character, which must
X *		 be one of the ones in the "valid" string.
X */
Xprompt_char(row, col, promptstr, valid)
Xchar *promptstr, *valid;
Xint row, col;
X{
X	char *index();
X	register int c;
X
X	/*
X	 * Print the prompt.
X	 */
X	mvaddstr(row, col, promptstr);
X	clrtoeol();
X	refresh();
X
X	/*
X	 * Read characters...
X	 */
X	while ((c = getchar()) != EOF) {
X		/*
X		 * If it's not a valid one, beep
X		 * and get another one.
X		 */
X		if (index(valid, c) == NULL) {
X			putc('\007', stdout);
X			fflush(stdout);
X			continue;
X		}
X
X		/*
X		 * Add the character to the
X		 * screen, and return it.
X		 */
X		addch(c);
X		refresh();
X
X		return(c);
X	}
X}
X
X/*
X * main_menu - we dispatch to database functions from here.
X */
Xmain_menu(dbname)
Xchar *dbname;
X{
X	register char c;
X
X	/*
X	 * Set tty modes.
X	 */
X	set_modes();
X
X	/*
X	 * Forever...
X	 */
X	for (;;) {
X		/*
X		 * Clear the screen.
X		 */
X		clear();
X
X		/*
X		 * Print the name of the database and number of
X		 * entries.
X		 */
X		mvprintw(0, 0, "Database: %s  (%d entries)", dbname,
X			 dbentries);
X
X		/*
X		 * Mention whether or not it's modified.
X		 */
X		if (dbmodified)
X			addstr("  [modified]");
X
X		/*
X		 * Print the choices.
X		 */
X		mvaddstr(3, 8, "a - Add new entry to database");
X		mvaddstr(4, 8, "f - Find entry in database");
X		mvaddstr(5, 8, "r - Read database entry by entry");
X		mvaddstr(6, 8, "s - Save modifications, do not exit");
X		mvaddstr(7, 8, "q - Save modifications, exit");
X		mvaddstr(8, 8, "x - Exit");
X
X		/*
X		 * Get one of the choices.
X		 */
X		c = prompt_char(10, 0, "Command: ", "afqrsx\014");
X
X		/*
X		 * Dispatch the choice to the proper
X		 * database function.
X		 */
X		switch (c) {
X		case 'a':
X			add_entry();
X			break;
X		case 'f':
X			find_entry();
X			break;
X		case 'q':
X			save_bye(dbname);
X			break;
X		case 'r':
X			read_db();
X			break;
X		case 's':
X			save_db(dbname);
X			break;
X		case 'x':
X			byebye();
X			break;
X		case CTRL(l):			/* redraw screen	*/
X			wrefresh(curscr);
X			break;
X		}
X	}
X}
X
X/*
X * disp_entries - display all database entries with DB_PRINT set, and
X *		  let the user choose what to do with the entry.
X */
Xdisp_entries()
X{
X	int first, save = -1;
X	register int c, i, j;
X
X	/*
X	 * Clear the screen.
X	 */
Xtop:	clear();
X
X	/*
X	 * Print the names of the fields.
X	 */
X	for (i = 0; i < idx.idx_nlines; i++) {
X		mvprintw(i, 0, "%s%s", idx.idx_lines[i],
X			 idx.idx_lines[i][0] ? ":" : "");
X	}
X
X	/*
X	 * Print the menu.
X	 */
X	for (i=0; disp_menu[i] != NULL; i++)
X		mvaddstr(idx.idx_nlines+i+2, 10, disp_menu[i]);
X
X	/*
X	 * Find the first printable entry, and save its
X	 * index in first.
X	 */
X	for (first=0; first < dbentries; first++) {
X		if (db[first].db_flag & DB_PRINT)
X			break;
X	}
X
X	/*
X	 * Set current entry to either the first one
X	 * or the saved one.
X	 */
X	if (save < 0)
X		i = first;
X	else
X		i = save;
X
X	/*
X	 * Until we run out of entries...
X	 */
X	while (i < dbentries) {
X		/*
X		 * Print the entry.
X		 */
X		for (j=0; j < idx.idx_nlines; j++) {
X			move(j, idx.idx_maxlen + 2);
X			clrtoeol();
X			addstr(db[i].db_lines[j]);
X		}
X
X		/*
X		 * Print current/total entry numbers.
X		 */
X		mvprintw(idx.idx_nlines+7, COLS-15, "%5d/%-5d", i+1,
X			 dbentries);
X
X		/*
X		 * See what they want to do with this entry.
X		 */
X		c = prompt_char(idx.idx_nlines+6, 0, "Command: ",
X				"-deq\n\014");
X
X		/*
X		 * Dispatch the command...
X		 */
X		switch (c) {
X		case '\n':			/* go to next entry	*/
X			/*
X			 * Save this one.
X			 */
X			save = i;
X
X			/*
X			 * Advance to next printable one.
X			 */
X			for (i++; i < dbentries; i++) {
X				if (db[i].db_flag & DB_PRINT)
X					break;
X			}
X
X			break;
X		case '-':			/* go to previous entry	*/
X			/*
X			 * Save this one.
X			 */
X			save = i;
X
X			/*
X			 * Hunt for the previous printable one.
X			 */
X			for (i--; i >= 0; i--) {
X				if (db[i].db_flag & DB_PRINT)
X					break;
X			}
X
X			if (i < 0)
X				i = first;
X
X			break;
X		case 'd':			/* delete entry		*/
X			/*
X			 * See if they really want to delete it.
X			 * If so, mark it as not valid, and
X			 * re-sort the database.
X			 *
X			 * Because of the save variable, next
X			 * time through we'll print the last
X			 * one they looked at.
X			 */
X			if (del_entry(&db[i])) {
X				db[i].db_flag &= ~(DB_VALID | DB_PRINT);
X				qsort(db, dbentries, sizeof(struct dbfile),
X				      dbsort);
X				dbmodified = 1;
X				dbentries--;
X				goto top;
X			}
X
X			save = i;
X			goto top;
X		case 'e':			/* edit entry		*/
X			/*
X			 * Let them edit the entry.
X			 */
X			if (edit_entry(&db[i], "modified"))
X				dbmodified = 1;
X
X			save = i;
X			goto top;
X		case 'q':			/* return to main menu	*/
X			return;
X		case '\014':			/* redraw screen	*/
X			break;
X		}
X	}
X}
X
X/*
X * edit_entry - allow the user to edit a database entry.
X */
Xedit_entry(entry, word)
Xstruct dbfile *entry;
Xchar *word;
X{
X	int *len;
X	int col0;
X	char *line;
X	char tbuf[64];
X	char *malloc();
X	struct dbfile tmp;
X	register int c, i, j, row, col;
X
X	/*
X	 * Figure out where "column zero" is, to the
X	 * right of the longest field name.
X	 */
X	col0 = idx.idx_maxlen + 2;
X
X	/*
X	 * Clear the screen.
X	 */
X	clear();
X
X	/*
X	 * Print the field names.
X	 */
X	for (row = 0; row < idx.idx_nlines; row++) {
X		mvprintw(row, 0, "%s%s", idx.idx_lines[row],
X			 idx.idx_lines[row][0] ? ":" : "");
X	}
X
X	/*
X	 * Allocate some space in a temporary entry, and copy
X	 * the entry to be edited into it.  This way they can
X	 * abort the edit.
X	 */
X	for (i=0; i < idx.idx_nlines; i++) {
X		/*
X		 * Allocate memory for this line.
X		 */
X		if ((tmp.db_lines[i] = malloc(BUFSIZ)) == NULL) {
X			error("%s: out of memory.\n", pname, 0, 0, 0);
X			exit(1);
X		}
X
X		/*
X		 * Save the length of the entry, zero the
X		 * memory.
X		 */
X		tmp.db_lens[i] = entry->db_lens[i];
X		bzero(tmp.db_lines[i], BUFSIZ);
X
X		/*
X		 * Copy and print the line from the entry.
X		 */
X		if (entry->db_lines[i]) {
X			strcpy(tmp.db_lines[i], entry->db_lines[i]);
X			mvaddstr(i, col0, entry->db_lines[i]);
X		}
X	}
X
X	col = col0;
X	row = 0;
X
X	move(row, col);
X	refresh();
X
X	/*
X	 * Now let them edit the entry.  We provide the basic EMACS-style
X	 * control characters.
X	 */
X	while ((c = getchar()) != EOF) {
X		/*
X		 * Get the current line and line length.
X		 */
X		line = tmp.db_lines[row];
X		len = &tmp.db_lens[row];
X
X		switch (c) {
X		case CTRL(a):			/* beginning of line	*/
X			col = col0;
X			break;
X		case CTRL(b):			/* back character	*/
X			if (col > col0)
X				col--;
X			break;
X		case CTRL(d):			/* delete character	*/
X			if (*len) {
X				/*
X				 * Calculate position of character
X				 * in string.
X				 */
X				i = col - col0;
X
X				/*
X				 * Shuffle the string to the "left".
X				 */
X				while (i < *len) {
X					line[i] = line[i+1];
X					i++;
X				}
X
X				*len -= 1;
X
X				/*
X				 * Delete the character on the screen.
X				 */
X				delch();
X			}
X
X			break;
X		case CTRL(e):			/* end of line		*/
X			col = col0 + *len;
X			break;
X		case CTRL(f):			/* forward character	*/
X			if ((col - col0) < *len)
X				col++;
X			break;
X		case CTRL(h):			/* backspace delete	*/
X		case '\177':
X			if (*len) {
X				/*
X				 * Calculate position of character to
X				 * delete.
X				 */
X				i = col - col0 - 1;
X
X				/*
X				 * Shuffle string "left".
X				 */
X				while (i < *len) {
X					line[i] = line[i+1];
X					i++;
X				}
X
X				*len -= 1;
X
X				/*
X				 * Delete the character from the screen.
X				 */
X				move(row, --col);
X				delch();
X			}
X			break;
X		case CTRL(k):			/* kill line		*/
X			/*
X			 * Kill the line.
X			 */
X			if (*len) {
X				i = col - col0;
X
X				line[i] = '\0';
X				*len = i;
X
X				clrtoeol();
X			}
X			break;
X		case CTRL(l):			/* redraw screen	*/
X			wrefresh(curscr);
X			break;
X		case CTRL(n):			/* next line		*/
X			/*
X			 * Wrap around to the top if necessary.
X			 */
X			if (++row >= idx.idx_nlines)
X				row = 0;
X
X			/*
X			 * If nothing in this column, move to
X			 * nearest non-empty column.
X			 */
X			if ((col - col0) > tmp.db_lens[row])
X				col = col0 + tmp.db_lens[row];
X			break;
X		case CTRL(p):			/* previous line	*/
X			/*
X			 * Wrap around if necessary.
X			 */
X			if (--row < 0)
X				row = idx.idx_nlines - 1;
X
X			/*
X			 * If nothing in this column, move to
X			 * nearest non-empty column.
X			 */
X			if ((col - col0) > tmp.db_lens[row])
X				col = col0 + tmp.db_lens[row];
X			break;
X		case CTRL([):			/* save entry		*/
X			/*
X			 * Prompt for whether to save the entry.
X			 */
X			sprintf(tbuf, "Save %s entry in database? ", word);
X			c = prompt_char(idx.idx_nlines+2, 0, tbuf, "YyNn\n");
X
X			/*
X			 * See what they said.
X			 */
X			switch (c) {
X			case '\n':		/* never mind		*/
X				move(idx.idx_nlines+2, 0);
X				clrtoeol();
X				break;
X			case 'Y':		/* save entry		*/
X			case 'y':
X				/*
X				 * Copy the temporary entry into the real
X				 * entry.
X				 */
X				for (i=0; i < idx.idx_nlines; i++) {
X					if (entry->db_lines[i])
X						free(entry->db_lines[i]);
X
X					entry->db_lines[i] = savestr(tmp.db_lines[i]);
X					entry->db_lens[i] = tmp.db_lens[i];
X					free(tmp.db_lines[i]);
X				}
X
X				return(1);
X			case 'N':		/* don't save entry	*/
X			case 'n':
X				/*
X				 * Free temporary memory.
X				 */
X				for (i=0; i < idx.idx_nlines; i++)
X					free(tmp.db_lines[i]);
X				return(0);
X			}
X			break;
X		case '\n':			/* go to next line	*/
X			/*
X			 * Wrap around if necessary.
X			 */
X			if (++row >= idx.idx_nlines)
X				row = 0;
X			col = col0;
X			break;
X		default:			/* something else	*/
X			/*
X			 * If it's the user's kill character, we'll
X			 * accept that to delete the line too.
X			 */
X			if (c == _tty.sg_kill) {
X				move(row, col0);
X				clrtoeol();
X				col = col0;
X
X				*line = '\0';
X				*len = 0;
X			}
X			else {
X				/*
X				 * If it's a printable character, insert
X				 * it into the string.
X				 */
X				if ((c >= ' ') && (c < '\177')) {
X					/*
X					 * Calculate character position.
X					 */
X					i = col - col0;
X
X					/*
X					 * If necessary, move the string
X					 * to the "right" to insert the
X					 * character.
X					 */
X					if (i < *len) {
X						for (j = *len; j >= i; j--)
X							line[j+1] = line[j];
X					}
X
X					line[i] = c;
X					*len += 1;
X					col++;
X
X					/*
X					 * Insert the character on the
X					 * screen.
X					 */
X					insch(c);
X				}
X			}
X			break;
X		}
X
X		/*
X		 * Move to the current row/column.
X		 */
X		move(row, col);
X		refresh();
X	}
X}
X
X/*
X * reset_modes - restore tty modes to their original state.
X */
Xreset_modes()
X{
X	/*
X	 * No need.
X	 */
X	if (!screen_inited)
X		return;
X
X	screen_inited = 0;
X
X	/*
X	 * Move to bottom of screen.
X	 */
X	move(LINES-1, 0);
X	refresh();
X
X	/*
X	 * Restore modes.
X	 */
X	nocbreak();
X	echo();
X
X	/*
X	 * Turn curses "off".
X	 */
X	endwin();
X
X	signal(SIGQUIT, SIG_DFL);
X	signal(SIGINT, SIG_DFL);
X}
X
X/*
X * set_modes - set tty modes to what we want.
X */
Xset_modes()
X{
X	/*
X	 * Already done.
X	 */
X	if (screen_inited)
X		return;
X
X	screen_inited = 1;
X
X	/*
X	 * Ignore signals.
X	 */
X	signal(SIGQUIT, SIG_IGN);
X	signal(SIGINT, SIG_IGN);
X
X	/*
X	 * Turn "curses" on, turn echo off, turn cbreak on.
X	 */
X	initscr();
X	noecho();
X	cbreak();
X}
X
END_OF_FILE
if test 14727 -ne `wc -c <'screen.c'`; then
    echo shar: \"'screen.c'\" unpacked with wrong size!
fi
# end of 'screen.c'
fi
echo shar: End of archive 2 \(of 2\).
cp /dev/null ark2isdone
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
