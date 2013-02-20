/*
 * $Header: /u5/davy/progs/index/RCS/defs.h,v 1.1 89/08/09 11:06:09 davy Exp $
 *
 * defs.h - definitions for the index program.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 * $Log:	defs.h,v $
 * Revision 1.1  89/08/09  11:06:09  davy
 * Initial revision
 * 
 */
#define PATCHLEVEL	0		/* level of patches applied	*/

#define MAXDBFILES	64		/* max. no. of database files	*/
#define MAXDBLINES	16		/* max. no. of fields in dbase	*/

#define EDITOR		"/usr/ucb/vi"	/* editor to use when creating	*/

#define INDEXDIR	".index"	/* directory where stuff is	*/
#define DBFILE_SUFFIX	".db"		/* database file suffix		*/
#define FMTFILE_SUFFIX	".fmt"		/* format program suffix	*/
#define IDXFILE_SUFFIX	".idx"		/* index definition suffix	*/

/*
 * Values for db_flag.
 */
#define DB_VALID	0x01
#define DB_PRINT	0x02

/*
 * For 4.2 curses.
 */
#ifndef cbreak
#define cbreak() crmode()
#endif
#ifndef nocbreak
#define nocbreak() nocrmode()
#endif

/*
 * Usually defined in ttychars.h.
 */
#ifndef CTRL
#define CTRL(c)		('c' & 037)
#endif

/*
 * Structure to hold the contents of the index definition.
 */
struct idxfile {
	int	idx_maxlen;		/* longest field length		*/
	int	idx_nlines;		/* number of lines per entry	*/
	char	idx_search[MAXDBLINES];	/* non-zero if field searchable	*/
	char	*idx_lines[MAXDBLINES];	/* strings naming the fields	*/
};

/*
 * Structure to hold a database entry.
 */
struct dbfile {
	int	db_flag;		/* flag, see above		*/
	int	db_lens[MAXDBLINES];	/* lengths of line contents	*/
	char	*db_lines[MAXDBLINES];	/* lines in the entry		*/
};

extern char	*pname;			/* program name			*/

extern int	dbsize;			/* size of db array		*/
extern int	igncase;		/* non-zero if -i switch given	*/
extern int	verbose;		/* non-zero if -v switch given	*/
extern int	dbentries;		/* number of entries in db	*/
extern int	dbmodified;		/* non-zero if db needs saving	*/

extern char	dbasedir[];		/* path to the INDEXDIR		*/

extern struct	dbfile *db;		/* database entries array	*/
extern struct	idxfile idx;		/* index definition structure	*/

char		*savestr();
char		*select_db();

int		byebye();
int		dbsort();
