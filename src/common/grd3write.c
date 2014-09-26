/*
  grd3write.h

  writes paintshop pro gradients.
  2006 (c) J.J. Green
*/

#include <stdio.h>
#include <string.h>

#if defined HAVE_ENDIAN_H
#include <endian.h>
#elif defined HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#include "grd3write.h"

static int grd3_write_stream(FILE*,grd3_t*);

extern int grd3_write(const char* file,grd3_t* grd3)
{
  int err;

  if (file)
    {
      FILE* s;     

      if ((s = fopen(file, "w")) == NULL)
	return 1;

      err = grd3_write_stream(s, grd3);

      fclose(s);
    }
  else
    err = grd3_write_stream(stdout, grd3);

  return err;
}

static int write_initial_rgb(FILE*,grd3_rgbseg_t*);
static int write_rgb(FILE*,grd3_rgbseg_t*);
static int write_op(FILE*,grd3_opseg_t*);
static int write_zeros(FILE*,int);

static size_t ustrlen(const unsigned char* s)
{
  size_t n;

  for (n=0 ; *s ; s++) n++;

  return n;
}

static int grd3_write_stream(FILE *s,grd3_t *grd3)
{
  int i,n;
  unsigned short u[4];

  /* magic */

  fwrite(grd3magic,1,4,s);
  
  /* version */

  for (i=0 ; i<2 ; i++) u[i] = htobe16(grd3->ver[i]);
  fwrite(u,2,2,s);

  /* title : check non-null or get segfaults */

  if (grd3->name)
    {
      unsigned char len = ustrlen(grd3->name);

      fwrite(&len,1,1,s);
      fwrite(grd3->name,1,len,s);
    }
  else
    {
      const unsigned char name[] = "unspecified";
      unsigned char len = ustrlen(name);

      fwrite(&len,1,1,s);
      fwrite(name,1,len,s);
    }

  /* rgb gradient */

  n = grd3->rgb.n;

  u[0] = htobe16(n);
  fwrite(u,2,1,s);

  write_initial_rgb(s,grd3->rgb.seg);
  for (i=1 ; i<n ; i++) write_rgb(s,grd3->rgb.seg+i);

  write_zeros(s,2);

  /* opacity gradient */

  n = grd3->op.n;

  u[0] = htobe16(grd3->op.n);
  fwrite(u,2,1,s);

  for (i=0 ; i<n ; i++) write_op(s,grd3->op.seg+i);

  write_zeros(s,2);

  /* foot */

  write_zeros(s,1);

  return 0;
}

static int write_initial_rgb(FILE *s,grd3_rgbseg_t *seg)
{
  unsigned short u[8];

  u[0] = 0;
  u[1] = 0;
  u[2] = 0;
  u[3] = htobe16(seg->midpoint);
  u[4] = 0;
  u[5] = htobe16(seg->r);
  u[6] = htobe16(seg->g);
  u[7] = htobe16(seg->b);

  fwrite(u,2,8,s);

  return 0;
}

static int write_rgb(FILE *s,grd3_rgbseg_t *seg)
{
  unsigned short u[10];

  u[0] = 0;
  u[1] = 0;
  u[2] = 0;
  u[3] = htobe16(seg->z); 
  u[4] = 0;
  u[5] = htobe16(seg->midpoint); 
  u[6] = 0;
  u[7] = htobe16(seg->r);
  u[8] = htobe16(seg->g);
  u[9] = htobe16(seg->b);

  fwrite(u,2,10,s);

  return 0;
}

static int write_op(FILE *s,grd3_opseg_t *seg)
{
  unsigned short u[5];

  u[0] = 0;
  u[1] = htobe16(seg->z);
  u[2] = 0;
  u[3] = htobe16(seg->midpoint);
  u[4] = htobe16(seg->opacity);

  fwrite(u,2,5,s);

  return 0;
}

static int write_zeros(FILE *s,int n)
{
  int i;
  unsigned short u[n];

  for (i=0 ; i<n ; i++) u[i] = 0;

  fwrite(u,2,n,s);

  return 0;
}
