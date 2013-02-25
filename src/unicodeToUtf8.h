/* vim: nospell
 * 
har library for converting  between unicode and utf8 on Mac Os X        */
/* relies on the UIC library.   Copyrighted by IBM                      */
char 	* 	wcstombs_alloc(const wchar_t * wcsS) ;
size_t 		wcstombs_alloc2(char ** mbs, const wchar_t * wcsS, const size_t wcslen) ;
size_t  	mbstowcs_alloc2(wchar_t **buf,const char *string, const size_t slen) ;
wchar_t * 	mbstowcs_alloc(const char *string) ;
int32_t 	unicodeFromUTF8( UChar *uic_s, const int32_t uic_capacity, const char *utf8S,const size_t utf8Len ) ;
UChar  	*	unicodeFromWcs_alloc( size_t *uchlen, const wchar_t *wcsS ) ;
int32_t 	unicodeFromWcs( UChar *uic_s, const int32_t uic_capacity, const wchar_t *wcsS,const size_t wcsLen ) ;
size_t 		utf8FromUnicode(char *s, int32_t s_capacity, const UChar *ucsS, const int32_t ucsLen) ;
UChar 	*	lowercasedUString(wchar_t *wcsS) ;
char 	* 	utf8StringFromUnicode( UChar *ucsS ) ;
char 	* 	utf8FromUnicode_alloc( size_t *slen, const UChar *ucsS, const int32_t ucsLen) ;
wchar_t * 	wcsStringFromUnicode( UChar *ucsS )  ;
wchar_t *	wcsFromUnicode_alloc( size_t *wcslen ,const UChar *ucsS, const int32_t ucsLen) ;
size_t 		wcsFromUnicode( wchar_t **wcs, int32_t wcs_capacity,const UChar *ucsS, const int32_t ucsLen) ;
UChar 	* 	unicodeFromUTF8_alloc( int32_t *ucsLen, const char *utf8S ,const size_t utf8Len )  ;

