#ifndef lint
static char *RCSid = "$Header: /u5/davy/progs/index/RCS/screen.c,v 1.1 89/08/09 11:06:53 davy Exp $";
#endif
/*
 * screen.c - screen management routines.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	screen.c,v $
 * Revision 1.1  89/08/09  11:06:53  davy
 * Initial revision
 * 
 */
#include <sys/param.h>
#include <signal.h>
#include <curses.h>
#include <stdio.h>
#include "defs.h"

static int	screen_inited = 0;	/* for use with set/reset modes	*/

/*
 * Menu used by disp_entries routine.
 */
static char	*disp_menu[] = {
	"<RET> = next entry                   \"d\" = delete this entry\n",
	"\"-\" = previous entry                 \"e\" = edit this entry\n",
	"\"q\" = return to main menu",
	0
};

/*
 * prompt_str - prompt the user for a string.
 */
prompt_str(row, col, promptstr, answer)
char *promptstr, *answer;
int row, col;
{
	char *line;
	int len, col0;
	register int c, i, j;

	/*
	 * Print the prompt at (row,col).
	 */
	mvaddstr(row, col, promptstr);
	refresh();

	/*
	 * Calculate "column zero", which is at the right
	 * end of the prompt.
	 */
	col += strlen(promptstr);
	line = answer;
	col0 = col;
	len = 0;

	/*
	 * Read characters till we get what we want.  The user
	 * is allowed basic EMACS-style line editing.
	 */
	while ((c = getchar()) != EOF) {
		switch (c) {
		case CTRL(a):			/* beginning of line	*/
			col = col0;
			break;
		case CTRL(b):			/* back character	*/
			if (col > col0)
				col--;
			break;
		case CTRL(d):			/* delete character	*/
			/*
			 * If there's stuff in the string,
			 * delete this character.
			 */
			if (len) {
				/*
				 * Calculate string position of
				 * character to delete.
				 */
				i = col - col0;

				/*
				 * Shuffle the string "left" one
				 * place.
				 */
				while (i < len) {
					line[i] = line[i+1];
					i++;
				}

				/*
				 * Delete the character on the screen.
				 */
				len -= 1;
				delch();
			}

			break;
		case CTRL(e):			/* end of line		*/
			col = col0 + len;
			break;
		case CTRL(f):			/* forward character	*/
			if ((col - col0) < len)
				col++;
			break;
		case CTRL(h):			/* backspace delete	*/
		case '\177':
			/*
			 * If there's stuff in the string,
			 * delete the character.
			 */
			if (len) {
				/*
				 * Calculate position in string of
				 * character to be deleted.
				 */
				i = col - col0 - 1;

				/*
				 * Shuffle the string "left" one place.
				 */
				while (i < len) {
					line[i] = line[i+1];
					i++;
				}

				len -= 1;

				/*
				 * Delete the character on the screen.
				 */
				move(row, --col);
				delch();
			}
			break;
		case CTRL(k):			/* kill line		*/
			/*
			 * Clear the string.
			 */
			if (len) {
				i = col - col0;

				line[i] = '\0';
				len = i;

				clrtoeol();
			}
			break;
		case CTRL(l):			/* redraw screen	*/
			wrefresh(curscr);
			break;
		case '\n':			/* return the string	*/
			line[len] = '\0';
			return;
		default:			/* regular character	*/
			/*
			 * If it's the user's line-kill character,
			 * we'll accept that to kill the line too.
			 */
			if (c == _tty.sg_kill) {
				move(row, col0);
				clrtoeol();
				col = col0;

				*line = '\0';
				len = 0;
			}
			else {
				/*
				 * If it's a printable character,
				 * insert it into the string.
				 */
				if ((c >= ' ') && (c < '\177')) {
					/*
					 * Calculate position of character
					 * in string.
					 */
					i = col - col0;

					/*
					 * If we have to, move the
					 * string "right" one place
					 * to insert the character.
					 */
					if (i < len) {
						for (j = len; j >= i; j--)
							line[j+1] = line[j];
					}

					line[i] = c;
					len += 1;
					col++;

					/*
					 * Insert the character on the
					 * screen.
					 */
					insch(c);

				}
			}
			break;
		}

		/*
		 * Move the cursor.
		 */
		move(row, col);
		refresh();
	}
}

/*
 * prompt_char - prompt the user for a single character, which must
 *		 be one of the ones in the "valid" string.
 */
prompt_char(row, col, promptstr, valid)
char *promptstr, *valid;
int row, col;
{
	char *index();
	register int c;

	/*
	 * Print the prompt.
	 */
	mvaddstr(row, col, promptstr);
	clrtoeol();
	refresh();

	/*
	 * Read characters...
	 */
	while ((c = getchar()) != EOF) {
		/*
		 * If it's not a valid one, beep
		 * and get another one.
		 */
		if (index(valid, c) == NULL) {
			putc('\007', stdout);
			fflush(stdout);
			continue;
		}

		/*
		 * Add the character to the
		 * screen, and return it.
		 */
		addch(c);
		refresh();

		return(c);
	}
}

/*
 * main_menu - we dispatch to database functions from here.
 */
main_menu(dbname)
char *dbname;
{
	register char c;

	/*
	 * Set tty modes.
	 */
	set_modes();

	/*
	 * Forever...
	 */
	for (;;) {
		/*
		 * Clear the screen.
		 */
		clear();

		/*
		 * Print the name of the database and number of
		 * entries.
		 */
		mvprintw(0, 0, "Database: %s  (%d entries)", dbname,
			 dbentries);

		/*
		 * Mention whether or not it's modified.
		 */
		if (dbmodified)
			addstr("  [modified]");

		/*
		 * Print the choices.
		 */
		mvaddstr(3, 8, "a - Add new entry to database");
		mvaddstr(4, 8, "f - Find entry in database");
		mvaddstr(5, 8, "r - Read database entry by entry");
		mvaddstr(6, 8, "s - Save modifications, do not exit");
		mvaddstr(7, 8, "q - Save modifications, exit");
		mvaddstr(8, 8, "x - Exit");

		/*
		 * Get one of the choices.
		 */
		c = prompt_char(10, 0, "Command: ", "afqrsx\014");

		/*
		 * Dispatch the choice to the proper
		 * database function.
		 */
		switch (c) {
		case 'a':
			add_entry();
			break;
		case 'f':
			find_entry();
			break;
		case 'q':
			save_bye(dbname);
			break;
		case 'r':
			read_db();
			break;
		case 's':
			save_db(dbname);
			break;
		case 'x':
			byebye();
			break;
		case CTRL(l):			/* redraw screen	*/
			wrefresh(curscr);
			break;
		}
	}
}

/*
 * disp_entries - display all database entries with DB_PRINT set, and
 *		  let the user choose what to do with the entry.
 */
disp_entries()
{
	int first, save = -1;
	register int c, i, j;

	/*
	 * Clear the screen.
	 */
top:	clear();

	/*
	 * Print the names of the fields.
	 */
	for (i = 0; i < idx.idx_nlines; i++) {
		mvprintw(i, 0, "%s%s", idx.idx_lines[i],
			 idx.idx_lines[i][0] ? ":" : "");
	}

	/*
	 * Print the menu.
	 */
	for (i=0; disp_menu[i] != NULL; i++)
		mvaddstr(idx.idx_nlines+i+2, 10, disp_menu[i]);

	/*
	 * Find the first printable entry, and save its
	 * index in first.
	 */
	for (first=0; first < dbentries; first++) {
		if (db[first].db_flag & DB_PRINT)
			break;
	}

	/*
	 * Set current entry to either the first one
	 * or the saved one.
	 */
	if (save < 0)
		i = first;
	else
		i = save;

	/*
	 * Until we run out of entries...
	 */
	while (i < dbentries) {
		/*
		 * Print the entry.
		 */
		for (j=0; j < idx.idx_nlines; j++) {
			move(j, idx.idx_maxlen + 2);
			clrtoeol();
			addstr(db[i].db_lines[j]);
		}

		/*
		 * Print current/total entry numbers.
		 */
		mvprintw(idx.idx_nlines+7, COLS-15, "%5d/%-5d", i+1,
			 dbentries);

		/*
		 * See what they want to do with this entry.
		 */
		c = prompt_char(idx.idx_nlines+6, 0, "Command: ",
				"-deq\n\014");

		/*
		 * Dispatch the command...
		 */
		switch (c) {
		case '\n':			/* go to next entry	*/
			/*
			 * Save this one.
			 */
			save = i;

			/*
			 * Advance to next printable one.
			 */
			for (i++; i < dbentries; i++) {
				if (db[i].db_flag & DB_PRINT)
					break;
			}

			break;
		case '-':			/* go to previous entry	*/
			/*
			 * Save this one.
			 */
			save = i;

			/*
			 * Hunt for the previous printable one.
			 */
			for (i--; i >= 0; i--) {
				if (db[i].db_flag & DB_PRINT)
					break;
			}

			if (i < 0)
				i = first;

			break;
		case 'd':			/* delete entry		*/
			/*
			 * See if they really want to delete it.
			 * If so, mark it as not valid, and
			 * re-sort the database.
			 *
			 * Because of the save variable, next
			 * time through we'll print the last
			 * one they looked at.
			 */
			if (del_entry(&db[i])) {
				db[i].db_flag &= ~(DB_VALID | DB_PRINT);
				qsort(db, dbentries, sizeof(struct dbfile),
				      dbsort);
				dbmodified = 1;
				dbentries--;
				goto top;
			}

			save = i;
			goto top;
		case 'e':			/* edit entry		*/
			/*
			 * Let them edit the entry.
			 */
			if (edit_entry(&db[i], "modified"))
				dbmodified = 1;

			save = i;
			goto top;
		case 'q':			/* return to main menu	*/
			return;
		case '\014':			/* redraw screen	*/
			break;
		}
	}
}

/*
 * edit_entry - allow the user to edit a database entry.
 */
edit_entry(entry, word)
struct dbfile *entry;
char *word;
{
	int *len;
	int col0;
	char *line;
	char tbuf[64];
	char *malloc();
	struct dbfile tmp;
	register int c, i, j, row, col;

	/*
	 * Figure out where "column zero" is, to the
	 * right of the longest field name.
	 */
	col0 = idx.idx_maxlen + 2;

	/*
	 * Clear the screen.
	 */
	clear();

	/*
	 * Print the field names.
	 */
	for (row = 0; row < idx.idx_nlines; row++) {
		mvprintw(row, 0, "%s%s", idx.idx_lines[row],
			 idx.idx_lines[row][0] ? ":" : "");
	}

	/*
	 * Allocate some space in a temporary entry, and copy
	 * the entry to be edited into it.  This way they can
	 * abort the edit.
	 */
	for (i=0; i < idx.idx_nlines; i++) {
		/*
		 * Allocate memory for this line.
		 */
		if ((tmp.db_lines[i] = malloc(BUFSIZ)) == NULL) {
			error("%s: out of memory.\n", pname, 0, 0, 0);
			exit(1);
		}

		/*
		 * Save the length of the entry, zero the
		 * memory.
		 */
		tmp.db_lens[i] = entry->db_lens[i];
		bzero(tmp.db_lines[i], BUFSIZ);

		/*
		 * Copy and print the line from the entry.
		 */
		if (entry->db_lines[i]) {
			strcpy(tmp.db_lines[i], entry->db_lines[i]);
			mvaddstr(i, col0, entry->db_lines[i]);
		}
	}

	col = col0;
	row = 0;

	move(row, col);
	refresh();

	/*
	 * Now let them edit the entry.  We provide the basic EMACS-style
	 * control characters.
	 */
	while ((c = getchar()) != EOF) {
		/*
		 * Get the current line and line length.
		 */
		line = tmp.db_lines[row];
		len = &tmp.db_lens[row];

		switch (c) {
		case CTRL(a):			/* beginning of line	*/
			col = col0;
			break;
		case CTRL(b):			/* back character	*/
			if (col > col0)
				col--;
			break;
		case CTRL(d):			/* delete character	*/
			if (*len) {
				/*
				 * Calculate position of character
				 * in string.
				 */
				i = col - col0;

				/*
				 * Shuffle the string to the "left".
				 */
				while (i < *len) {
					line[i] = line[i+1];
					i++;
				}

				*len -= 1;

				/*
				 * Delete the character on the screen.
				 */
				delch();
			}

			break;
		case CTRL(e):			/* end of line		*/
			col = col0 + *len;
			break;
		case CTRL(f):			/* forward character	*/
			if ((col - col0) < *len)
				col++;
			break;
		case CTRL(h):			/* backspace delete	*/
		case '\177':
			if (*len) {
				/*
				 * Calculate position of character to
				 * delete.
				 */
				i = col - col0 - 1;

				/*
				 * Shuffle string "left".
				 */
				while (i < *len) {
					line[i] = line[i+1];
					i++;
				}

				*len -= 1;

				/*
				 * Delete the character from the screen.
				 */
				move(row, --col);
				delch();
			}
			break;
		case CTRL(k):			/* kill line		*/
			/*
			 * Kill the line.
			 */
			if (*len) {
				i = col - col0;

				line[i] = '\0';
				*len = i;

				clrtoeol();
			}
			break;
		case CTRL(l):			/* redraw screen	*/
			wrefresh(curscr);
			break;
		case CTRL(n):			/* next line		*/
			/*
			 * Wrap around to the top if necessary.
			 */
			if (++row >= idx.idx_nlines)
				row = 0;

			/*
			 * If nothing in this column, move to
			 * nearest non-empty column.
			 */
			if ((col - col0) > tmp.db_lens[row])
				col = col0 + tmp.db_lens[row];
			break;
		case CTRL(p):			/* previous line	*/
			/*
			 * Wrap around if necessary.
			 */
			if (--row < 0)
				row = idx.idx_nlines - 1;

			/*
			 * If nothing in this column, move to
			 * nearest non-empty column.
			 */
			if ((col - col0) > tmp.db_lens[row])
				col = col0 + tmp.db_lens[row];
			break;
		case CTRL([):			/* save entry		*/
			/*
			 * Prompt for whether to save the entry.
			 */
			sprintf(tbuf, "Save %s entry in database? ", word);
			c = prompt_char(idx.idx_nlines+2, 0, tbuf, "YyNn\n");

			/*
			 * See what they said.
			 */
			switch (c) {
			case '\n':		/* never mind		*/
				move(idx.idx_nlines+2, 0);
				clrtoeol();
				break;
			case 'Y':		/* save entry		*/
			case 'y':
				/*
				 * Copy the temporary entry into the real
				 * entry.
				 */
				for (i=0; i < idx.idx_nlines; i++) {
					if (entry->db_lines[i])
						free(entry->db_lines[i]);

					entry->db_lines[i] = savestr(tmp.db_lines[i]);
					entry->db_lens[i] = tmp.db_lens[i];
					free(tmp.db_lines[i]);
				}

				return(1);
			case 'N':		/* don't save entry	*/
			case 'n':
				/*
				 * Free temporary memory.
				 */
				for (i=0; i < idx.idx_nlines; i++)
					free(tmp.db_lines[i]);
				return(0);
			}
			break;
		case '\n':			/* go to next line	*/
			/*
			 * Wrap around if necessary.
			 */
			if (++row >= idx.idx_nlines)
				row = 0;
			col = col0;
			break;
		default:			/* something else	*/
			/*
			 * If it's the user's kill character, we'll
			 * accept that to delete the line too.
			 */
			if (c == _tty.sg_kill) {
				move(row, col0);
				clrtoeol();
				col = col0;

				*line = '\0';
				*len = 0;
			}
			else {
				/*
				 * If it's a printable character, insert
				 * it into the string.
				 */
				if ((c >= ' ') && (c < '\177')) {
					/*
					 * Calculate character position.
					 */
					i = col - col0;

					/*
					 * If necessary, move the string
					 * to the "right" to insert the
					 * character.
					 */
					if (i < *len) {
						for (j = *len; j >= i; j--)
							line[j+1] = line[j];
					}

					line[i] = c;
					*len += 1;
					col++;

					/*
					 * Insert the character on the
					 * screen.
					 */
					insch(c);
				}
			}
			break;
		}

		/*
		 * Move to the current row/column.
		 */
		move(row, col);
		refresh();
	}
}

/*
 * reset_modes - restore tty modes to their original state.
 */
reset_modes()
{
	/*
	 * No need.
	 */
	if (!screen_inited)
		return;

	screen_inited = 0;

	/*
	 * Move to bottom of screen.
	 */
	move(LINES-1, 0);
	refresh();

	/*
	 * Restore modes.
	 */
	nocbreak();
	echo();

	/*
	 * Turn curses "off".
	 */
	endwin();

	signal(SIGQUIT, SIG_DFL);
	signal(SIGINT, SIG_DFL);
}

/*
 * set_modes - set tty modes to what we want.
 */
set_modes()
{
	/*
	 * Already done.
	 */
	if (screen_inited)
		return;

	screen_inited = 1;

	/*
	 * Ignore signals.
	 */
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	/*
	 * Turn "curses" on, turn echo off, turn cbreak on.
	 */
	initscr();
	noecho();
	cbreak();
}

