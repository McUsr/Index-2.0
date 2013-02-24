/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4:ro
 * initcurses: Own private little library to work with curses faster. */
/* We don't install signal handlers here anymore.
 * We do remove them, if we had them though.
 * Version 2.0
 * Updated/Ported to Mac Os X 10.6.8 Snow Leopard and onwards.
 * This version acknowledges Mac OsX's unicode filenames,
 * Utf-8, and sorting with respect to locale.
 * Tommy Bollman/McUsr 2013.02.06
 */
#include <common.h>
#include <initcurses.h>

/* constants for writing to error-messages to  the screen correctly */
#define SCR_INACTIVE 0x0
#define SCR_INITED 0x01 
#define SCR_DRAWING 0x02
#define SCR_HEADING_PAINTED 0x04
#define SCR_PENDING 0x08 
static int screen_status = 0;	/* for use with set/reset modes */

int
curses_active(void) 
{
    if ( screen_status & SCR_INITED ) {
        return 1 ;
    } else {
        return 0 ;
    }
}
/* restore tty modes to their original state.                   */
void reset_modes(void)
{
	/* No need.                                                 */
	if (screen_status == 0 )
		return;

	screen_status = 0;

	/* Move to bottom of screen.                                */
	move(LINES - 1, 0);
	refresh();
	/* TODO: prolly need to check initial values, and save them. */
	/* intrflush(stdscr, FALSE); */
	keypad(stdscr, TRUE);

	/* Restore modes.                                           */
	nocbreak();
	echo();
	/* noraw() ; */

	/* Turn curses "off".                                       */
	endwin();
/* TODO: investigate if have to to this after reset modes.*/
	/* signal(SIGQUIT, SIG_DFL);                                */
	/* signal(SIGINT, SIG_DFL);                                 */
}

/* TODO: dette er avvik fra den andre signal handleren jeg har. */
/* set tty modes to what we want.                               */
void set_modes(void)
{
	/* Already done.                                            */
	if (screen_status)
		return;

	screen_status |= SCR_INITED;

	/* Ignore signals. signalhandling moved into main.          */
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, SIG_IGN);

	/* Turn "curses" on, turn echo off, turn cbreak on.         */
	(void)initscr();
	/* intrflush(stdscr, FALSE); */
	keypad(stdscr, TRUE);
	(void)nonl();
	(void)noecho();		/* don't echo input.                       */
	(void)cbreak();		/* input one char at a time, "raw" mode.   */
	/* raw() ; */
}
