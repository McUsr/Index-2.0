/* vim: nospell
 * * dbio.h - database input/output routines.
 *
 * David A. Curry
 * Research Institute for Advanced Computer Science
 * Mail Stop 230-5
 * NASA Ames Research Center
 * Moffett Field, CA 94035
 * davy@riacs.edu
 *
 */
#ifndef __DBIO_UTIL__
#define __DBIO_UTIL__

extern dbrecord *db;      /* array of database entries    */
extern idxfile idx; /* index definition structure   */

extern int      dbmodified;     /* non-zero if db needs saving  */
extern int      dbentries;      /* number of entries in db      */
extern int      dbsize;         /* size of db array             */
void 			set_reverse(void) ;
void 			read_labelfile(void) ;
int 			file_is_empty( char *filename) ;
void 			remove_unwanted_file( void ) ;
void            release_idx(void) ;
void            read_database(void) ;
void            save_db(void) ;
int             compare_legacy(const void **string1, const void **string2) ;
int 			reverse_dbsort(dbrecord * a, dbrecord * b) ;
int             dbsort( dbrecord *a, dbrecord *b) ;
void            release_db(void) ;
#endif

