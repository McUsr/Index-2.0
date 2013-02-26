/* vim: nospell fenc=utf-8 ft=c et sw=4 ts=4 sts=4 
 *
 * screen.c - screen management routines. 
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
 *
 */
#include "common.h"
#include "unicodeToUtf8.h"
#include "initcurses.h"
#include "defs.h"
#include "util.h"
#include "dbio.h"
#include "screen.h"
#include "dbfilename.h"

/* TODO: comment your statics! */
static int ConvertCh(chtype source, cchar_t * target);

static int AddCh(chtype ch);
static wchar_t *wcs_dbdir( void )  ;
static wchar_t *wcs_dbfullname(void) ;
static wchar_t *wprgname = NULL ;
static wchar_t *entriesLine = NULL ;
static wchar_t *db_dir = NULL ;
static wchar_t *db_fullname = NULL ;
static wchar_t *labelline = NULL ;
static wchar_t *databaseline = NULL ;

#ifdef TESTING
int main(void)
{

	char *myLocale = setlocale(LC_ALL, "");

	char *selectedDB = NULL;

	printf("Hello World\n");
	wchar_t svar[256];

	char *meld = "Test åg ær av vide chørs";

	set_modes();
	prompt_str(4, 5, meld, svar);
	reset_modes();

	/* Set tty modes.                                           */
	reset_modes();
	return 0;
}

#endif
/* Thomas Dickey's test_addwchar.c ncurses test-suite.          */
#define TempBuffer(source_len, source_cast) \
    if (source != 0) { \
        size_t need = source_len + 1; \
        wchar_t have[2]; \
        int n = 0; \
 \
        if (need > temp_length) { \
            temp_length = need * 2; \
            temp_buffer = typeRealloc(cchar_t, temp_length, temp_buffer); \
        } \
        have[0] = 0; \
        have[1] = 0; \
        do { \
            have[0] = source_cast; \
            setcchar(&temp_buffer[n++], have, A_NORMAL, 0, NULL); \
        } while (have[0] != 0); \
    } else if (temp_buffer != 0) { \
        free(temp_buffer); \
        temp_buffer = 0; \
        temp_length = 0; \
    } \
    return temp_buffer;

/* stolen from Thomas Dickey's test_addwchar.c ncurses test-suite.  */
static int
ConvertCh(chtype source, cchar_t * target)
{
	wchar_t tmp_wchar[2];

	tmp_wchar[0] = source;
	tmp_wchar[1] = 0;
	if (setcchar(target, tmp_wchar, A_NORMAL, 0, NULL) == ERR) {
		beep();
		return FALSE;
	}
	return TRUE;
}

/* stolen from Thomas Dickey's test_addwchar.c ncurses test-suite.  */
static int
AddCh(chtype ch)
{
	int code;

	cchar_t tmp_cchar;

	if (ConvertCh(ch, &tmp_cchar)) {
		code = add_wch(&tmp_cchar);
	} else {
		code = addch((chtype) ch);
	}
	return code;
}

static int
InsWch(chtype ch)
{
	int code;

	cchar_t tmp_cchar;

	if (ConvertCh(ch, &tmp_cchar)) {
		code = ins_wch(&tmp_cchar);
	} else {
		code = insch((chtype) ch);
	}
	return code;
}

/* prompt the user for a string.                                */
/* TODO: all length calculations of the string must be rebuilt.
 *
 * BUG:
 *          Når jeg står på slutten av linjen så blir cursor pos
 *          justert mot venstre selv om jeg ikke sletter noe.
 *
 * */
void
prompt_str(int row, int col, const char *promptstr, wchar_t * answer)
{
	wchar_t *line = answer;

	int len, col0;

	/* struct sgttyb _tty ; */
	register int code, i, j;

	/* Converts  the promptstr to a  wide version.              */
	wchar_t *wpromptstr = mbstowcs_alloc(promptstr);

	if (wpromptstr == NULL) {
        yerror( YMBS_WCS_ERR,"prompt_str->mbstowcs_alloc", "wpromptstr", YX_EXTERNAL_CAUSE ) ;
    }

	wchar_t ch;

	/* Print the wide prompt at (row,col).                          */
	mvaddwstr(row, col, wpromptstr);

	refresh();

	/* Calc "column zero", which is at right end of prompt.     */
	col += wcslen(wpromptstr);
	col0 = col;
	mvaddwstr(row, col, answer);
    len = wcslen(answer) ;
    col += len ; 
	/* Read chars till we get what we want. Useris allowed      */
	/* basic EMACS-style line editing.                          */
	while ((code = get_wch(&ch)) != EOF) {
		switch (ch) {
		case CTRL('a'):	/* beginning of line            */
			col = col0;
			break;
		case KEY_LEFT:
		case CTRL('b'):	/* back character               */
			if (col > col0)
				col--;
			break;
		case CTRL('d'):	/* delete character             */
			/*
			 * If there's stuff in the string,
			 * delete this character.
			 */
			if (col == (col0 + (int)wcslen(line))) {
				col--;
			} else if (len) {
				/* Calc string pos of char to delete.           */
				i = col - col0;

				/* Shuffle the string "left" one place.         */
				while (i < len) {
					line[i] = line[i + 1];
					i++;
				}

				/* Delete char on the screen.                   */
				len -= 1;
				delch();	/* prob ok that isn't wide. */
                if (col== (col0 + (int)wcslen(line)) ) {
                    --col ;
                }
			}

			break;
		case CTRL('e'):	/* end of line                  */
			col = col0 + len;
			break;
		case KEY_RIGHT:
		case CTRL('f'):	/* forward character            */
			if ((col - col0) < len)
				col++;
			break;
		case KEY_BACKSPACE:
		case CTRL('h'):	/* backspace delete */
		case '\177':
			/* If stuff in the string, delete char.             */
			if (len && ((col - 1) >= col0)) {
				/* Calc pos in string of char to delete         */
				int l = col - col0 - 1;
				if (l < 0)
					break;
				/* Shuffle the string "left" one place.         */
				while (l < len) {
					line[l] = line[l + 1];
					l++;
				}

				len -= 1;
				/* Delete the character on the screen.          */
				move(row, --col);
				delch();
			}
			break;
		case CTRL('k'):	/* kill line                    */
			/* Clear the string.                                */
			if (len) {
				i = col - col0;

				line[i] = '\0';
				len = i;

				clrtoeol();
			}
			break;
		case CTRL('l'):	/* redraw screen                */
			wrefresh(curscr);
			break;
		case KEY_ENTER:
		case '\r':	/* return the string            */
		case '\n':	/* return the string            */
			line[len] = '\0';
			return;
		default:	/* regular character            */
			if (ch == KEY_DL) {
				move(row, col0);
				clrtoeol();
				col = col0;

				*line = '\0';
				len = 0;
			} else if (code != KEY_CODE_YES) {
				if (iswctype(ch, wctype("print"))) {
					if (col == (COLS - 1)) {
						beep();
						break;
					}
					/* Calculate position of char in string.    */
					i = col - col0;

					/* If we have to, move string "right" one   */
					/* place to insert the character.           */
					if (i < len) {
						for (j = len; j >= i; j--)
							line[j + 1] = line[j];
					}
					line[i] = ch;
					len += 1;
					col++;

					/* Insert the character on the screen.      */
					InsWch((chtype) ch);
					/* ins_wch(&ch); */
				}
			}
			break;
		}

		/* Move the cursor.                                     */
		move(row, col);
		refresh();
	}
}

/* prompt the user for a single character, which must be  in    */
/* "valid" string.                                              */
wchar_t
prompt_char(int row, int col, const char *promptstr, const char *valid)
{
	wchar_t *w_prompt, *w_valid=NULL, ch;
   
	int code;

	w_prompt = mbstowcs_alloc(promptstr);

	/* if w_promptstr == NULL ?? .... */

	/* Print the prompt.                                        */
	mvaddwstr(row, col, w_prompt);
	clrtoeol();
	refresh();

    if ( valid  != NULL ) {
            w_valid  = mbstowcs_alloc(valid);
    }
	/* Read characters...                                       */
	while ((code = get_wch(&ch)) != ERR) {
		/* If it's not a valid one, beep and get another one.   */
		/* if (index(valid, c) == NULL) { */
        if ( valid  != NULL ) {
		    if (wcsrchr(w_valid, ch) == NULL) {	/* CHANGED !! */
			    beep();
			    continue;
		    }

		    /* Add the character to the screen, and return it.      */
		    AddCh((chtype) ch);
		    refresh();
        }
		goto _exit;
	}
 _exit:
	free(w_prompt);
	refresh();
    if (valid != NULL ) {
	    free(w_valid);
        w_valid= NULL ;
        return (ch);		/* to avoid compiler warning */
    } else {
        return '\0' ;
   }
}

/*
   returns the widechar path of the label file directory of the datebase
   that is active of one set by the INDEXDIR variable, or $HOME/.index
   (~/.index) really, since the directory of a database file specified
   with a path from the command line really doesn't count when it comes
   to finding label, or filter files.  One way to circumvent this is
   to set INDEXDIR "." move to the directory where the file exists, and
   execute index from there. -great for editing and testing filter files!

*/
#define MAXSCREENWIDTH 254
static wchar_t *
wcs_dbdir( void ) 
{
    if (db_dir == NULL ) { 
        db_dir = mbstowcs_alloc( get_dbase_dir() ) ;
     }  
     return db_dir ;
}

/*
    returns the widechar *fullname of the database in use, that is
    the filename with a full directory spec in front of it, with the
    adjoining .db suffix.
*/
static wchar_t *
wcs_dbfullname(void) 
{
    if (db_fullname == NULL ) {
        db_fullname = mbstowcs_alloc(getFullDbName() ) ;
    }
    return db_fullname ;
}
/*
    Updates the line specifying the number of entries in the database,
    when called from: main_menu, and read_database, edit_entry, add_entry,
    delete_entry, find_entry.
*/
void
initEntryLine(void ) {

	char tbuf[MAXSCREENWIDTH];
    
	sprintf(tbuf, " (%d entries)", dbentries);
    if (entriesLine != NULL ) {
        free(entriesLine ) ;
    }
	entriesLine = mbstowcs_alloc(tbuf);
}
/*
    initiates the heading that is to be printed, save the entries line
    This handler, along with paintheading is called upon from: main_menu,
    and read_database, edit_entry, add_entry, delete_entry, find_entry.

*/

void
initHeading(void)
{
	char tbuf[MAXSCREENWIDTH];
    if (wprgname == NULL ) {
        wprgname = mbstowcs_alloc(getProgramName() ) ;
    }
    if ((labelline==NULL) || (databaseline == NULL )) {
        strcpy(tbuf,"Label Directory: ") ;  /* 17 */
        labelline = mbstowcs_alloc(tbuf ) ;
        strcpy(tbuf,"Database Name: ") ;  /* 17 */
        databaseline = mbstowcs_alloc(tbuf ) ;
    }
    db_dir = wcs_dbdir() ;
    db_fullname = wcs_dbfullname() ;

}

/*
  paints the heading on the different "screens" with useful data
  is called upon from: main_menu, and read_database, edit_entry,
  add_entry, delete_entry, find_entry.

*/
void
paintHeading(const char *modeword) 
{ 
    wchar_t *dbmodistr = NULL ;
    move(0,0) ;
    clrtoeol() ;
	addwstr(wprgname);
    move(0,15) ; 
    if (modeword != NULL ) {
        dbmodistr = mbstowcs_alloc(modeword);
	    addwstr(dbmodistr);
        move(0,40);
    }
    addwstr(entriesLine) ;
    mvaddwstr(1,0,labelline) ;
	mvaddwstr(1,17, db_dir);  
    mvaddwstr(2,0,databaseline) ;
	mvaddwstr(2, 17, db_fullname);  
    if (dbmodistr != NULL ) {
        free(dbmodistr) ;
        dbmodistr = NULL ;
        
    }
}
/*
 * TODO: a.) Deklarere en db datatype basert på wide-characters.
 *       b.) kopiere inn og ut av datastrukturene.
 * */
 /*
    Returns whether we should save our changes or not.

    Edits an existing an entry. This routine is far to big, and handles
    everything, from movement between fields, and editing fields, even
    painting the screen.

 */
int
edit_entry(dbrecord * entry, const char *operationDesc, const char *entryDesc)
{
	int *len;
	int col0;
	wchar_t *line=NULL;
	char tbuf[MAXSCREENWIDTH];
	int code = 0;
	wchar_t ch;
	dbbuffer tmp;
	register int i, j, row, col;
    initHeading() ;
	/* Where is "column zero"? To right of longest field name.  */
	col0 = idx.idx_maxlen + 2;

	 clear();		/* Clear the screen.                            */
    initEntryLine() ;
    paintHeading(operationDesc) ;
	/* get max col TODO: change this when sigwhinch */
    /* first time: allocat wchar fulldbdir name
       fulldbdir is a static.*/
	for (row = STARTROW; row < (idx.idx_nlines+STARTROW); row++) { 
		/* print field names.                                   */
		mvaddwstr(row, 0, idx.idx_lines[row-STARTROW]);
	}
	/* Allocate some space in a temporary entry, and copy entry */
	/* to be edited into it.  This way they can abort the edit. */
	/* Here we need to allocate some extra space so we can edit */

	for (i = STARTROW; i < (idx.idx_nlines+STARTROW); i++) {
        int k = i-STARTROW ;
		if (entry->db_lens[k] == 0) {
			/* Allocate memory for this line.                   */
            size_t linelen = (MAXSCREENWIDTH * sizeof(wchar_t));
			tmp.db_lines[k] =
                 (wchar_t *) ymalloc(linelen,
                    "edit_entry","tmp.db_lines[k]" );
            memset(tmp.db_lines[k],0,linelen);
			tmp.db_lens[k] = 0;
		} else {
			/* Copy and print the line from the entry.          */
			tmp.db_lines[k] =
			    wcsFromUnicode_alloc((size_t *) & tmp.db_lens[k],
						 entry->db_lines[k],
						 (int32_t) entry->db_lens[k]);
			if (tmp.db_lines[k] == NULL) {
                yerror( YICU_CONV_ERR ,"edit_entry->wcsFromUnicode_alloc", "tmp.db_lines[k]", YX_EXTERNAL_CAUSE ) ;
			}
			/* reallocates more space to maximum linebuffer size. */
			tmp.db_lines[k] =
			    (wchar_t *) yrealloc(tmp.db_lines[k], (size_t)
						(MAXSCREENWIDTH * sizeof(wchar_t)),"edit_entry","tmp.db_lines[k]");
		}

		move(i, col0);
		clrtoeol();
		if (tmp.db_lens[k] > 0) {
			addwstr(tmp.db_lines[k]);
		}
	}			/* *could* have factored out the index code.              */
	col = col0;
	row = STARTROW;		/* row er hvilke rad i recorden (felt). */

	move(row, col);
	refresh();
	/* Editing entry. We provide basic EMACS-style cntrl chars. */
	while ((code = get_wch(&ch)) != EOF) {
		/* Get the current line and line length.                */
		line = tmp.db_lines[row-STARTROW];
		/* f.p. *len = &tmp.db_lens[row]; */
		len = &tmp.db_lens[row-STARTROW];
		switch (ch) {
		case CTRL('a'):	/* beginning of line            */
			col = col0;
			break;
		case KEY_LEFT:
		case CTRL('b'):	/* back character               */
			if (col > col0)
				col--;
			break;
		case CTRL('d'):	/* delete character             */
			if (col == (col0 + (int)wcslen(line))) {
				col--;
			} else if (*len) {
				/* Calculate position of character in string.   */
				int l = col - col0;

				/* Shuffle the string to the "left".            */
				while (l < *len) {
					line[l] = line[l + 1];
					l++;
				}
				*len -= 1;
				/* Delete the character on the screen.          */
				delch();
                if (col== (col0 + (int)wcslen(line)) ) {
                    --col ;
                }
			}

			break;
		case CTRL('e'):	/* end of line                  */
			col = col0 + *len;
			break;
		case KEY_RIGHT:
		case CTRL('f'):	/* forward character            */
			if ((col - col0) < *len)
				col++;
			break;
		case KEY_BACKSPACE:
		case CTRL('h'):	/* backspace delete             */
		case '\177':
			if (*len && ((col - 1) >= col0)) {
				/* Calculate position of character to delete.   */
				int l = col - col0 - 1;
				if (l < 0)
					break;
				/* Shuffle string "left".                        */
				while (l < *len) {
					line[l] = line[l + 1];
					l++;
				}

				*len -= 1;

				/* Delete the character from the screen.        */
				move(row, --col);
				delch();
			}
			break;
		case CTRL('k'):	/* kill line                    */
			if (len) {
                
			    int l = col - col0;

				line[l] = (wchar_t) '\0';
				*len = l;

				clrtoeol();
			}
			break;
		case CTRL('l'):	/* redraw screen                */
			wrefresh(curscr);
			break;
		case KEY_DOWN:
		case CTRL('n'):	/* next line                    */
			/* Wrap around to the top if necessary.             */
			if (++row >= (idx.idx_nlines+STARTROW))
				row = STARTROW; 
			/* If nothing in this column, move to nearest       */
			/* non-empty column.                                */
			if ((col - col0) > tmp.db_lens[row-STARTROW])
				col = col0 + tmp.db_lens[row-STARTROW];
			line[*len] = (wchar_t) '\0';
			break;
		case KEY_UP:
		case CTRL('p'):	/* previous line                */
			/* Wrap around if necessary.                        */
			if (--row < STARTROW)
				row = (idx.idx_nlines+STARTROW) - 1;

			/* If nothing in this column, move to nearest       */
			/* on-empty column.                                 */
			if ((col - col0) > tmp.db_lens[row-STARTROW])
				col = col0 + tmp.db_lens[row-STARTROW];
			line[*len] = (wchar_t) '\0';
			break;
		case CTRL('['):	/* save entry:  ESC or something...  */
			if (line[*len] != (wchar_t) '\0')
				line[*len] = (wchar_t) '\0';
			sprintf(tbuf, "Save %s entry in database (y/n)? ", entryDesc);
			ch = prompt_char(idx.idx_nlines + 2+ STARTROW, 0, tbuf, "yYnN");

			/* See what they said.                              */
			switch (ch) {
			case '\n':	/* never mind                       */
				move(idx.idx_nlines + 2, 0);
				clrtoeol();
				break;
			case 'Y':	/* save entry                       */
			case 'y':
				/* Copy the temporary entry into the real entry. */
				/* if there isn't anything to copy, then the entry db gets the value
				   NULL, and length 0 */
				for (i = 0; i < idx.idx_nlines; i++) {

					/* remove old contents in entry             */
					if (entry->db_lens[i] > 0) {
						free(entry->db_lines[i]);
						entry->db_lines[i] = NULL;
						entry->db_lens[i] = 0;
					} 
                    
                    if (tmp.db_lens[i] > 0) {
                        entry->db_lens[i]=tmp.db_lens[i] ;
						entry->db_lines[i] =
						    unicodeFromWcs_alloc((size_t *) &entry->db_lens[i],tmp.db_lines[i]);

					    if (entry->db_lines[i] == NULL) {
                            yerror( YICU_CONV_ERR ,"edit_entry->unicodeFromWcs_alloc", "entry->db_lines[i]", YX_EXTERNAL_CAUSE ) ;
                        }
                    } /* had a dangling else bug here ? */
					free(tmp.db_lines[i]);
                    tmp.db_lines[i] = NULL ;
				    tmp.db_lens[i] = 0;
				}
				return (1);
			case 'N':	/* don't save entry                 */
			case 'n':
				/* Free temporary memory.                       */
				for (i = 0; i < idx.idx_nlines; i++) {
					tmp.db_lens[i] = 0;
					free(tmp.db_lines[i]);
					tmp.db_lines[i] = NULL;
				}
				return (0);
			}
			break;
		case '\r':	/* return the string            */
		case '\n':	/* go to next line                  */
			/* Wrap around if necessary.                        */
			if (++row >= (idx.idx_nlines+STARTROW))
				row = STARTROW;
			col = col0;
			break;
		default:	/* something else                   */
			/* User's kill character: accepted to del line too. */
			if (ch == KEY_DL) {
				move(row, col0);
				clrtoeol();
				col = col0;

				*line = (wchar_t) '\0';
				*len = 0;
			} else if (code != KEY_CODE_YES) {
				/* If it's a printable character, insert it into */
				/* the string.                                  */
				if (iswctype(ch, wctype("print"))) {
					if (col == (COLS - 1)) {
						beep();
						break;
					}
					/* Calculate character position.            */
					i = col - col0;

					/* If necessary, move string * to "right"   */
					/* to insert the character.                 */
					if (i < *len) {
						for (j = *len; j >= i; j--)
							line[j + 1] = line[j];
					}

					line[i] = ch;
					*len += 1;
					col++;

					/* Insert the character on the screen.       */
					InsWch((chtype) ch);

				}
			}
			break;
		}

		/* Move to the current row/column.                       */
		move(row, col);
		refresh();
	}
	return (0);
}

int
byebye(void)
{
    static const char exitPrompt[]="Really exit without saving? (y/n) ";
	register char c;

	register int x, y;

	/* If db is modified, then see if user really wants to  */
	/* quit without saving.                                 */
	if (dbmodified) {
		getyx(curscr, y, x);
		c = prompt_char(y, 0,exitPrompt , "YyNn");
		if ((c == 'n') || (c == 'N'))
			return 0;
		else
			return 1;
	}
	return 1;		/* Silently exit as dbase wasn't modified    */
	/* reset_modes(); exit(0); takes place in finish()/main() */

}
