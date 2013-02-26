/* common.h
 * common headers for all the files in the project.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * common includes for the index project.
 */
#define _DARWIN_FEATURE_64_BIT_INODE
/* force this for the stat structure */
#pragma GCC diagnostic ignored "-Wconversion" 
#pragma GCC diagnostic ignored "-Wmissing-prototypes" 
#pragma GCC diagnostic ignored "-Wmissing-declarations" 
#pragma GCC diagnostic ignored "-Wstrict-prototypes" 
#include <libgen.h>
#include <sys/param.h>
#include <err.h>
#include <sys/errno.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <signal.h>
#include <getopt.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <xlocale.h> 
#include <ctype.h>
#include <wctype.h>
#include <wchar.h>
#define _XOPEN_SOURCE_EXTENDED
#include <curses.h>
#define U_CHARSET_IS_UTF8 1
#include <unicode/putil.h>
#include <unicode/uchar.h>
#include <unicode/uclean.h>
#include <unicode/ucol.h>
#include <unicode/umachine.h>
#include <unicode/uregex.h>
#include <unicode/ustdio.h>
#include <unicode/ustring.h>
#include <unicode/utf.h>
#include <unicode/utypes.h>
#include <assert.h>
#pragma GCC diagnostic warning "-Wconversion"
#pragma GCC diagnostic warning "-Wmissing-prototypes" 
#pragma GCC diagnostic warning "-Wmissing-declarations" 
#pragma GCC diagnostic warning "-Wstrict-prototypes" 
