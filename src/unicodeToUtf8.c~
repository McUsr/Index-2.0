/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4:ro
 *
 * unicdodeToUtf8: a small library for handling unicode text.
 * It relies totally on ICU's unicode library. see licence.text.
 *
 * Tommy Bollman/McUsr 2013.02.06
 */
#include <common.h>
#include <util.h>
#include <unicodeToUtf8.h>
/* Prøver å allokere buffer, men kan ikke se at det skal hjelpe.
 * når jeg skal finne lengden på bufferet jeg skal allokere.
 *
 * Det var noe snakk om å finne platform spesifik encoding.
 * Dette går altså ikke, og jeg kan ikke skjønne annet en at det er
 * fordi strengen ikke er en utf-8 streng. så det jeg får prøve å gjøre
 * er å konvertere strengen til widechar først.
 *
 *
 * */
#ifdef TESTING
int main(void)
{
	char *home;

	char *extra;

	UChar utf_home1[256];

	UChar utf_home2[256];

	char back[256];

	size_t slen = 0, slen2 = 0, buflen = 0;

	int32_t reslen = 0;

	char *loc = setlocale(LC_CTYPE, NULL);

	printf("Locale: %s\n", loc);
	home = getcwd(NULL, (size_t) 0);

	printf("min hjemme katalog er null terminert!\n %s\n", home);
	slen = strlen(home);
	printf("den har %d tegn!\n", (int)slen);
	printf("\nTEST1:utf8 til unicode\n");
	reslen = unicodeFromUTF8(utf_home1, 256, home, slen);

	if (reslen >= 0) {
		printf("lengden blir på %d\n", (int)reslen);
		u_printf("%S <--\n", utf_home1);
	} else
		printf("something erred!\n");

	printf("\nTEST2:unicode til utf8 \n");

	slen2 = utf8FromUnicode(back, (int32_t) 256, utf_home1, reslen);

	if (reslen >= 0) {
		printf("lengden blir på %d\n", (int)reslen);
		printf("%s <--\n", back);
	} else
		printf("something erred!\n");

	/* two steps for convering a string to wcs to unicode */
	wchar_t *buf = NULL;

	/* buf=mbstowcs_alloc(home) ; */
	buflen = mbstowcs_alloc2(&buf, home, slen);
	/* TODO: WARNING. comparision always true */
	if (buflen >= 0) {
		printf("\nTEST3: wcs to plain string: \n");
		printf("Has a valid wcs with a length of %d\n", (int)buflen);
		extra = wcstombs_alloc(buf);
		printf("-> %s\n", extra);

		/* tricket for å fastlå at dette virkelig virker: er selvsagt å foreta
		   den inverse testen! */
		printf("\nTEST3.5: unicode til wcs\n");
		/* vi stuffer utf_home2 med noe annet, og ser på resultatet. */
		U_STRING_INIT(utf_home2, "Quick-Fox 2", 11);
		u_printf("test 3.5  initalized with%S <--\n", utf_home2);

		reslen = unicodeFromWcs(utf_home2, 256, buf, buflen);

		if (reslen >= 0) {
			printf("\nTEST4: wcs til unicode\n");
			printf("lengden av unicode fra wcs blir på %d\n",
			       (int)reslen);
			u_printf("it now contains: %S <--\n", utf_home2);
			wcscpy(buf, (wchar_t *) "dummy");
			buflen = wcsFromUnicode(&buf, 256, utf_home2, reslen);

			/* TODO: WARNING. comparision always true */
			if (buflen >= 0) {
				printf("\nTEST4.5:unicode til wcs \n");
				printf("%ls\n", buf);
				printf("\nTEST5:unicode til wcs \n");
				printf
				    ("lengden av wcs fra unicode  blir på %d\n",
				     (int)buflen);
				free(extra);
				extra = wcstombs_alloc(buf);
				printf("wcs-> %s\n", extra);

				printf("wcs-> %s\n", extra);
			} else
				printf("something erred!\n");

		} else
			printf("something erred!\n");
	} else
		printf("something wrong with initial conversion to wcs\n");

	free(buf);
	free(home);
    int a=0;
    fscanf(stdin,"%d\n",a) ;
	return (0);
}

#endif
/* converts a unicode string to utf8, aka char * on Mac Os X  in order to
 * pass over information to the OS, or into files  from unicode. It stores
 * the string into reallocated memory it returns the length of the string
 * in characters, 0 if something went wrong.
 * This scheme is good, when we aren't allocating memory, but storing in a
 * preallocated buffer. */

size_t
utf8FromUnicode(char *s, int32_t s_capacity, const UChar * ucsS,
		const int32_t ucsLen)
{
	int32_t bfsz = 0;

	UErrorCode uic_ERR = U_ZERO_ERROR;

	assert(__APPLE__ == 1);
	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strToUTF8(NULL, 0, &bfsz, ucsS, ucsLen, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR)
		return 1;
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	(void)u_strToUTF8(s, s_capacity, &bfsz, ucsS, ucsLen, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR)
		return 0;
	return strlen(s);
}
/* creates a lowercase unicode string from a wchar_t string, dies if something goes wrong */
UChar *lowercasedUString(wchar_t *wcsS)
{

	UChar *ucs_convertedStr = NULL;
    size_t ucs_len= 0 ; 
	ucs_convertedStr = unicodeFromWcs_alloc((size_t *) & ucs_len, wcsS);
	if (ucs_convertedStr == NULL)
		memerrmsg("lowercasedUString:ucs_convertedStr==NULL");
	int32_t uchBaseLen = u_strlen(ucs_convertedStr);
	/* lowercasing may cause more characters.               */
	UChar *lowerChosenBase = ymalloc((size_t)( uchBaseLen * 2 + 1 ) * sizeof(UChar),
        "lowercasedUString","lowerChosenBase");

	UErrorCode status = U_ZERO_ERROR;

	 u_strToLower(lowerChosenBase, (int32_t) (uchBaseLen * 2),
				    (UChar *) ucs_convertedStr, uchBaseLen,
				    getMyLocale(), &status);

	free(ucs_convertedStr);
	ucs_convertedStr = NULL;

	if (status != U_ZERO_ERROR) {
		fprintf(stderr,
			"lowercasedUString: error during lowercase of lowerChosenBase.\n");
		free(lowerChosenBase);
		exit(1);
	}
    return  lowerChosenBase ;
}

/* returns the utf8 (multibyte string) from a unicode string 
   NULL if something went wrong. the string must be freed afterwards.
*/
char * 
utf8StringFromUnicode( UChar *ucsS ) 
{
    int32_t ucs_len = u_strlen(ucsS);
    size_t slen = 0;
    char *t = NULL;
    t = utf8FromUnicode_alloc( &slen, ucsS, ucs_len) ;
    if ( slen == 0 ) {
        free(t) ;
        t=NULL ;
    }
    return t ;
}
/* converts a unicode string to utf8, aka char * on Mac Os X  in order to
 * pass over information to the OS, or into files  from unicode. It stores
 * the converted string into  memory it allocates that must be subsequently 
 * freed. it returns the length of the string * in characters, in slen,
 * 0 if something went wrong. 
 * PARAMETERS:
 *      slen:       the holder of the string length
 *      ucsS:       the string to convert
 *      ucsLen :    the length of the string to convert.
 */
char *
utf8FromUnicode_alloc(size_t * slen, const UChar * ucsS,
			    const int32_t ucsLen)
{
	char *buf;

	int32_t sCap = (int32_t) BUFSIZ * sizeof(char);

	int32_t bfsz = 0;

	assert(__APPLE__ == 1);
	buf = (char *)ymalloc((size_t) BUFSIZ,"utf8FromUnicode_alloc","buf");
	/* Make buffer to store converted utf-16 line in    */

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strToUTF8(NULL, 0, &bfsz, ucsS, ucsLen, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR)
		return NULL;
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	buf = u_strToUTF8(buf, sCap, &bfsz, ucsS, ucsLen, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR)
		return NULL;
	if (buf == NULL)
		return NULL;
	buf = (char *)yrealloc(buf, (size_t) (bfsz * sizeof(char)),"utf8FromUnicode_alloc","buf");
	/* shrink to fit */
	if (buf == NULL)
		return NULL;
	*slen = (size_t) bfsz;
	return buf;
}

/* returns the  wcs_string from a unicode string 
   NULL if something went wrong. the string must be freed afterwards.
*/
wchar_t * 
wcsStringFromUnicode( UChar *ucsS ) 
{
    int32_t ucs_len = u_strlen(ucsS);
    size_t slen = 0;
    wchar_t *t = NULL;
    t = wcsFromUnicode_alloc( &slen, ucsS, ucs_len) ;
    if ( slen == 0 ) {
        free(t) ;
        t=NULL ;
    }
    return t ;
}
/* for usage with static buffers only!! */
wchar_t *
wcsFromUnicode_alloc(size_t * wcs_len, const UChar * ucsS,
			      const int32_t ucsLen)
{

	size_t dummy = 0;

	char *buf = NULL;

	wchar_t *wcsbuf=NULL;

	assert(__APPLE__ == 1);
	/* Size of Apple's wide chars is 32 bit, it doesn't play well with ICU      */
	/* so this code needs a rewrite. where your platform is added.              */
	/* Please push your changes to the repository!                              */
	buf = utf8FromUnicode_alloc(&dummy, ucsS, ucsLen);
	if (buf == NULL)
		return NULL;

	wcsbuf = mbstowcs_alloc(buf);
	if (wcsbuf == NULL)
		return NULL;
	free(buf);

	if (wcsbuf == NULL)
		return NULL;
	*wcs_len = wcslen(wcsbuf);

	return wcsbuf;
}

/* for usage with static buffers only!! */
size_t
wcsFromUnicode(wchar_t ** wcs, int32_t wcs_capacity, const UChar * ucsS,
	       const int32_t ucsLen)
{
	size_t size = ((wcs_capacity * 4) * sizeof(char) + 1);

	char *buf = NULL;

	assert(__APPLE__ == 1);
	/* Size of Apple's wide chars is 32 bit, it doesn't play well with ICU      */
	/* so this code needs a rewrite. where your platform is added.              */
	/* Please push your changes to the repository!                              */
	buf = (char *)ymalloc(size,"wcsFromUnicode","buf");
	size_t slen = utf8FromUnicode(buf, (int32_t) size, ucsS, ucsLen);

	if (slen < 1) {
		free(buf);
		return 0;
	}
	int32_t bfsz = mbstowcs_alloc2(wcs, buf, slen);

	free(buf);
	if (slen < 1) {
		return 0;
	}
	return bfsz;
}

/* converts a wide character string to a multibyte string.
 * You have to allocate memory yourself: Use with care! */
size_t
wcstombs_alloc2(char **mbs, const wchar_t * wcsS, const size_t wcsLength)
{
	size_t size = wcsLength * 4 + 1;

	*mbs = ymalloc(size * sizeof(char),"wcstombs_alloc2","mbs");
	size = wcstombs(*mbs, wcsS, size);
	if (size == (size_t) 0)
		return (size_t) 0;
	return strlen(*mbs);
}

/* Converst  a multi-byte (UTF8 string to a wide character.
 * You have to allocate memory yourself: Use with care! */
size_t
mbstowcs_alloc2(wchar_t ** buf, const char *string, size_t slen)
{
	size_t size = (slen + 1) * sizeof(wchar_t);

	*buf = (wchar_t *) ymalloc(size,"mbstowcs_alloc2","buf");
	size = mbstowcs(*buf, string, size);
	if (size == (size_t) 0)
		return 0;
	return wcslen(*buf);
}

/* function that returns an allocated wchar array, filled with converted str    */
wchar_t *
mbstowcs_alloc(const char *string)
{
	size_t slen = strlen(string);

	wchar_t *buf = ymalloc(((slen + 1) * sizeof(wchar_t)),"mbstowcs_alloc","buf") ;
	size_t wcs_len = mbstowcs(buf, string, slen);

	if (wcs_len == (size_t) 0) {
		free(buf);
		return NULL;
	}
	buf[wcs_len] = (wchar_t) '\0';
	buf = yrealloc(buf, (wcs_len * sizeof(wchar_t)),"mbstowcs_alloc","buf");
	return buf;
}

/* converts a wide character string to a multibyte string.                      */
/* Det hadde sett pent ut med code points her.                                  */
char *
wcstombs_alloc(const wchar_t * wcsS)
{
	size_t wcs_len = wcslen(wcsS);

	size_t bufsz = wcs_len * 4 + 1;

	/* One can need four chars for one codepoint in wcs.    */
	char *buf = (char *)ymalloc(bufsz * sizeof(char),"wcstombs_alloc","buf");

	assert(__APPLE__ == 1);
	size_t slen = wcstombs(buf, wcsS, bufsz);

	if (slen == (size_t) - 1) {
		fprintf(stderr, "wcstombs_alloc: failure during conversion\n");
		free(buf);
		return NULL;
	}
	buf[slen] = '\0';
	buf = (char *) yrealloc(buf, ((slen + 1) * sizeof(char)),"wcstobms_alloc","buf");
	return buf;
}
/* returns a unicode string converted from a wchar_t string.
   it takes a pointer to a size_t variable, as a cointainer for
   the length of the converted string (uchlen).
   wcsS is the wchar_t * pointer  that is to be converted.
   
*/
UChar *
unicodeFromWcs_alloc(size_t * uchlen, const wchar_t * wcsS)
{
	char *buf = wcstombs_alloc(wcsS);

	if (buf == NULL)
		return NULL;
	size_t slen = strlen(buf);

	assert(__APPLE__ == 1);
	/* Size of Apple's wide chars is 32 bit, it doesn't play well with ICU      */
	/* so this code needs a rewrite. where your platform is added.              */
	/* Please push your changes to the repository!                              */
	UChar *uchbuf = (UChar *) ymalloc(((slen + 1) * sizeof(UChar)),"unicodeFromWcs_alloc","uchbuf");
    memset(uchbuf,(int) 0,(slen + 1 * sizeof(UChar)));
	/* have to convert back to multibyte string before converting to unicode!
	   uses the assumption that the mbs is utf8, and that locale has been set
	   in order for this to work!                                               */
	int32_t bfsz = 0;

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strFromUTF8(NULL, 0, &bfsz, buf, (int32_t) slen, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR) {
	/* if we didn't get an overflow error during preflight, then its an error!  */
		fprintf(stderr,
			"unicodeFromWcs_alloc u_strFromUTF8 PREFLIGHT errcode = %d\n",
			(int)uic_ERR);
		free(buf);
		free(uchbuf);
		return NULL;
	}

	uic_ERR = U_ZERO_ERROR;
	uchbuf =
	    u_strFromUTF8(uchbuf, (int32_t) (slen + 1), &bfsz, buf,
			  (int32_t) slen, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR) {
		fprintf(stderr,
			"unicodeFromWcs_alloc u_strFromUTF8 errcode = %d\n",
			(int)uic_ERR);
		return NULL;
	}
	free(buf);
	uchbuf = (UChar *) yrealloc(uchbuf, ((bfsz + 1) * sizeof(UChar)),"unicodeFromWcs_alloc","uchbuf");
	uchbuf[bfsz] = (UChar) '\0';
	*uchlen = bfsz;
	return uchbuf;
}

/* converts a wide character string (wcs) into a unicode string.
 * then stores the string  into  preallocated memory.
 * It returns the length in codepoints if the encoding succeeded, 0 otherwise.  */
int32_t
unicodeFromWcs(UChar * uic_s, const int32_t uic_capacity, const wchar_t * wcsS,
	       const size_t wcsLen)
{
	int32_t bfsz = 0;

	/* Please push your changes to the repository!                              */
	char *extra;

	int32_t slen = (int32_t) wcstombs_alloc2(&extra, wcsS, wcsLen);

	assert(__APPLE__ == 1);
	/* Size of Apple's wide chars is 32 bit, it doesn't play well with ICU      */
	/* so this code needs a rewrite. where your platform is added.              */
	if (slen < 0)
		return 0;
	/* have to convert back to multibyte string before converting to unicode!
	   uses the assumption that the mbs is utf8, and that locale has been set
	   in order for this to work!                                               */

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strFromUTF8(NULL, 0, &bfsz, extra, slen, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR) {
		free(extra);
		return 0;
	}

	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	(void)u_strFromUTF8(uic_s, (int32_t) uic_capacity, &bfsz, extra, slen,
			    &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	free(extra);

	if (uic_ERR != U_ZERO_ERROR) {
		return 0;
	}
	return u_strlen(uic_s);
}

/* converts an utf8 encoded "multibyte string" aka a char *s in UTF-8 from  OS
 * function, or file, and **encodes it into a unicode string**, then stores the
 * string * into  preallocated memory.
 * It returns the length in codepoints if the encoding succeeded, 0 otherwise.
 */
int32_t
unicodeFromUTF8(UChar * uic_s, const int32_t uic_capacity, const char *utf8S,
		const size_t utf8Len)
{
	int32_t bfsz = 0;

	UErrorCode uic_ERR = U_ZERO_ERROR;

	assert(__APPLE__ == 1);
	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strFromUTF8(NULL, 0, &bfsz, utf8S, (int32_t) utf8Len, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR)
		return 0;
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	(void)u_strFromUTF8(uic_s, (int32_t) uic_capacity, &bfsz, utf8S,
			    (int32_t) utf8Len, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR)
		return 0;
	return u_strlen(uic_s);
}

/* converts an utf8 encoded "multibyte string" aka a char *s in UTF-8 from  OS
 * function, or file, and encodes it into a unicode string.
 * Allocates memory, returns NULL upon error.
 */
UChar *
unicodeFromUTF8_alloc(int32_t * ucsLen, const char *utf8S,
			     const size_t utf8Len)
{
	UChar *buf;

	int32_t uchCap = ((int32_t) utf8Len + 1) * sizeof(UChar);

	int32_t bfsz = 0;

	assert(__APPLE__ == 1);
	buf = (UChar *) ymalloc((size_t) uchCap,"unicodeFromUTF8_alloc","buf");
	/* Make buffer to store converted utf-16 line in    */

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strFromUTF8(NULL, 0, &bfsz, utf8S, (int32_t) utf8Len, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR)
		return NULL;
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	buf =
	    u_strFromUTF8(buf, uchCap, &bfsz, utf8S, (int32_t) utf8Len,
			  &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR)
		return NULL;

	buf = (UChar *) yrealloc(buf, (size_t) (bfsz * sizeof(UChar)),"unicodeFromUTF8_alloc","buf");

	*ucsLen = bfsz;
	return buf;
}
