#ifndef __DBFILENAME__
#define __DBFILENAME__
void 		set_fulldbdir( char *theDir ) ;
void 		collect_dbase_dirs(void) ;
char * 		get_dbase_dir(void ) ;
FILE * 		open_db(void) ;
int 		justCreated(void) ;
int 		labelFileExists(wchar_t * dbname) ;
char *		newLabelFilePath(void) ;
char * 		getFullLabelFileName(void) ;
char * 		makeBackupName( const char * originalname ) ;
char *		getDbBackupFileName(void) ;
char * 		getFullDbName(void) ;
int 		dbFileHasNoPath(void )  ;
FILE * 		open_label_file(void) ;
int 		shouldCreateMissingLabelFile(void) ;
void 		setLabelFileCreated(void) ;
int 		labelFileIsJustCreated(void ) ;
void 		set_dbfilter( char *filtername ) ;
int 		has_dbfilter(void)  ;
char * 		open_dbfilter(void) ;
int 		no_dbase_name(void) ;
void 		set_dbase_shortname( wchar_t *dbname ) ;
void 		release_filenames(void) ;
void 		set_dbase_name(char *dbasearg) ;
wchar_t *	get_dbase_name(void) ;
#endif
