/* Prototypes for screen.c */

#ifndef __SCREEN__
#define __SCREEN__
void 			initEntryLine(void ) ;
void 			initHeading(void) ;
void 			paintHeading(const char *modeword) ;
int				edit_entry(dbrecord * entry, const char *operationDesc, const char *entryDesc) ;
wchar_t         prompt_char(int row,int col, const char *promptstr, const char *valid);
void            prompt_str( int row, int col, const char *promptstr, wchar_t *answer) ;
int             byebye(void) ;
#endif
