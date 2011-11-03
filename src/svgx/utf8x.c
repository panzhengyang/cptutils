/*
  convert a utf8 multibyte string to ascii with
  character transliteration

  $Id$
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <iconv.h>
#include <locale.h>

#include "utf8ascii.h"

extern int utf8_to_ascii(const unsigned char *in, 
			 char *out,
			 size_t lenout)
{
  const char lname[] = "en_US.UTF-8";
  
  if (setlocale(LC_CTYPE,lname) == NULL)
    {
      fprintf(stderr,"failed to set locale to %s\n",lname);
      return 1;
    }

  iconv_t cv = iconv_open("ASCII//TRANSLIT","UTF-8");
  
  if (cv == (iconv_t)(-1))
    {
      fprintf(stderr,"error opening iconv descriptor: %s\n",
	      strerror(errno));
      return 1;
    }

  size_t lenin = strlen((const char*)in) + 1;

  if (iconv(cv, 
	    (char**)&(in), &(lenin), 
	    (char**)&(out), &(lenout)) == (size_t)-1)
    {
      fprintf(stderr,"error in iconv: %s\n",
	      strerror(errno));
      return 1;
    }

  if (iconv_close(cv) == -1)
    {
      fprintf(stderr,"error closing iconv descriptor: %s\n",
	      strerror(errno));
      return 1;
    }

  return 0;
}

