/*
  grd3read.c

  read paintshop pro gradients.
  2005 (c) J.J. Green
*/

#include <stdio.h>

#if defined HAVE_ENDIAN_H
#include <endian.h>
#elif defined HAVE_SYS_ENDIAN_H
#include <sys/endian.h>
#endif

#include "grd3read.h"

static int grd3_read_stream(FILE*, grd3_t*);

extern int grd3_read(const char* file, grd3_t* grd3)
{
  int err;

  if (file)
    {
      FILE* s;

      if ((s = fopen(file,"r")) == NULL)
	{
	  fprintf(stderr,"failed to open %s\n",file);
	  return 1;
	}

      err = grd3_read_stream(s,grd3);

      fclose(s);
    }
  else
    err = grd3_read_stream(stdin,grd3);

  return err;
}

static int read_first_rgbseg(FILE*,grd3_rgbseg_t*);
static int read_rgbseg(FILE*,grd3_rgbseg_t*);
static int read_opseg(FILE*,grd3_opseg_t*);
static int read_block_end(FILE*);

static int grd3_read_stream(FILE* s,grd3_t* grad)
{
  unsigned char b[6];
  unsigned short u[2];
  int i,n;

  /* first 4 are the grd3 magic number */

  if (fread(b,1,4,s) != 4)
    {
      fprintf(stderr,"failed to read magic\n");
      return 1;
    }

  for (i=0 ; i<4 ; i++)
    {
      if (b[i] != grd3magic[i])
	{
	  int j;

	  fprintf(stderr,"unexpected magic :");
	  for (j=0 ; j<4 ; j++) fprintf(stderr," %i",(int)b[j]);
	  fprintf(stderr,"\n");

	  return 1;
	}
    }

  /* next 2 shorts, a format version (usually 3 1) */

  if (fread(u,2,2,s) != 2)
    {
      fprintf(stderr,"failed to read version\n");
      return 1;
    }

  grad->ver[0] = be16toh(u[0]);
  grad->ver[1] = be16toh(u[1]);

  /* then a single unsigned char, the title length */

  n = fgetc(s);

  /* make a copy of the title, if feasible */

  if (n<0) 
    {
      fprintf(stderr,"title length is negative! (%i)\n",n);
      return 1;
    }
  else
    {
      unsigned char *name;
      int j;

      if ((name = malloc(n+1)) == NULL)
	{
	  fprintf(stderr,"failed malloc()");
	  return 1;
	}

      for (j=0 ; j<n ; j++)  name[j] = fgetc(s);

      name[n] = '\0';

      grad->name = name;
    }
    
  /* then an short, the number of rgb samples */

  if (fread(u,2,1,s) != 1)
    {
      fprintf(stderr,"failed to read number of RGB samples\n");
      return 1;
    }

  n = be16toh(u[0]);

  if (n<2)
    {
      fprintf(stderr,"too few samples (%i)\n",n);
      return 1;
    }
  else 
    {
      int j, err = 0;
      grd3_rgbseg_t* seg;

      if ((seg = malloc(n*sizeof(grd3_rgbseg_t))) == NULL)
	{
	  fprintf(stderr,"failed malloc()");
	  return 1;
	}
      
      err += read_first_rgbseg(s,seg);

      for (j=1 ; j<n ; j++) 
	err += read_rgbseg(s,seg+j);

      if (err)
	{
	  fprintf(stderr,"error reading rgb segments (%i)\n",err);
	  return err;
	}

      grad->rgb.n   = n;
      grad->rgb.seg = seg;
    }

  if (read_block_end(s) != 0)
    {
      fprintf(stderr,"error reading endblock\n");
      return 1;
    }

  /* then a short, the number of opacity samples */

  if (fread(u,1,2,s) != 2)
    {
      fprintf(stderr,"failed to read number of opacity samples\n");
      return 1;
    }

  n = be16toh(u[0]);

  if (n<2) 
    {
      fprintf(stderr,
	      "there are %i opacity stop%s (not enough)\n",
	      n,(n==1 ? "" : "s" ));
      return 1;
    }
  else 
    {
      int j, err = 0;
      grd3_opseg_t* seg;

      if ((seg = malloc(n*sizeof(grd3_opseg_t))) == NULL)
	{
	  fprintf(stderr,"failed malloc()");
	  return 1;
	}
      
      for (j=0 ; j<n ; j++) 
	err += read_opseg(s,seg+j);

      if (err)
	{
	  fprintf(stderr,"error reading op segments (%i)\n",err);
	  return err;
	}

      grad->op.n   = n;
      grad->op.seg = seg;
    }

  if (read_block_end(s) != 0)
    {
      fprintf(stderr,"error reading endblock\n");
      return 1;
    }

  return 0;
}

/*
  rgb segment is (shorts)

  0 0 
  0 z 
  0 m 
  0 r g b

  in all but the first, there the [0 z] block is 
  absent and z is implicitly zero

  In all files so far tested, the z value for the final
  segment is 4096 = 2^12 (16,0) and the redundant 
  midpoint 50.
*/

static int read_rgbseg_head(FILE*,grd3_rgbseg_t*);
static int read_rgbseg_midpoint(FILE*,grd3_rgbseg_t*);
static int read_rgbseg_z(FILE*,grd3_rgbseg_t*);
static int read_rgbseg_rgb(FILE*,grd3_rgbseg_t*);

static int read_first_rgbseg(FILE *s,grd3_rgbseg_t* seg)
{
  int err = 0;

  err += read_rgbseg_head(s,seg);
  err += read_rgbseg_midpoint(s,seg);
  err += read_rgbseg_rgb(s,seg);

  if (err) return err;

  seg->z = 0;

  return 0;
}

static int read_rgbseg(FILE *s,grd3_rgbseg_t* seg)
{
  int err = 0;

  err += read_rgbseg_head(s,seg);
  err += read_rgbseg_z(s,seg);
  err += read_rgbseg_midpoint(s,seg);
  err += read_rgbseg_rgb(s,seg);

  return err;
}

static int print_array(unsigned short* u,int n,FILE* s,const char* fmt)
{
  int i;

  for (i=0 ; i<n ; i++) fprintf(s,fmt,(int)be16toh(u[i]));

  return 0;
}

/* expect short[2] = 0 0, unused */ 

static int read_rgbseg_head(FILE *s,grd3_rgbseg_t* seg)
{
  unsigned short u[2];

  if (fread(u,2,2,s) != 2)
    {
      fprintf(stderr,"failed to read RGB segment header\n");
      return 1;
    }

  if (u[0] != 0)
    {
      fprintf(stderr,"unusual head found : ");
      print_array(u,2,stderr," %i");
      fprintf(stderr,"\n");

      return 1;
    }

  return 0;
}

/* 
   expect ushort[2] = 0 m 

   m is the midpoint in percentage, though the 
   grd3 gui seems to limit it to 5 < m < 95
*/

static int read_rgbseg_midpoint(FILE *s,grd3_rgbseg_t* seg)
{
  unsigned short u[2];

  if (fread(u,2,2,s) != 2)
    {
      fprintf(stderr,"failed to read RGB segment header\n");
      return 1;
    }

  if (u[0] != 0)
    {
      fprintf(stderr,"unusual midpoint block found : ");
      print_array(u,2,stderr," %i");
      fprintf(stderr,"\n");

      return 1;
    }

  seg->midpoint = be16toh(u[1]);

  return 0;
}

/* expect ushort[2] = 0 z  */  

static int read_rgbseg_z(FILE *s,grd3_rgbseg_t* seg)
{
  unsigned short u[2];

  if (fread(u,2,2,s) != 2)
    {
      fprintf(stderr,"failed to read RGB segment z-value\n");
      return 1;
    }

  if (u[0] != 0)
    {  
      fprintf(stderr,"unusual z pair found : ");
      print_array(u,2,stderr," %i");
      fprintf(stderr,"\n");

      return 1;
    }
  
  seg->z = be16toh(u[1]);

  return 0;
}

/* 
   expect ushort[4] = 0 r g b, the first short is
   unused (it is not an alpha channel), the next
   3 are the RGB values using the full 16 bits 
   range. To get the 8 bit value integer divide
   by 256
 */

static int read_rgbseg_rgb(FILE *s,grd3_rgbseg_t* seg)
{
  unsigned short u[4];

  if (fread(u,2,4,s) != 4)
    {
      fprintf(stderr,"failed to read RGB values\n");
      return 1;
    }

  if (u[0])
    {
      fprintf(stderr,
	      "unexpected rgb padding : %i\n",
	      (int)u[0]);
      return 1;
    }

  seg->r = be16toh(u[1]);
  seg->g = be16toh(u[2]);
  seg->b = be16toh(u[3]);

  return 0;
}

/* 
   expect ushort[5] = 0 z 0 m p  
   
   where

     z is the z-coordinate 0 <= z <= 16*256
     m is the mid-point    0 <= m <= 100
     p is the opacity      0 <= p <= 256 

   note that the opacity segments do not have a special
   initial form where the z is implicit, the first has
   z=0 and the last has z=4096=16*256

   The m in the final segment is unused (and usually 50).
*/

static int read_opseg(FILE *s,grd3_opseg_t* seg)
{
  unsigned short u[5];

  if (fread(u,2,5,s) != 5)
    {
      fprintf(stderr,"failed to read opacity segment\n");
      return 1;
    }

  if (u[0] || u[2])
    {
      fprintf(stderr,"unusual opacity segment:");
      print_array(u,2,stderr," %i");
      fprintf(stderr,"\n");
    }

  seg->z        = be16toh(u[1]);
  seg->midpoint = be16toh(u[3]);
  seg->opacity  = be16toh(u[4]);

  return 0;
}

static int read_block_end(FILE* s)
{
  unsigned short u[2];

  if (fread(u,2,2,s) != 2)
    {
      fprintf(stderr,"failed to read RGB segment header\n");
      return 1;
    }

  if (u[0] || u[1])
    {
      fprintf(stderr,"unepected block end : %i %i\n",
	      (int)u[0],(int)u[1]);
      return 1;
    }

  return 0;
}

#ifdef TESTPROG

/* test program */

int main (int argc,char** argv)
{
  FILE *s;
  char *file = argv[1];
  grd3_t grad;
  int i;

  if (argc != 2)
    {
      fprintf(stderr,"no file specified\n");
      return EXIT_FAILURE;
    }

  if ((s = fopen(file,"r")) == NULL)
    {
      fprintf(stderr,"failed to open %s\n",file);
      return EXIT_FAILURE;
    }

  if (read_grd3(s,&grad) != 0)
    {
      fprintf(stderr,"failed to read %s\n",file);
      return EXIT_FAILURE;
    }

  fclose(s);

  printf("# %s\n",grad.name);
  for (i=0 ; i<grad.n ; i++)
    {
      grd3_rgbseg_t seg = grad.seg[i];

      printf("%.4i %.3i %.3i %.3i\n",
	     seg.z,
	     seg.r,
	     seg.g,
	     seg.b);
    }

  return EXIT_SUCCESS;
}

#endif
