/* vim: nospell
 * defs.h - definitions for the index program.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 * 
 */
#define PATCHLEVEL  0           /* level of patches applied     */

#define MAXDBFILES  64          /* max. no. of database files   */

#define EDITOR  (char *)    "/usr/bin/nano" /* editor for creating      */

#define INDEXDIR    ".index"    /* directory where stuff is     */
#define DBFILE_SUFFIX   ".db"   /* database file suffix         */
#define FMTFILE_SUFFIX  ".fmt"  /* format program suffix        */
#define IDXFILE_SUFFIX  ".idx"  /* index definition suffix      */

/*  Values for db_flag.                                         */
#define DB_VALID    0x01
#define DB_PRINT    0x02

#define LC_STRING_CAP 40

/* For 4.2 curses.                                              */
#ifndef cbreak
#define cbreak() crmode()
#endif
#ifndef nocbreak
#define nocbreak() nocrmode()
#endif

/* was defined in ttychars.h.                                   */
#undef CTRL
#define CTRL(x) ((x) & 0x1f)

/* defines for exit codes */

#define YX_ALL_WELL 0
#define YX_EXTERNAL_CAUSE 1
#define YX_NO_DATA 1
#define YX_USER_ERROR 2

/* constants that helps us find the correct error message in 
  the errstrings table defined in util.c */
#define YMALLOC_ERR 0
#define YREALLOC_ERR 1
#define YICU_CONV_ERR 2
#define YMBS_WCS_ERR 3
#define YPTH_TOOLONG_ERR 4
#define YPTH_ISROOT_ERR 5
#define YPTH_ISPWD_ERR 6
#define YPTH_NOPWD_ERR 7
#define YFILE_CREAT_ERR 8
#define YFILE_RENMV_ERR 9
#define YFILE_FINDF_ERR 10
#define YFILE_OREAD_ERR 11
#define YFILE_FREAD_ERR 12
#define YFILE_FWRITE_ERR 13
#define YICU_COLLATO_ERR 14
#define YICU_CRECOLL_ERR 15
#define YDIR_OPEN_ERR 16
#define YPIP_EXEC_ERR 17
#define YFORK_PROC_ERR 18
#define YICU_CNVFUTF8_ERR 19
#define YICU_CNVPREUTF8_ERR 20 
#define YICU_CNVTUTF8_ERR 21
#define YICU_CNVPRETUTF8_ERR 22 

#define STARTROW 4

#define MAXDBLINES  16      /* max. no. of fields in dbase      */
/*
 * Structure to hold the contents of the index definition.
 */

struct tagidxfile {
    int     idx_maxlen;             
            /* longest field length                             */
    int     idx_nlines;             
            /* number of lines per entry                        */
    wchar_t idx_search[MAXDBLINES]; 
            /* non-nil if can search field                      */
    wchar_t *idx_lines[MAXDBLINES]; 
            /* strings naming the fields                        */
} ;
typedef struct tagidxfile idxfile ;

struct tagdbrecord {
    int     db_flag;                /* flag, see above          */
    int     db_lens[MAXDBLINES];    /* lengths of line contents */
    UChar   *db_lines[MAXDBLINES];  /* lines in the entry       */
} ;
typedef struct tagdbrecord dbrecord ;

/* wchar version for a buffer when working on a full record.    */
struct tagdbbuffer {
    int     db_flag;                /* flag, see above          */
    int     db_lens[MAXDBLINES];    /* lengths of line contents */
    wchar_t *db_lines[MAXDBLINES];  /* lines in the entry       */

} ;
typedef struct tagdbbuffer dbbuffer ;
/*
 * Structure to hold a database entry.
 */

/* Det trace makroer i source, eller i koden til Eric.s Raymond */ 

void finish(int sig); /*from main.c */
