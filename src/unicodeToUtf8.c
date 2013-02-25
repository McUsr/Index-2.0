/* vim: nospell:fenc=utf-8:ft=c:et:sw=4:ts=4:sts=4:ro

  unicdodeToUtf8: a small library for handling unicode text.  It relies
  totally on ICU's unicode library. see licence.text.

  The calulations here are meant to be valid cross-platform rather than
  taking any assumptions about the size of wchar_t,
  (http://en.wikipedia.org/wiki/Wide_character) and trying to guess
  what kind of encoding they are encoded into (how many bits, and what
  kind of encoding the resulting code-points are encoded with.)

  The conversions are supposed to be reliable on the different platforms,
  rather than being memory efficient. Since we don't leak that much memory
  at all, (80 bytes or so, once, during startup) most of the excess
  memory will be freed immediately; the data are stored internally in
  utf16, which should use the same amount of bytes on all platforms,
  at all times. The conversions takes place when reading and writing to
  disk and stdout, furthermore  representing them on screen with Ncurses
  one record at a time. The excess memory are then instantly freed when
  you are done editing, or browsing to the next record.

  I also want to state that reallocation is done asap as the conversion
  is done. 

 The rules for allocating buffers converting from one encoding to another
 ========================================================================

 I call wchar_t just wchar_t and not by any encoding, since I think the
 encoding *may* vary with the size of the wchar_t which spans from at least
 one to four bytes. (see reference to Wikipedia article above.)

 I use the term unit, to specify a storage unit below. For instance a
 byte in a multibyte sequence is a unit, since I count those units in order
 to allocate space to hold them, a unit is the number you get when you use
 the accompanying length function to determine the length of an array of them,
 which for the multibute would be strlen of <string.h>, wcslen of <wchar.h>
 and u_strlen of <unicode/ustring.h>.

 From multibyte (UTF-8) to wide character (wchar_t)
 ------------------------------------------

 A string of wide characters will never contain more units than a multibyte
 string so the mapping is 1:1.

 From wchar_t to multibyte
 -------------------------

 The multibyte representation in units (utf-8) can be at most 4-times larger
 so we allocate space for 4-times the number of wch_char units.
 The mapping is 4:1

 From wchar_t to UChar (16-bit UTF-16)
 ------------------------------------

Is never performed directly, as I converted through multi-byte, this
calculation is here for completion.

I allocate 4 times the size of the wchar_t string in units, to hold the
converted result.

 From UChar to wchar_t
 ---------------------

Is never performed directly, as I converted through multi-byte, this
calculation is here for completion.

I allocate 4 times the size of the UChar string in units, to hold the
converted result.

 From multibyte to UChar
 ----------------------

As with multibyte to wchar_t, I think there will never be more UChar
units than multibyte units so a buffer with the same number of units
are allocated.

 From UChar to multibyte 
 -----------------------

 At least for some values, I guess that 4 bytes are needed to represent
 one UChar value so we allocate 4 times the number of UChar units in
 the UChar string.


 Again:
 ------

 The size of wchar_t in bytes are never considered, nor what kind of
 encoding that is used for them, The excess memory usage are for a
 limited time, and for the comfort of any that wants to port Index to
 their platform, without having to take platform specific calculations
 on other platforms into account.

If you want to optimize this for your platform, then you are of course
free to do so, and responsible for the results.

  Tommy Bollman/McUsr 2013.02.25

 */
#include <common.h>
#include <defs.h>
#include <util.h>
#include <unicodeToUtf8.h>
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
/*
Converts a unicode string to utf8, aka char * on Mac Os X  in order to
pass over information to the OS, or into files  from unicode. It stores
the string into reallocated memory it returns the length of the string
in characters, 0 if something went wrong. 
nb! uses preallocated memory -> no reallocation!
 */

size_t
utf8FromUnicode(char *s, int32_t s_capacity, const UChar * ucsS,
		const int32_t ucsLen)
{
    static const char procname[]="utf8FromUnicode" ;
    static const char sname[]="s" ;
	int32_t bfsz = 0;

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strToUTF8(NULL, 0, &bfsz, ucsS, ucsLen, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR) {
        y_icuerror(YICU_CNVPRETUTF8_ERR ,procname,"NULL", uic_ERR ); 
    }
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	(void)u_strToUTF8(s, s_capacity, &bfsz, ucsS, ucsLen, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR) {
        y_icuerror(YICU_CNVTUTF8_ERR ,procname,sname, uic_ERR ); 
     }
     size_t slen =strlen(s) ;
	return slen;
}
/*
creates a lowercase unicode string from a wchar_t string,
dies if something goes wrong */
UChar *lowercasedUString(wchar_t *wcsS)
{
    static const char procname[]="lowercasedUString" ;
    static const char lowerbasename[]="lowerChosenBase" ;

	UChar *ucs_convertedStr = NULL;
    size_t ucs_len= 0 ; 
	ucs_convertedStr = unicodeFromWcs_alloc((size_t *) & ucs_len, wcsS);
	int32_t uchBaseLen = u_strlen(ucs_convertedStr);
	/* lowercasing may cause more characters. Doubling the amount should suffice. */
    UChar *lowerChosenBase = ymalloc((size_t)( uchBaseLen * 2 + 1 ) * sizeof(UChar),
        procname,lowerbasename);

	UErrorCode status = U_ZERO_ERROR;

	 u_strToLower(lowerChosenBase, (int32_t) (uchBaseLen * 2),
				    (UChar *) ucs_convertedStr, uchBaseLen,
				    getMyLocale(), &status);

	free(ucs_convertedStr);
	ucs_convertedStr = NULL;

	if (status != U_ZERO_ERROR) {
		free(lowerChosenBase);
        y_icuSimpleError(
            "Index: lowercasedUString: error during lowercase of lowerChosenBase.",status);
	}
	lowerChosenBase = (char *)yrealloc(lowerChosenBase, u_strlen(lowerChosenBase) ,
        procname,lowerbasename);
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
/*
  Converts a unicode string to utf8 (char), in order to  pass over information
  to the OS, or into files  from unicode. It stores the converted string
  into  memory it allocates. That memory  must be subsequently freed.

  Returns: the length of the string * in characters, in slen,
  0 if something went wrong. 
  PARAMETERS:
 *      slen:       the holder of the string length
 *      ucsS:       the string to convert
 *      ucsLen :    the length of the string to convert.
 */
char *
utf8FromUnicode_alloc(size_t * slen, const UChar * ucsS,
			    const int32_t ucsLen)
{
    /* TODO: burde ha releasing av buffere som virker,
    problem er at det site allocerte minnet ikke blir frigitt */
    const char procname[]="utf8FromUnicode_alloc" ;
    const char bufname[]="buf" ;
	char *buf;

	/* int32_t sCap = (int32_t) BUFSIZ * sizeof(char); */
	int32_t sCap = ucsLen * 4;

	int32_t bfsz = 0;

	buf = (char *)ymalloc((size_t) (ucsLen +1 ),procname,bufname);
	/* Make buffer to store converted utf-16 line in    */

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strToUTF8(NULL, 0, &bfsz, ucsS, ucsLen, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR) {
        y_icuerror(YICU_CNVFUTF8_ERR,procname,bufname, uic_ERR ); 
    }
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	buf = u_strToUTF8(buf, sCap, &bfsz, ucsS, ucsLen, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR !=U_ZERO_ERROR) {
        y_icuerror(YICU_CNVFUTF8_ERR,procname,bufname, uic_ERR ); 
    }
	buf = (char *)yrealloc(buf, (size_t) bfsz ,procname,bufname);
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
    *wcs_len = 0 ;
	buf = utf8FromUnicode_alloc(&dummy, ucsS, ucsLen);
	if (buf == NULL)
		return NULL;

	wcsbuf = mbstowcs_alloc(buf);
	if (wcsbuf == NULL)
		return NULL;
	free(buf);
	*wcs_len = wcslen(wcsbuf);

	return wcsbuf;
}

/*
   Converts unioce to wcs , returns number of chars.
   needs the capacity to hold wcs_in the UChar string and its size.
 */
size_t
wcsFromUnicode(wchar_t ** wcs, int32_t wcs_capacity, const UChar * ucsS,
	       const int32_t ucsLen)
{
	size_t size = ((wcs_capacity * 4) * sizeof(char) + 1);

	char *buf = NULL;

    /* converting from unciode, to char (multibyte before ending up with
       wcs, may seem laborius, but it should work at all times */
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

/* Routine that returns  the length of a wcs string converted from a
   multi-byte (UTF8 string ) Dies if it met an illegal byte sequence.
*/
size_t
mbstowcs_alloc2(wchar_t ** buf, const char *string, size_t slen)
{
    static const char procname[]="mbstowcs_alloc2" ;
    static const char bufname[]="buf" ;
	size_t wcsize = (slen + 1) * sizeof(wchar_t);

	*buf = (wchar_t *) ymalloc(wcsize,procname,bufname);
	wcsize = mbstowcs(*buf, string, wcsize);
	if (wcsize == (size_t) -1) {
		free(buf);
        ysimpleError("Index: mbstowcs_alloc2: failure during conversion.",YX_EXTERNAL_CAUSE);
	}
	*buf = yrealloc(*buf, wcsize ,procname,bufname);
	return wcsize; /* was wcslen(*buf) */
}

/* function that returns an allocated wchar array, filled with the
   converted string. Dies if it met an illegal byte sequence. 
*/
wchar_t *
mbstowcs_alloc(const char *string)
{
    static const char procname[]="mbstowcs_alloc" ;
    static const char bufname[]="buf" ;
	size_t slen = strlen(string);

	wchar_t *buf = ymalloc(((slen + 1) * sizeof(wchar_t)),procname,bufname) ;
	size_t wcs_len = mbstowcs(buf, string, slen);

	if (wcs_len == (size_t) -1) {
		free(buf);
        ysimpleError("Index: mbstowcs_alloc: failure during conversion.",YX_EXTERNAL_CAUSE);
	}
	buf[wcs_len] = (wchar_t) '\0';
	buf = yrealloc(buf, (wcs_len * sizeof(wchar_t)),procname,bufname);
	return buf;
}
/* TODO: realloc all over! */
/* converts a wide character string to a multibyte string.                      */
char *
wcstombs_alloc(const wchar_t * wcsS)
{
    static const char procname[]="wcstombs_alloc" ;
    static const char bufname[]="buf" ;
	size_t wcs_len = wcslen(wcsS);

	size_t bufsz = wcs_len * 4 + 1;

	/* One can need four chars for one codepoint in wcs.    */
	char *buf = (char *)ymalloc(bufsz * sizeof(char),procname,bufname);

	size_t slen = wcstombs(buf, wcsS, bufsz);

	if (slen == (size_t) - 1) {
		free(buf);
        ysimpleError("Index: wcstombs_alloc: failure during conversion.",YX_EXTERNAL_CAUSE);
	}
	buf[slen] = '\0';
	buf = (char *) yrealloc(buf, ((slen + 1) * sizeof(char)),procname,bufname);
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
    const char procname[]="unicodeFromWcs_alloc" ;
    const char uchbufname[]="uchbuf" ;
	char *buf = wcstombs_alloc(wcsS);
    

	if (buf == NULL)
		return NULL;
	size_t slen = strlen(buf);

	UChar *uchbuf = (UChar *) ymalloc(((slen + 1) * sizeof(UChar)),procname,uchbufname);
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
		free(buf);
		free(uchbuf);
        y_icuerror(YICU_CNVPREUTF8_ERR,procname,uchbufname, uic_ERR ); 
	}

	uic_ERR = U_ZERO_ERROR;
	uchbuf =
	    u_strFromUTF8(uchbuf, (int32_t) (slen + 1), &bfsz, buf,
			  (int32_t) slen, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR) {
		free(buf);
		free(uchbuf);
        y_icuerror(YICU_CNVFUTF8_ERR,procname,uchbufname, uic_ERR ); 
	}
	free(buf);
	uchbuf = (UChar *) yrealloc(uchbuf, ((bfsz + 1) * sizeof(UChar)),procname,uchbufname);
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
    static const char procname[]="unicodeFromWcs" ;
	int32_t bfsz = 0;

	/* Please push your changes to the repository!                              */
	char *extra;

	int32_t slen = (int32_t) wcstombs_alloc2(&extra, wcsS, wcsLen);

	/* have to convert back to multibyte string before converting to unicode!
	   uses the assumption that the mbs is utf8, and that locale has been set
	   in order for this to work!                                               */

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strFromUTF8(NULL, 0, &bfsz, extra, slen, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR) {
		free(extra);
        y_icuerror(YICU_CNVFUTF8_ERR,procname,"NULL", uic_ERR ); 
	}

	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	(void)u_strFromUTF8(uic_s, (int32_t) uic_capacity, &bfsz, extra, slen,
			    &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	free(extra);

	if (uic_ERR != U_ZERO_ERROR) {
        y_icuerror(YICU_CNVFUTF8_ERR,procname,"uic_s", uic_ERR ); 
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
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR) {
        y_icuerror(YICU_CNVFUTF8_ERR,"unicodeFromUTF8","NULL", uic_ERR ); 
    }
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	(void)u_strFromUTF8(uic_s, (int32_t) uic_capacity, &bfsz, utf8S,
			    (int32_t) utf8Len, &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR) {
        y_icuerror(YICU_CNVFUTF8_ERR,"unicodeFromUTF8","uic_s", uic_ERR ); 
    }
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
    const char procname[]="unicodeFromUTF8_alloc" ;
    const char bufname[] ="buf" ;
	UChar *buf;

	int32_t uchCap = ((int32_t) utf8Len + 1) * sizeof(UChar);

	int32_t bfsz = 0;

	assert(__APPLE__ == 1);
	buf = (UChar *) ymalloc((size_t) uchCap,procname,bufname);
	/* Make buffer to store converted utf-16 line in    */

	UErrorCode uic_ERR = U_ZERO_ERROR;

	/* preflight, we determine the size of the buffer we need by encoding it.   */
	(void)u_strFromUTF8(NULL, 0, &bfsz, utf8S, (int32_t) utf8Len, &uic_ERR);
	if (uic_ERR != U_BUFFER_OVERFLOW_ERROR) {
        y_icuerror(YICU_CNVFUTF8_ERR,procname,"NULL", uic_ERR ); 
    }
	uic_ERR = U_ZERO_ERROR;
	/* if we didn't get an overflow error during preflight, then its an error!  */
	buf =
	    u_strFromUTF8(buf, uchCap, &bfsz, utf8S, (int32_t) utf8Len,
			  &uic_ERR);
	/* We perform the conversion to unicode into a preallocated buffer.         */
	if (uic_ERR != U_ZERO_ERROR) {
        y_icuerror(YICU_CNVFUTF8_ERR,procname,"buf", uic_ERR ); 
    }

	buf = (UChar *) yrealloc(buf, (size_t) (bfsz * sizeof(UChar)),procname,bufname);

	*ucsLen = bfsz;
	return buf;
}
