#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <iconv.h>

#include <btrace.h>

#include "ucs2utf8.h"

extern int ucs2_to_utf8(const char *ucs2,
			size_t ucs2len,
			char *utf8,
			size_t utf8len)
{
  /* UCS-2BE seems to work, so does UTF-16BE */

  iconv_t cd = iconv_open("UTF-8", "UCS-2BE");
  if (cd == (iconv_t)(-1)) return 1;

  size_t 
    ucs2_bytes_left = ucs2len, 
    utf8_bytes_left = utf8len,
    converted;

  converted = iconv(cd, (char**)&(ucs2), &(ucs2_bytes_left),
		    &(utf8), &(utf8_bytes_left));

  if (converted == (size_t)-1)
    {
      btrace("error in iconv: %s\n", strerror(errno));
      return 1;
    }

  if (iconv_close(cd) == -1)
    {
      btrace("error closing iconv descriptor: %s\n", strerror(errno));
      return 1;
    }

  return 0;
}
