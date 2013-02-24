/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4:nomagic
 * printdb.c - print entries from the dafield_separatorase.
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
 */
#include <common.h>
#include <initcurses.h>
#include <unicodeToUtf8.h>
#include <defs.h>
#include <dbfunc.h>
#include <util.h>
#include <dbio.h>
#include <dbfilename.h>
#include <dbprint.h>
 
static int quiet = 0 ;

static int header = 0;

static int verbose = 0;

static int nosql = 0;

static int csv = 0 ;

static int suppress_labels = 0;

static char field_separator = (char )0x09; /* tab */

static char * csv_value( const char *field_value, int FIELD_DELIM ) ;

void
set_csv( void ) 
{
    csv = 1 ;
}

void
set_quiet(void)
{
    quiet=1 ;
}
void
set_header(void)
{
	header = 1;
}

void
set_fieldsep(char fsep)
{
	field_separator = fsep;
}

void
set_verbose(void)
{
	verbose = 1;
}

void
set_nosql(void)
{
	nosql = 1;
}

void
set_suppress_labels(void)
{
	suppress_labels = 1;
}

/*
   Returns a hyphenated string, with any cvs delimiters escaped insde the string.

 */
 
#define  ESCAPE  '\\'
#define HYPHEN  '"' 
static char *
csv_value( const char *field_value, int FIELD_DELIM )
{
		size_t field_len=strlen(field_value) ;
        register int nr_delims=0;
        char *delim_inside = strchr(field_value,FIELD_DELIM) ;
        while ( delim_inside != NULL ) {
            nr_delims++ ; /* counts them all */
            char *next_searchpos = (++delim_inside) ;
            delim_inside = strchr(next_searchpos,FIELD_DELIM ) ;
        }
        /* we make the buffer to insert the quoted string in */
		size_t csv_field_len = field_len + 3 + nr_delims ;
		/* place to hold fieldvalue + escapes "\", hyphens, and null byte */
	   	char *csv_string  =   (char *)ymalloc(csv_field_len,"csv_value","csv_string" );
        memset(csv_string, (int) 0, csv_field_len ) ;
        char *inptr = csv_string ;
        *inptr++ = HYPHEN ;
		
        if (nr_delims ) {
			/* had the chosen field-delimiter insde the string. */
            const char *outptr = field_value ;
            delim_inside = strchr(field_value,FIELD_DELIM) ;
            while ( delim_inside != NULL ) {
                while ( outptr < delim_inside ) {
                    *inptr++ = *outptr++ ;
                }
                *inptr++ = ESCAPE ;
                *inptr++ = *outptr++ ;
                delim_inside = strchr(outptr,FIELD_DELIM) ;
            }
            while (*outptr != '\0' ) {
                *inptr++ = *outptr++ ;
            }
			*inptr++ = HYPHEN ;
        	inptr = NULL ;
        } else {
			strcat(inptr,field_value) ;
			strcat(inptr,"\"") ;
		}
	return csv_string ;
}
/*
 * print_db - print out entries marked DB_PRINT in the dafield_separatorase.
 * The return value from print_db becomes the exit code of index.
 */
int
print_db(void)
{
    const char procname[]="print_db" ;
	FILE *pp;

	char * buf;

	char *idplines[MAXDBLINES];

	char *dblinebuf = NULL;

	size_t buflen = 0;
    
    int hasoutput=0 ;

	register int i, j;


    /* QUIET: if quiet is set we just return 0 if there were data
     1 if there were none test for filter. */
     
   /* an inverted options would also be nice */
   
	/*
	 * If no dbFilter was specified, we just spit the entries out,
	 * with their field names, to standard output.
	 */
	/* if (((dbFilter == NULL) && (!supress_labels) && header ) && (!supress_labels )) ||  */
	if (!has_dbfilter()) {
        /* We are performing a simple print. */
		if ((!suppress_labels) && (!quiet)) {
                /* We are going to print them */
			if (header ) {
                /* on top, and present the date in a tabular format */
				for (i = 0; i < idx.idx_nlines; i++) {
					/* what happens when the entries are null?
					   -can't happen! */
					if (nosql) {
						char *tmpbuf =
						    wcstombs_alloc(idx.idx_lines
								   [i]);

						size_t tmpbuflen =
						    strlen(tmpbuf);

						char *nsqlified =
						    (char *)ymalloc((tmpbuflen + 2),procname,"nsqlified");
                        char *t = nsqlified ;
                        *t++ = (char) 0x01 ;
                        strcat(t,tmpbuf ) ;
						free(tmpbuf);
						tmpbuf = NULL;
                        t = NULL ;
                        if (csv) {
                            idplines[i] = csv_value(nsqlified, field_separator ) ;
                            free(nsqlified );
                            nsqlified = NULL ;
                        } else {
						    idplines[i] = nsqlified;
                        }
					} else {
                        if (csv) {
                            char *t = wcstombs_alloc(idx.idx_lines[i]);
                            idplines[i] = csv_value(t, field_separator ) ;
                            free (t ) ;
                            t = NULL ;
                        } else {
						    idplines[i] = wcstombs_alloc(idx.idx_lines[i]);
                        }
					}
				}

				fprintf(stdout, "%s", idplines[0]);
                free(idplines[0]) ;
                idplines[0] = NULL ;
				for (i = 1; i < idx.idx_nlines; i++) {
					fprintf(stdout, "%c%s", field_separator,
						idplines[i]);
					free(idplines[i]);
					idplines[i] = NULL;
				}
                fprintf(stdout,"\n") ;
		    } else {
                /* print the data record by record, one field per line */
				for (i = 0; i < idx.idx_nlines; i++) {
					/* what happens when the entries are null?
					   -can't happen! */
					idplines[i] =
					    wcstombs_alloc(idx.idx_lines[i]);
				}
			}
		}
        /* we are now  taking care of outputing the records */
		for (i = 0; i < dbentries; i++) {
			if ((db[i].db_flag & DB_VALID) == 0)
				continue;
			if ((db[i].db_flag & DB_PRINT) == 0)
				continue;

            hasoutput = 1 ;
            if (quiet) {
                break ;
            }
			for (j = 0; j < idx.idx_nlines; j++) {
				/* verbose -v command line option */
				if ((!verbose) && (!csv) && (!header)) {
					if (db[i].db_lens[j] == 0)
						continue;
				}
                register int flen =db[i].db_lens[j] ; 
				if ( flen  > 0) {
					dblinebuf =
					    utf8FromUnicode_alloc(&buflen,
					        db[i].db_lines [j], u_strlen(db [i].db_lines [j]));
					dblinebuf[buflen] = '\0';
                } 
			    if (!header) {
                    if (flen > 0 ) {
                        if (!suppress_labels ) {
					        printf("%-*s%s\n", idx.idx_maxlen + 2, idplines[j],
                            dblinebuf);
                        } else {
					        printf("%s\n", dblinebuf);
                        }
				        free(dblinebuf);
				        dblinebuf = NULL;
                    } else {
                        if (!suppress_labels ) {
					        printf("%-*s\n", idx.idx_maxlen + 2, idplines[j]);
                        } else {
                             printf("\n" ) ;
                        }
                    }
				} else {
                    if (j < ( idx.idx_nlines - 1 )) {
                    /* following field separator */
                        if (!flen ) {
                            printf("%c",field_separator ) ;
                        } else if (csv ) {
                            char *t = csv_value(dblinebuf,field_separator ) ;
                            printf("%s%c",t,field_separator ) ;
                            free(t) ;
                            t = NULL ;
				            free(dblinebuf);
				            dblinebuf = NULL;
                        } else {
					        printf("%s%c", dblinebuf,field_separator);
				            free(dblinebuf);
				            dblinebuf = NULL;
                        }
                    } else  if (flen) {
                        if (csv ) {
                            char *t = csv_value(dblinebuf,field_separator ) ;
                            printf("%s",t ) ;
                            free(t) ;
                            t = NULL ;
				            free(dblinebuf);
				            dblinebuf = NULL;
                        } else {
					        printf("%s", dblinebuf);
				            free(dblinebuf);
				            dblinebuf = NULL;
                        }
                    }
                            
				}
			}
			putchar('\n');
		}
        if ( (!quiet) && (!suppress_labels )) {
		    for (i = 0; i < idx.idx_nlines; i++) {
			    free(idplines[i]);
			    idplines[i] = NULL;
		    }
            /* we have just figured if any of the data was to be printed */
        } 
	   return (hasoutput == 1 ) ? 0 : 1 ;
	}
    fprintf(stderr,"has dbfilter in print_db\n") ;
	/*
	 * Otherwise, we set up a pipe to the dbFilter, and print
	 * first the field names, and then the fields.  We do
	 * this one entry per line, with fields separated by
	 * field_separators.
	 */

	/*
	 * Create the path to a formatting program in the dafield_separatorase
	 * directory.
	 */
    buf = open_dbfilter() ;
	/*
	 * If that's not there, then assume they gave us some
	 * program name (like "more" or something), and just
	 * stick it in there.
	 */
	/*
	 * Open the pipe.
	 */
	if ((pp = popen(buf, "w")) == NULL) {
        yerror(YPIP_EXEC_ERR,procname,buf,YX_EXTERNAL_CAUSE ) ;
	}
	/* det kan hende vi vil ha på headers sendere uten filter
	   da havner dette over.
	 */
    if (!suppress_labels ) {
    	for (i = 0; i < idx.idx_nlines; i++) {
    		/* what happens when the entries are null?
    		   -can't happen! */
    		if (nosql) {
    			char *tmpbuf = wcstombs_alloc(idx.idx_lines[i]);
    
    			size_t tmpbuflen = strlen(tmpbuf);
    
    			char *nsqlified = (char *)ymalloc((tmpbuflen + 2),procname,"nsqlified");
                char *t = nsqlified ;
                *t++ = (char) 0x01 ;
                strcat(t,tmpbuf ) ;
    			free(tmpbuf);
    			tmpbuf = NULL;
                t=NULL ;
                if (csv) {
                    idplines[i] = csv_value(nsqlified, field_separator ) ;
                    free(nsqlified );
                    nsqlified = NULL ;
                } else {
    		        idplines[i] = nsqlified;
                }
    		} else {
                if (csv) {
                    char *t = wcstombs_alloc(idx.idx_lines[i]);
                    idplines[i] = csv_value(t, field_separator ) ;
                    free (t ) ;
                    t = NULL ;
                } else {
    		        idplines[i] = wcstombs_alloc(idx.idx_lines[i]);
                }
    		}
    	}
    
    	/*
    	 * Print the field names, separated by field_separator.
    	 */
    	fprintf(pp, "%s", idplines[0]);
        free(idplines[0]) ;
        idplines[0] = NULL ;
    	for (i = 1; i < idx.idx_nlines; i++) {
    		fprintf(pp, "%c%s", field_separator, idplines[i]);
    		free(idplines[i]);
    		idplines[i] = NULL;
    	}
    	putc('\n', pp);
    }
    /* TODO: manual page: quiet option doesn't work with a filter in use */

	/*
	 * Print the entries, with fields separated
	 * by field_separators.
	 */
	for (i = 0; i < dbentries; i++) {
		if ((db[i].db_flag & DB_VALID) == 0)
			continue;
		if ((db[i].db_flag & DB_PRINT) == 0)
			continue;

		for (j = 0; j < idx.idx_nlines; j++) {
			if (db[i].db_lens[j] > 0) {
				dblinebuf =
				    utf8FromUnicode_alloc(&buflen,
							  db[i].db_lines[j],
							  u_strlen(db
								   [i].db_lines
								   [j]));
				dblinebuf[buflen] = '\0';
                if (j < ( idx.idx_nlines - 1 )) {
                    if (csv) {
                        char *t = csv_value(dblinebuf,field_separator ) ;
                        fprintf(pp,"%s%c",t,field_separator ) ;
                        free(t) ;
                        t = NULL ;
                    } else {
				        fprintf(pp, "%s%c", dblinebuf,field_separator);
                    }
                } else {
                    if (csv) {
                        char *t = csv_value(dblinebuf,field_separator ) ;
                        fprintf(pp,"%s",t ) ;
                        free(t) ;
                        t = NULL ;
                    } else {
				        fprintf(pp, "%s", dblinebuf);
                    }
                }
			} else { /* field without contents */
                if (j < ( idx.idx_nlines - 1 )) {
                    /* if this isn't the last field */
				    fprintf(pp, "%c", field_separator);
                 }
			}
			free(dblinebuf);
			dblinebuf = NULL;
		}
		putc('\n', pp);
	}

	/*
	 * Close the pipe.
	 */
	pclose(pp);
    return 0 ;
}
