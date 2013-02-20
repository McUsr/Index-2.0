#ifndef __DBPRINT__
#define __DBPRINT__
void set_verbose(void) ;
void set_fieldsep(char fsep) ;
void set_csv( void )  ;
void set_header(void) ;
void set_quiet(void) ;
void set_nosql(void) ;
void set_suppress_labels(void) ;
int 	print_db(void) ;
#endif
