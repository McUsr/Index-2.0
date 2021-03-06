/* dbfilename.c - database filename routines 
this module is used in index ver 2.0 that 
is derived from index 1.0 by David A. Curry.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Purpose
=======

The whole reason for this module is to support
relative pathnames for databases on the commandline.

Business rules for finding and resolving paths
==============================================


TODO:
needs shortdbname. or something telling us what we got:

	if we have the full path, then we'll go directly there to find it.

    we'll also make a copy of the parent directory when we have the full path.


TODO:
		clean up code in dbselect, the variable used there, must be passed over to here
		this is for the situations when we choose to select or create a database.

		dbselect.c^set_dbase_name(char *) :	reprogrammeres?

		check over the utf8 routines, I think there were some issues somewhere.
TODO:
		flags and variables, releasing.
TODO:
		Update the comments further, factor out check basename.

 */
#if HAVE_CONFIG_H
# include <config.h>
#endif
#include "common.h"
#include "unicodeToUtf8.h"
#include "util.h" 
#include "defs.h"
#include "dbfilename.h"
 
static FILE * openFromDir(char *fname, char *filedir ) ;
static FILE * openLabelFileFromDir(char *fname, char *filedir ) ;
static char * makeAfilenameFromStem(const char *suffix, char *fname, char *filedir) ;
static char * makeAfilename( char *fname, char *filedir) ;
static char * tildeExpandedFileName(char *farg) ;
static char * baseNameCheck( const char* pathToCheck, const char *caller, const char *variable) ;
static char * extBasenameCheck (const char * pathToCheck, const char *caller, const char *variable) ;
 
#define F_ARGVNAME  0x01 
/*
For the case that we need to see that we need to make the stem file name.
amongst other things, as we can get only the stem from command line.
*/
#define F_GOTFNAME  0x02 
/*
so we know whether to select a file later
*/
   
#define F_FULL_PATH  0x04 
/*
implies that we have a directory supplied from the command-line. FIRST
LATER set when we have created a full path name.
*/

#define D_INDEXDIR  0x08 
/*
Set if we are using the index dir as a first stop for label files.
*/

#define D_HOMEDIR  0x10
/*
set if we find the file in the homedir
*/
#define F_LABEL_MADE  0x20 
#define F_HAS_FILTER  0x40 
#define L_FULL_PATH 0X80
/*
Set when we have collected the full path to the label file.
It is set immediately if the path is resolved from the command
line, then it is set immediately, if not it is set when the 
database is settled via the ui and a directory.
*/
#define F_BCK_NAME  0X100 
#define F_ARGV_DIR 0X200 

#define F_CREATED 0X400	
/*
set when we create a new dbase file with a name given  from command line.
*/
#define L_CREATED 0X800	
/*
gets set when we create a new index file.
*/

/* assumes int is 16 bits wide, we'll also use unsigned */
static unsigned int fn_status=0; 

static char * stemdbname = NULL ;
/* Will alwyas exist before we open a db.
   can be set from select db -- set_dbase_shortname()
 * but is set from main, -- set_dbase_name, if arg was given on command line. */
static char * basedbname = NULL ;
/* Same as stem, but with the suffix attached */

static wchar_t *shortdbname=NULL;
/* The shortdbname, is the one that we do write in the dbselect routine.
not sure if it should be here really, for technical reasons, but semantically
it really belongs here. This is the converted stem filename
maybe it doesn't belong here after all.
*/

static char * fulllabelname = NULL ;
/* this one is used, if we create a new label file. */

static char * fulldbname = NULL ;
/* May or may not exist, if, then it is set from main.c -- set_dbase_name() */

static char * bckdbname = NULL ;
/* May or may not exist, if, then it is set from main.c -- set_dbase_name() */

static char * fulldbdir = NULL ;
/*
May.. is then the first place to look for label and fmt files from set_dbase_name
called from main
this is set when a db name came with a path from the commandline.
*/

static	char *dbfilter=NULL;
/*
May or may not, this for when we have a filter we want to use.
*/

static char *indexdir_varpath=NULL;
/*
May .. this is the directory that is set by the INDEXDIR variable.
*/

static char * indexhomedir = NULL ;
/*
Always .. this is the path to the directory that is hardcoded: $HOME/.index
if it doesn't exist, then we do try to make it. If we can't, we die with an error message.
except for if we are root, then we refuse to make it, and dies with an error message.
*/

static char * pwddir = NULL ;
/* our working directory while we started up, obtained from $PWD in the shell */

/*
Returns a basename that is either a filename, or ".", otherwise dies.
It can't take NULL as a paramter.
Evaluates if the basename library function returns something that can be used.
Returning the root directory is under all circumstances something we can't use.
NULL, signigfying that the path is also too long is also something we can't use.
"." on the other hand, can be used as a value for setting the INDEXDIR variable
to the current directory, making the tool more flexible. It is intgreated with 
the errror messages system.
*/

static char *
baseNameCheck( const char* pathToCheck, const char *caller, const char *variable)
{
	char *basePtr= basename(pathToCheck) ;
   	if ( basePtr == NULL ) {
        yerror( YPTH_TOOLONG_ERR,caller,variable, YX_USER_ERROR ) ;
   	} else  if (!strcmp(basePtr,"/")) {
        yerror( YPTH_ISROOT_ERR,caller,variable, YX_USER_ERROR ) ;
   	} 
	return basePtr ;
}
/*
returns the path from the basename library call, if it isn't NULL, "/" or "."
*/
static char *
extBasenameCheck (const char * pathToCheck, const char *caller, const char *variable)
{
	char *extBasePtr=baseNameCheck(pathToCheck,caller,variable ) ;
     if (!strcmp(extBasePtr,".")) {
        yerror( YPTH_ISPWD_ERR,caller,variable, YX_USER_ERROR ) ;
	 }
	return extBasePtr ;
}

/*
Collects necessary directories during initialization of the program
TODO: USE PWD FOR "." HERE
*/
void
collect_dbase_dirs(void)
{
	const char procname[]="collect_dbase_dirs" ;
	const char indexdirname[]="INDEXDIR" ;
	const char homename[]="HOME" ;
	char *s = NULL;
	/* and finally, we gather the PWD */
	if ((s = getenv("PWD")) == NULL) { /* This *SHOULD* work in most situation */
		yerror(YPTH_NOPWD_ERR,procname,"$PWD",YX_EXTERNAL_CAUSE ) ;
	}
	size_t pwdlen = strlen(s) + 2 ;
	pwddir = (char *) ymalloc(pwdlen,procname,"pwddir") ; 
	strcpy(pwddir,s) ;
	strcat(pwddir,"/") ;
	char *tst_stem= NULL ;
	/* if defined, then this is where we are looking for db's */
	if ((s = getenv(indexdirname)) != NULL) {
    	tst_stem = baseNameCheck(s,procname,indexdirname) ;
   		if (!strcmp(tst_stem,".")) {
			tst_stem = pwddir ;  /*bluntly translates */
		}
		size_t tst_stem_len = strlen(tst_stem ) + 1 ;	
		indexdir_varpath = (char *) ymalloc(tst_stem_len,procname,"indexdir_varpath") ; 
		fn_status = (fn_status | D_INDEXDIR) ;
		/* But it has to resolve to something */
		strcpy(indexdir_varpath, s);
		tst_stem = NULL ;	
	}

	/* collects the home directory. */
	if ((s = getenv(homename)) == NULL) {
		yerror(YPTH_NOPWD_ERR,procname,homename,YX_EXTERNAL_CAUSE ) ;
	}
	size_t home_slen = strlen(s) +  strlen(INDEXDIR) + 2 ;
	
	indexhomedir = (char *) ymalloc(home_slen,procname,"indexhomedir") ; 
	fn_status |= D_HOMEDIR ;
	/* TODO: We shouldn't set D_HOMEDIR IF D_INDEXDIR IS SET ??? */
	/* Make name.                                           */
	strcpy(indexhomedir,s) ;
	strcat(indexhomedir,"/") ;
	strcat(indexhomedir,INDEXDIR) ;
   	s = NULL ;
}

/*
returns the directory where we should look for files. 
it is either the INDEXDIR directory, or the $HOME./index
directory if the former isn't set.
   
It will be used under all circumstances for finding label
files. It won't be used for the db-file, if the path to that 
file is set on the commandline. Then F_FULL_PATH is set.

If it is set to INDEXDIR, then $HOME/.index will be used 
as a fall back for the label file if the full path to the
database file is specified on the commandline.
   
It will always return one or the other of those two directories.
This directory governs the place to look for db files, label
files, and also where a new db file without a path is created.
 	
Those directories have been gathered up front with
collect_dbase_dirs().
*/
char *
get_dbase_dir(void )
{
	if (fn_status & D_INDEXDIR ) {
		return indexdir_varpath ;
	} else {
		return indexhomedir ; 
	}
}

/* 	Returns a filepointer, after we have tried to open the
   	dbfile, (open_db() is the only caller).
	If it finds the file.db then it saves the full name of it
	in the fulldbname, and sets the flag: F_FULL_PATH so we 
	know that we just can fetch it there, without further
	tracking.
*/
static FILE *
openFromDir(char *fname, char *filedir ) 
{
		FILE *fp = NULL ;
		char *aDatabaseFileName = makeAfilename(fname,filedir) ;
		if ((fp = fopen(aDatabaseFileName, "r")) != NULL )  {
			fn_status |= F_FULL_PATH ;
			fulldbname = aDatabaseFileName ;
		} else {
			free(aDatabaseFileName) ;
			aDatabaseFileName = NULL ;
		}
		return fp ;
}

/* Returns a filepointer as above, but this one gets called from
   open_label_file() it saves the full name of it when it finds it
   in fullabelname. for reuse. then it sets L_FULL_PATH to mark
   that we have gotten the full path to the label file.
*/
static FILE *
openLabelFileFromDir(char *fname, char *filedir ) 
{
		FILE *fp = NULL ;
		char *aLabelFileName = makeAfilename(fname,filedir) ;
		if ((fp = fopen(aLabelFileName, "r")) != NULL )  {
			fn_status |= L_FULL_PATH ;
			fulllabelname = aLabelFileName ;
		} else {
			free(aLabelFileName) ;
			aLabelFileName = NULL ;
		}
		return fp ;
}

/*
makes a filename from a suffix,basename and directory,
must free memory afterwards.
TODO: could this have been used more often???
*/
static char *
makeAfilenameFromStem(const char *suffix, char *fname, char *filedir)
{
	char * tmpfname = NULL ;
	int endingSlash = 0 ;
	/* make a path name of index dir and stemname that is suffixed */
	/* try it, if it works, return filepointer */
	size_t dirlen = strlen(filedir) ;
	if (filedir[dirlen-1] == '/' ) {
		endingSlash = 1; 
	}
	size_t fnlen = strlen(fname) ;
	size_t sufflen = strlen(suffix ) ;
	tmpfname = (char *) ymalloc((dirlen +fnlen + sufflen + 2 - endingSlash),
		"makeAfilenameFromStem","tmpfname") ; 
	strcpy(tmpfname,filedir) ;
	if (!endingSlash) {
		strcat(tmpfname,"/") ;
	}
	strcat(tmpfname,fname) ;
	strcat(tmpfname,suffix) ;
	return tmpfname ;
}
/*
makes a filename from a basename and directory,
must free memory afterwards.
*/
static char *
makeAfilename( char *fname, char *filedir)
{
	char * tmpfname = NULL ;
	int endingSlash = 0 ;
	/* make a path name of index dir and stemname that is suffixed */
	/* try it, if it works, return filepointer */
	size_t dirlen = strlen(filedir) ;
	if (filedir[dirlen-1] == '/' ) {
		endingSlash = 1; 
	}
	size_t fnlen = strlen(fname) + (dirlen  + 2 - endingSlash ) ;	;
	tmpfname = (char *) ymalloc(fnlen,"makeAfilename","tmpfname") ; 
	strcpy(tmpfname,filedir) ;
	if (!endingSlash) {
		strcat(tmpfname,"/") ;
	}
	strcat(tmpfname,fname) ;
	return tmpfname ;
}

/*
Returns a file pointer to the db to open , or dies trying to open it.
It will try to open it one or two places, according to the constituency
of the filename. Is the only user of openFromDir, which sets the fulldbname
as a side-effect. It is called only from read_db.

  TODO:
  Once we see that we can open the file, we'll set the directory accordingly.
  in open from dir I think. WHAT???

  so that we won't have to resolve it again.
*/
FILE *
open_db(void) 
{
	const char procname[]="open_db" ;
	FILE *fp = NULL ;
	char *tmpfname ;
	/* Decude how we got here, from selectdb or commandline */
	if (fn_status & F_FULL_PATH ) {
			/* Definately from the commandline when F_FULL_PATH
			is set WHEN we are called. We even made an index file
			for it if that didn't exist in open_label_file() before we came here. */
			/* try opening the file with fulldbname */
			/* try to open it, if it fails give error message and die. */
			if ( (fp = fopen(fulldbname,"r" ))  != NULL ) {
				return fp ;
			} else { /* it is a file that doesn't exist */
				if ((fp = fopen(fulldbname, "w")) == NULL) {
					yerror(YFILE_CREAT_ERR,procname,fulldbname,YX_EXTERNAL_CAUSE ) ;
				} else { 
					fn_status |= F_CREATED ;
					return fp ;
				}
			}
			
			/* We create the empty file in the read_label file thing
			that are called before this routine. so if it isn't here: ERR! */
	} else if (fn_status & L_CREATED ) {
		/* if an index file has been created, and we come here,
		then we are started up from the ui.( interesting fact.)
	
		It works this way: if the index file were first, then 
		the "new file process" was initiated from the ui.

		we could of course simply logic and create 
		file when we have created the index file in dbselect.

		I am not sure what is logically worst, but making it in
		the dbselect is kind of simpler logically.
		We are going for dbeselect first, but maybe getting back here.
		*/

		/* !! label file has been created and exists, make corresponding 
		   db file here. (but check if it for some reason exists first!
		 */
		/* we do have the stem, the base and the shortdb name when we arrive here. */

		/* just a precaution may have made a copy of a file, and renamed it. */
		if ( (fp = openFromDir(basedbname, pwddir ) ) != NULL ) {
				return fp ;
		} else if (fn_status & D_INDEXDIR ) {
			if ( (fp = openFromDir(basedbname, indexdir_varpath ) ) != NULL ) {
				return fp ;
			} else {
				/* create the file and return its filepointer if nothing went wrong */
					tmpfname = makeAfilename(basedbname,indexdir_varpath ) ;	
				if ((fp = fopen(tmpfname, "w")) == NULL) {
					/* print error message and die */
					yerror(YFILE_CREAT_ERR,procname,tmpfname,YX_EXTERNAL_CAUSE ) ;
				} else {
					fulldbname = tmpfname ;
					tmpfname = NULL ;
					fn_status |= F_CREATED ;
					fn_status |= F_FULL_PATH ;
					return fp ;
				}
			}
		} else {

			if( (fp = openFromDir(basedbname, indexhomedir ) ) != NULL ) {
				return fp ;
			} else {
					tmpfname = makeAfilename(basedbname,indexhomedir ) ;	
					if ((fp = fopen(tmpfname, "w")) == NULL) {
						/* print error message and die */
						yerror(YFILE_CREAT_ERR,procname,tmpfname,YX_EXTERNAL_CAUSE ) ;
					} else {
						fulldbname = tmpfname ;
						tmpfname = NULL ;
						fn_status |= F_CREATED ;
						fn_status |= F_FULL_PATH ;
						return fp ;
					}
			}
		}
   	} else {
    	/* we don't have the path:
    	We start checking the PWD, we'll do this all of the time.  in the label as well.*/
		/* when we get a hit, openFromDir will store the fulldbname and set F_FULL_PATH */ 
		if ( (fp = openFromDir(basedbname, pwddir ) ) != NULL ) {
				return fp ;
		}
		/* if no success with that, then we check the index dir. */
		if (fn_status & D_INDEXDIR ) {
			if( (fp = openFromDir(basedbname,indexdir_varpath ) ) != NULL ) {
				return fp ;
			}
		}
    	/* we'll check the INDEXHOMEDIR if we haven't found it. */
		
		if( (fp = openFromDir(basedbname, indexhomedir ) ) != NULL ) {
				return fp ;
		} else {
			yerror(YFILE_FINDF_ERR,procname,basedbname,YX_EXTERNAL_CAUSE ) ;
			/* we'll exit through yerror */
		}
    }
}

/* returns 1 True if the db file has ben created */
int
justCreated(void) {
	if ( fn_status & F_CREATED ) {
		return 1 ;
	} else { 
		return 0 ;
	}
}

/* 	Set that we have created a label file
	for the case of giving a new filename on the commandline.
*/
void
setLabelFileCreated(void)
{
	fn_status |= L_CREATED ;
}

/* returns true if a new label file has just been created. */
int
labelFileIsJustCreated(void ) 
{
	if ( fn_status & L_CREATED ) {
		return 1 ;
	} else {
		return 0 ;
	}
}

/* 
Returns 1 if the index file exists, 0 otherwise.
This function is called from select_dbfile, so it can only deal with files
existing in one or two directories, either the PATH pointed to by the INDEXDIR
variable, or from INDEXHOMEDIR. It saves a pointer to the label file if it
finds it as a nice side-effect.
*/
int
labelFileExists(wchar_t * dbname)
{
	static const char procname[]="labelFileExists" ;
	static const char fulldbdirname[]="fulldbdir" ;
	struct stat st;
	int success= 0 ;
	char * lblfname = wcstombs_alloc(dbname) ;

	size_t lblfnamelen = strlen(lblfname) + strlen(IDXFILE_SUFFIX) + 1 ;
	lblfname = (char *) yrealloc(lblfname, lblfnamelen,
			procname,"lblfname") ;
	strcat(lblfname,IDXFILE_SUFFIX) ;
	char *lblfile = NULL; 
	if (fn_status & D_INDEXDIR ) {
		lblfile = makeAfilename( lblfname, indexdir_varpath) ;
		if (stat(lblfile, &st) == 0) {
			size_t dir_len = strlen(indexdir_varpath) + 1 ;
			fulldbdir = (char *) ymalloc(dir_len,procname,fulldbdirname) ; 
			strcpy(fulldbdir,indexdir_varpath) ;
			success = 1 ;
		}
	}
	if (!success) {
		if( lblfile != NULL ) {
			free(lblfile ) ;
			lblfile = NULL ;
		}
		lblfile = makeAfilename( lblfname, indexhomedir) ;
		if (stat(lblfile, &st) == 0) {
			 size_t dir_len = strlen(indexhomedir) + 1 ;
			fulldbdir = (char *) ymalloc(dir_len,procname,fulldbdirname) ; 
			strcpy(fulldbdir,indexhomedir) ;
			success = 1 ;
		}
	}
	free(lblfname) ;
	lblfname=NULL;
	if (!success) {
		free(lblfile) ;
		lblfile=NULL;
	} else {
		fn_status |= L_FULL_PATH ;
		fulllabelname = lblfile ;
		lblfile = NULL ;
	}
	return success ;
}

/* 
Constructs a valid label_filename and returns a pointer to it.

According to the following rules:	

	We don't consider any full path, just if we have the INDEXDIR
	directory variable set in the environment or not.

	If INDEXDIR is set before invocation, then that is the path we
	use for the label file, if not we create it in $HOME/.index
*/
char *
newLabelFilePath(void) 
{
	if (!(fn_status & F_GOTFNAME )) {
		ysimpleError("newLabelFilePath: stemdbname not set before calling me.",YX_EXTERNAL_CAUSE) ;
	} 
	/* we really don't consider the filename */
	if ( fn_status & D_INDEXDIR ) {
			
		fulllabelname = makeAfilenameFromStem( IDXFILE_SUFFIX,
			stemdbname,indexdir_varpath) ; 
	} else {
		fulllabelname = makeAfilenameFromStem(
				IDXFILE_SUFFIX,stemdbname,indexhomedir) ; 
	}
	return fulllabelname ;
}
/* returs a previously set fullabelFilename
   TODO: evaluate if this one is to be made like getBackupFilename below.
   When in doubt any caller should look if the L_CREATED flag is set.
*/
char *
getFullLabelFileName(void) 
{
	if ( fulllabelname == NULL ) {
		ysimpleError("getFullLabelFileName: fullabel name not set.",YX_EXTERNAL_CAUSE) ;
	}
	return fulllabelname ;
}
/*
Makes a backupname by the emacs convention:
we put a '~' at the end of the filename, we are fed.
*/
char *
makeBackupName( const char * originalname )
{
	size_t bcknameLen = strlen(originalname) + 2 ;
	char *tmpName = (char *) ymalloc(bcknameLen,"getBackupFileName","tmpName") ; 
	strcpy(tmpName,originalname ) ;
	strcat(tmpName,"~") ;
 	return tmpName ;
}
/*
Returns the name for the backup file.
(Adds "~" at end of .db name.)
Creates the name if it doesnt exist.
*/
char *
getDbBackupFileName(void)
{
	if (!(fn_status & F_BCK_NAME )) {
		/* create the backupfile */
		bckdbname = makeBackupName(fulldbname ) ;
		fn_status |= F_BCK_NAME ;
	}
	return bckdbname ;
}
/*
Returns the fulldbname.
A caller should check the flag F_FULL_PATH
before calling it. (dbFileHasNoPath).
There isn't much use of a dbfile before it has
a full db name, which is the full path to the file.
*/
char *
getFullDbName(void)
{ 
	if (( fulldbname == NULL ) &&  (fn_status &  F_FULL_PATH) && (!(fn_status & F_ARGVNAME)) ) {
		ysimpleError("getFullDbName: fulldbname name not set.",YX_EXTERNAL_CAUSE) ;
		finish(0) ;
		exit(YX_EXTERNAL_CAUSE ) ;
	}
	return fulldbname ;
}

/*
Returns true if the dbfile is resolved with a full path.
*/
int
dbFileHasNoPath(void ) 
{
	if (!(fn_status & F_FULL_PATH )) {
		return 1 ;
	} else {
		return 0 ;
	}
}


/*
Returns a file pointer to the open label file.  Or NULL if we
didn't find an existing file.

It is called from read_labelfile() .  If the label file has been
created just before we are called, then the flag F_LABEL_MADE is
set, and we can return the path immediately.

Likewise, if labelFileExists has found the label file, then the
flag: L_FULL_PATH has been set, and we can retrieve the file
automatically. It returns NULL if it cant find a label file.
If this happens in read_labelfile(), then read_label file will
create an error message if the db file doesn't exist yet, or
die with an error message if it does. 
 
In some cases now, if you manage two kinds of files with the same
name, and forgets to specify the INDEXDIR, then you may end up with
some trouble. We find a path to a db file, but no INDEXDIR FILE,so
we are getting an error message.

TODO: incorporate this suggestion into the error message.
	*/
FILE *
open_label_file(void)
{
	const char procname[]="open_label_file";
	FILE *fp;

	if ((fn_status & L_FULL_PATH )|| (fn_status & F_LABEL_MADE )) {
	/*	fprintf(stderr,"Full path is set\n") ; */
	/* 	This is the situation, if we found the index file in
		the exists_label_file() from within select_db(). */
		if ((fp = fopen(fulllabelname, "r")) == NULL ) {
			yerror(YFILE_FINDF_ERR,procname,fulllabelname,YX_USER_ERROR ) ;
		} else {
			return fp ;
		}
	} /* else we are seeking a label file from read label file.
	( We have bypassed select_db() as a filename was given
	albeit non-existing.)
	if we had a filaname, then we passed by select_db(). */	
	/* we need to create baselabelfname first! */
	/* fprintf(stderr,"stemdbname == %s\n",stemdbname ) ;
	fprintf(stderr," len stemdbname = %u\n",strlen(stemdbname) );	 */
	size_t baselabelStorage = strlen(stemdbname) + strlen(IDXFILE_SUFFIX) + 1 ;
	
	char *baselabelname = (char *) ymalloc(baselabelStorage,procname,"baselabelname") ; 
	strcpy(baselabelname,stemdbname) ;
	strcat(baselabelname,IDXFILE_SUFFIX);
	/* TODO: any advantages of updating the fulldbname variable??? */	
	if (fn_status & D_INDEXDIR ) {
		if( (fp = openLabelFileFromDir(baselabelname,indexdir_varpath ) ) != NULL ) {
			return fp ;
		}
	}
	if( (fp = openLabelFileFromDir(baselabelname, indexhomedir ) ) != NULL ) {
		return fp ;
	}
	return NULL ;
}
 
/*
Returns true if we should create the missing label file.
It is ok to create the label file, if we are in the process
of creating a new dbase file, specified from the command line,
Or creating a new dbase file thru the user interface.
--Business rules used in openLabeFile() from dbio.c
*/
int
shouldCreateMissingLabelFile(void) 
{
	struct stat st;
	if (fn_status & F_FULL_PATH ) {
		if (stat(fulldbname, &st) == 0) {
			/* the file exists */
			if ( fn_status & F_CREATED ) {
				/* have just created the db from commandline 
				   seems like we need a label file as well. */
				return 1 ;
			} else { 
				return 0;
			}
		} else {
		/* it doesn't, ok to make label file */
			return 1;
		}
	} else {
		ysimpleError("shouldCreateMissingLabelFile: No full path so won't bother.",YX_EXTERNAL_CAUSE) ;
	}
	return 0 ; /* for compiler */
}

/*
Sets the dbfiltername with one
supplied from the command line.
*/
void
set_dbfilter( char *filtername ) 
{
	size_t filterlen = strlen(filtername) + 1 ;
	dbfilter = (char *) ymalloc(filterlen,"set_dbfilter","dbfilter") ; 
	fn_status |= F_HAS_FILTER ;
	if( *filtername == '=') {
		++filtername ;
	}
	strcpy(dbfilter,filtername) ;
}
/*
Returns true if there were a dbfilter name
supplied from the command line.
*/
int
has_dbfilter(void) 
{
	if ( fn_status & F_HAS_FILTER ) {
		return 1 ;
	} else {
		return 0 ;
	}
}

/*
Returns filepointer to dbfilter supplied from command-line.

for one, there is no correlation between names, and we have to set the name somewhere.
secondly: the behaviour here is more like open_db, and it is possible to have the filter
stored locally here as well. The second thing, is the thing with relative paths.
We may equally well have to resolve a relative path.
thirdly:the filter may be without, or have a totally different suffix.


	TODO: if not found, returns bare filter name, so it can be searched for through
	the path!

	I don't think we should open it either, just return it.
	(so we test for access).
   
	A filter file, is only tried to be opened once, and then path is returned,
	so this is just for label files, and, but mainly for dbfiles.
   if we are to make this to work from here, then we'd better send with the
   constant, so we can know which full filenemae to set!
*/
char *
open_dbfilter(void)
{

	/* 	New Rules:
		
		if we have a full path, and we don't find it, then that is an error.
		
		if no path is specified, then that isn't an error.
	*/
	const char procname[]="open_dbfilter" ;
	struct stat st;
	int onlystem=0 ;
	char *resultptr = NULL ;
	char *completeBaseName = NULL ;
    char * basefiltername = extBasenameCheck(dbfilter,procname,"dbfilter") ;
	
	if (strstr(dbfilter,FMTFILE_SUFFIX)==NULL) {
		onlystem = 1 ;
		size_t completefilterlen =strlen(dbfilter)+strlen(FMTFILE_SUFFIX) + 1  ;
		completeBaseName = (char *) ymalloc(completefilterlen,procname,"completeBaseName") ; 
		strcpy(completeBaseName,dbfilter) ;
		strcat(completeBaseName,FMTFILE_SUFFIX) ;
	}

	char * pwdsuffixname = NULL  ;
	int success = 0 ;
	/* checks if stem and the full argument are alike */
	if (!strcmp(basefiltername,dbfilter)) {
    	/* we don't have the path:
    	We start checking the PWD, we'll do this all of the time.  in the label as well.*/
		
		char * pwdstemname= makeAfilename( basefiltername, pwddir) ;
		if (stat(pwdstemname, &st) == 0) {
				resultptr = pwdstemname ;
				success = 1 ;
		} else if (onlystem ) {
			free(pwdstemname );
			pwdstemname = NULL ;

			pwdsuffixname= makeAfilename( completeBaseName, pwddir) ;
			if (stat(pwdsuffixname, &st) == 0) {
				resultptr = pwdsuffixname ;
				success = 1 ;
			}
		}
		/* if no success with that, then we check the index dir. */
		if ((!success) && (fn_status & D_INDEXDIR )) {
			/*
				D_INDEXDIR means that the INDEXDIR variable contains
			   	the path to the "database" files.
			*/
			free(pwdsuffixname) ;
			pwdsuffixname = NULL ;
			
			char * indexdirstemname = makeAfilename( basefiltername, indexdir_varpath) ;
			if (stat(indexdirstemname, &st) == 0) {
				resultptr = indexdirstemname ;
				success = 1 ;
			} else if ((!success) && (onlystem )) {
				free(indexdirstemname ) ;
				indexdirstemname = NULL ;
				char * indexsuffixname= makeAfilename( completeBaseName, indexdir_varpath) ;
				if (stat(indexsuffixname, &st) == 0) {
					resultptr = indexsuffixname ;
					success = 1 ;
				} else {
					free(indexsuffixname ) ;
					indexsuffixname = NULL ; 
				}
			}
		}
    	/* we'll check the INDEXHOMEDIR if we haven't found it. */
		
		if (!success) {
			
			char * indexhomedirstemname = makeAfilename( basefiltername, indexhomedir) ;
			if (stat(indexhomedirstemname, &st) == 0) {
				resultptr = indexhomedirstemname ;
				
				success = 1 ;
			} else if (onlystem ) { 
				free(indexhomedirstemname ) ;
				indexhomedirstemname = NULL ;
				char * indexhomedirsuffixname = makeAfilename( completeBaseName, indexhomedir) ;
				if (stat(indexhomedirsuffixname, &st) == 0) {
					resultptr = indexhomedirsuffixname ;
					success = 1 ;
				} else {
					free( indexhomedirsuffixname ) ;
					indexhomedirsuffixname = NULL ;
				} 
			}
		}
		if (!success) {
			resultptr = dbfilter ;
		}
		if (onlystem ) {
			free(completeBaseName) ;
			completeBaseName= NULL ;
		}
		return resultptr ;
		
	} else {
	/* the filtername and base part of it wasn't the same, we'll have to look
	   for a relative reference.
	*/
		const char fullfilterdirname[]="fullfilterdir" ;
		int error_code = 0 ;	
		char *fullfiltername =tildeExpandedFileName(dbfilter ) ;	
		char *fullfilterdir = (char *) ymalloc((PATH_MAX +1),procname,fullfilterdirname) ; 
    	realpath(fullfiltername,fullfilterdir) ;
		size_t fullfilterdirlen = strlen(fullfilterdir) ;
		fullfilterdir = (char *) yrealloc(fullfilterdir, (fullfilterdirlen+1),
			procname,fullfilterdirname) ;
    	size_t fullfilterbaselen = fullfilterdirlen +strlen(basefiltername) +2 ;
		char *fullfilterbase = (char *) ymalloc(fullfilterbaselen,procname,"fullfilterbase") ; 
    	strcat(fullfilterdir,"/") ;
		strcat(fullfilterdir,basefiltername) ;
		/* HERE */
		if (stat(fullfilterdir, &st) == 0) {
			resultptr = fullfilterdir ;
			goto _ffilt ; /* Adjusted */
		} else if (!onlystem ) {
			yerror(YFILE_FINDF_ERR,procname,fullfilterbase,YX_USER_ERROR ) ;
		}
		const char fullfiltersuffixname[]="fullfiltersuffix" ;
		char *fullfiltersuffix = (char *) ymalloc((PATH_MAX+1),procname,fullfiltersuffixname) ; 
		strcpy(fullfiltersuffix,fullfilterdir ) ;
		strcat(fullfiltersuffix,"/");
		strcat(fullfiltersuffix,completeBaseName) ;
		size_t fullfiltersuffixlen = strlen(fullfiltersuffix ) ;
		fullfiltersuffix = (char *) yrealloc(fullfiltersuffix, (fullfiltersuffixlen+1),
			procname,fullfiltersuffixname) ;
		if (stat(fullfiltersuffix, &st) == 0) {
			resultptr = fullfiltersuffix ;
			goto _fbase ;
		} else {
			yerror(YFILE_FINDF_ERR,procname,fullfiltersuffix,YX_USER_ERROR ) ;
		}
/* New memory deallocation rules:
	if we find it firs,t then we won't have to dealloc the last.
	but we'll keep the one we have.

	if we find the last, then we'll dealloc everything expect that one.
	which will be returned, throug a common pointer.

	The error problems makes it even more difficult.
*/
		free(fullfiltersuffix) ;
		fullfiltersuffix = NULL ;
_fbase:
		free(fullfilterbase) ;
		fullfilterbase = NULL ;
_ffilt:
		free(fullfilterdir) ;
		fullfilterdir = NULL ;
		free(fullfiltername) ;
		fullfiltername = NULL ;
		if (onlystem ) {
			free(completeBaseName) ;
			completeBaseName= NULL ;
		}
		if (! error_code ) {
			return resultptr;
		} else {
			finish(0) ;
			exit(error_code ) ;
		}		

	}
}

/*
Returns true if no dbase_name have been given from commandline.
*/
int no_dbase_name(void)
{
	if ((fn_status & F_GOTFNAME ) == 0 ) {
		return 1 ;
	} else {
		return 0 ; /* tests for the negative */
	}
}

/*
TODO: it should be fairly easy for us to know which directory we are listing files from
so we can complete this function.
*/
void
set_dbase_shortname( wchar_t *dbname )
{
	const char procname[]="set_dbase_shortname" ;	
	size_t dbnamelen = wcslen(dbname) ;
	/* we have to convert it back to utf8 */
	shortdbname = (wchar_t *) ymalloc(((dbnamelen+1)*sizeof(wchar_t)),
		procname,"shortdbname") ; 
	wcscpy(shortdbname,dbname) ;
	fn_status |= F_GOTFNAME ;
	
	stemdbname = wcstombs_alloc(dbname) ;
	size_t basedblen = strlen(stemdbname)  + strlen(DBFILE_SUFFIX) + 1 ;
	basedbname = (char *) ymalloc(basedblen, procname,"basedbname") ; 
	memset(basedbname,(int) 0, (basedblen*sizeof(wchar_t))) ;	
	strcpy(basedbname,stemdbname) ; 
	strcat(basedbname,DBFILE_SUFFIX) ;
	fn_status |= F_ARGVNAME ;
}

/*
Returns the shortdb wchar_t name,
will only be used from main.
*/
wchar_t *
get_dbase_name(void)
{
	return shortdbname;
}

/* cleans up all memory that we have allocated */
/* will be called from the finish handler, that 
   will be called from every errmsg handler */
void
release_filenames(void)
{
	if (fn_status & F_GOTFNAME ) {
		/* TODO:the moment we set dbshortname from 
		dbselect, we do set F_ARGVNAME AS WELL */
		if (stemdbname != NULL ) {
			free(stemdbname) ;
			stemdbname = NULL ;	
		}
		free(basedbname) ;
		basedbname = NULL ;	
		free(shortdbname ) ;
		shortdbname = NULL ; 
	}
	if (fn_status & F_FULL_PATH ) {
		free(fulldbdir ) ;
		fulldbdir = NULL ;
		free(fulldbname) ;
		fulldbname = NULL ;
	}
	if (fn_status & D_INDEXDIR ) {
		free(indexdir_varpath) ;
	 	indexdir_varpath = NULL ;

	}
	if (fn_status & D_HOMEDIR ) {
		free(indexhomedir ) ;
		 indexhomedir=NULL;
		free(pwddir ) ;
		pwddir = NULL ; 
	}
	if (fn_status & F_HAS_FILTER ) {
		free(dbfilter) ;
		dbfilter=NULL;
	}
	if (fn_status & F_LABEL_MADE ) {
		free(fulllabelname) ;
		fulllabelname = NULL ;
	}
	if (fn_status & F_BCK_NAME  ) {
		free(bckdbname ) ;
		bckdbname = NULL ;
	}
}

/* sets the database name from the commandline, but doesn't try to 
   check if it, exists. It stores basedbname, stemdbdname, and 
   shortdbname at a minimum, if the dabase name came with a path,
   absolute or relative, then it will resolve this path, and store the
   directory part of it as well.

   Which is a dubious thing to do, at least until I have read the spec.
   for finding the filters, and maybe rewritten it to also look where
   the dbfile resides.
   
   other routines, that will check if the database file
   exists, or resolve to the name that should be created are:

   it extracts the directory the file is to be created in, if the file
   doesn't exist.

	This one doesn't cover for all possible errors.
   */
void
set_dbase_name(char *dbasearg)
{
	const char procname[]="set_dbase_name" ;
	char *filename = NULL, *tmpStem = NULL ;
	size_t stemdbLen = 0,  basedbLen = 0 ,dbshortLen = 0 ;
	int dbaseargIsStem = 0 ;
	
	fn_status |= (F_GOTFNAME | F_ARGVNAME )  ;
	stemdbname = extBasenameCheck(dbasearg,procname,"dbasearg") ;
	/* stemdbname gets malloce'd and copied over below! */

	/* checks if stem and the full argument are alike */
	if (!strcmp(stemdbname,dbasearg)) {
		dbaseargIsStem=1 ;
		/* Then we only have to create shortname that is in wchar_t,
	 		stembdname and basebname which has suffix. */
	}
	int missingSuffix=1 ;	
	/* need to know wheter suffix was supplied or not, so we know
	   how to create basedbname and stemdbname*/
	if (strstr(stemdbname,DBFILE_SUFFIX) != NULL ) {
		missingSuffix = 0 ;
		basedbLen = strlen(stemdbname) + 1;
		dbshortLen = basedbLen - strlen(DBFILE_SUFFIX) ;
		stemdbLen = dbshortLen -1 ;
		
	} else { 
		basedbLen = strlen(stemdbname) + 1 ;
		dbshortLen = basedbLen ;
		stemdbLen = dbshortLen - 1 ;
		basedbLen += strlen(DBFILE_SUFFIX) ;
	}
	
	tmpStem = (char *) ymalloc((stemdbLen+1),procname,"tmpStem") ; 
	/* we start out only copying over the part that will populate shortdbname */	
	strncpy(tmpStem,stemdbname,stemdbLen) ;
	tmpStem[stemdbLen] = '\0' ; /* needs this */	

	shortdbname = (wchar_t *) ymalloc((dbshortLen * sizeof(wchar_t )) ,
		procname,"shortdbname") ; 
	mbstowcs(shortdbname, tmpStem, stemdbLen );
	fn_status |= F_GOTFNAME ;
	basedbname = (char *) ymalloc(basedbLen,procname,"basedbLen") ; 
	
	strcpy(basedbname,tmpStem) ;
	strcat(basedbname,DBFILE_SUFFIX ) ;
	stemdbname = tmpStem ;
	tmpStem = NULL ;

	if (dbaseargIsStem ) {
		return; /* we are done, there are no path or anything to resolve. */
	}
	
	filename = tildeExpandedFileName(dbasearg);
	fn_status |= F_FULL_PATH  ; /* implies that we have a directory */
	/* check for a relative path
		relative paths are strings with "./" if a such string is in the path
		then the path is relative, or may very well be.
	*/
	fulldbname = (char *) ymalloc((PATH_MAX+1),procname,"fulldbname") ; 
	/* It seems to resolve the full pathname, not just the dir */
	realpath(filename,fulldbname) ; /* taker is arg 2 */
	free(filename );
	filename = NULL ;
	
	size_t full_len = strlen(fulldbname) + 1 + (missingSuffix * strlen(DBFILE_SUFFIX)) ;
	fulldbname = (char *) yrealloc(fulldbname, full_len,
			procname,"fulldbname") ;
	
	if (missingSuffix ) {
		strcat(fulldbname,DBFILE_SUFFIX ) ;
	}
	
	fulldbdir = (char *) ymalloc((full_len+1),procname,"fulldbdir") ; 
	strcpy(fulldbdir,fulldbname ) ;
	strcat(fulldbdir,"/") ;
	fulldbdir  = dirname(fulldbdir) ;
	size_t fulldbdirlen = strlen(fulldbdir) + 1 ;
	fulldbdir = (char *) yrealloc(fulldbdir, fulldbdirlen,procname,"fulldbdir") ; 
	fn_status |= F_ARGV_DIR ; /* we got it if we want to look for filters in it. */
	
}

/* returns a tilde expanded name for when the tilde isn't automatically expanded.
that is, when a tilde is set infront of a username. */
static char
*tildeExpandedFileName(char *farg)
{
	const char procname[]="tildeExpandedFilename" ;
	const char filenamename[]="filename" ;
	/* ok, so we have something that can pass for a a filename */
	size_t slen = strlen(farg);
	char *filename = (char *) ymalloc((PATH_MAX+1),procname,filenamename) ; 
	strcpy(filename, farg);
	if ((char)filename[0] == '~') {
		/* if we come here; tilde wasn't alone: followed by a user name. */
		char *myhome = getenv("HOME");	/* First we'll have to get our name */

		if (myhome == NULL) {
			yerror(YPTH_NOPWD_ERR,procname,"$HOME",YX_USER_ERROR);  
		}
		size_t hlen = strlen(myhome);

		if (hlen == 1) {

		ysimpleError(
			"tildeExpandFileName: I can't resolve a users path while we are  logged on as root.",
				YX_EXTERNAL_CAUSE);
		}
		char *userpath = (char *) ymalloc((hlen+slen),procname,"userpath") ; 

		/*      This space will always suffice since we loose more than the tilde
		   if there was just a tilde, like prepending a path, then we will never get it.
		 */
		/* Create a string containing parent directory. */
		char *lastocc = strrchr(myhome, '/');

		size_t i = (size_t)(lastocc - myhome) + 1;

		strncpy(userpath, myhome, i);	/* cpy HOME */
		strcat(userpath, (filename + 1));	/* append the filename without tilde */
		strcpy(filename, userpath);	/* copy it over to dest */
		free(userpath);	/*  free the resources */
		userpath = NULL;
	}
	size_t filenamelen=strlen(filename) ;
	filename = (char *) yrealloc(filename,filenamelen, procname,filenamename ) ; 
	return filename;
}
