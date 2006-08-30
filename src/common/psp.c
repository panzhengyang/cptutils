/*
  psp.c

  read paintshop pro gradients.
  2005 (c) J.J. Green
  $Id: psp.c,v 1.5 2006/08/28 22:14:39 jjg Exp jjg $
*/

/* TODO : check fread return values */

#if 0
#define DEBUG_OPGRAD
#define DEBUG_H1
#endif

#include <stdio.h>

#include "psp.h"

#define SKIP(stream,n) fseek((stream),(n),SEEK_CUR)

static int read_first_rgbseg(FILE*,psp_rgbseg_t*);
static int read_rgbseg(FILE*,psp_rgbseg_t*);
static int read_opseg(FILE*,psp_opseg_t*);

extern int read_psp(FILE* s,psp_grad_t* grad)
{
  unsigned char b[6],magic[4] = "8BGR";
  int i,n;

  /* first 4 look like a magic number 8BGR */

  fread(b,1,4,s);

  for (i=0 ; i<4 ; i++)
    {
      if (b[i] != magic[i])
	{
	  int j;

	  fprintf(stderr,"unexpected magic :");
	  for (j=0 ; j<4 ; j++) fprintf(stderr," %i",(int)b[j]);
	  fprintf(stderr,"\n");

	  return 1;
	}
    }

  /* next 4 looks like a format version - 00 03 00 01 -> 3.1 ? */

  fread(b,1,4,s);

  grad->ver[0] = b[0]*256 + b[1];
  grad->ver[1] = b[2]*256 + b[3];

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
      char *name;
      int j;

      if ((name = malloc(n+1)) == NULL)
	{
	  fprintf(stderr,"failed malloc()");
	  return 1;
	}

      for (j=0 ; j<n+1 ; j++)  name[j] = fgetc(s);

      grad->name = name;
    }
    
  /* then an unsigned char, the number of rgb samples */

  n = fgetc(s);

#ifdef DEBUG_H1
  printf("rgb gradient : %i stops\n",n);
#endif

  if (n<2)
    {
      fprintf(stderr,"too few samples (%i)\n",n);
      return 1;
    }
  else 
    {
      int j, err = 0;
      psp_rgbseg_t* seg;

      if ((seg = malloc(n*sizeof(psp_rgbseg_t))) == NULL)
	{
	  fprintf(stderr,"failed malloc()");
	  return 1;
	}
      
      err += read_first_rgbseg(s,seg);

      for (j=1 ; j<n ; j++) 
	err += read_rgbseg(s,seg+j);

      if (err) return err;

      grad->rgb.n   = n;
      grad->rgb.seg = seg;
    }

  /* then 0 0 0 0 0 n, where n is the number of opacity samples */

  fread(b,1,6,s);

  for (i=0 ; i<5 ; i++)
    {
      if (b[i] != 0)
	{
	  int j;

	  fprintf(stderr,"unexpected opacity gradient header : ");
	  for (j=0 ; j<6 ; j++) fprintf(stderr," %i",(int)b[j]);
	  fprintf(stderr,"\n");

	  return 1;
	}
    }

   /* 
      check n feasible: 2 are needed, and there are only 16
      possible z values so more than 31 stops would be madness
      (think a 16-step staircase)
   */

  n = b[5];

#ifdef DEBUG_OPGRAD
  printf("opacity gradient: %i stops\n",n);
#endif

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
      psp_opseg_t* seg;

      if (n>31)
	{
	  fprintf(stderr,
		  "there are %i opacity stop%s (probably too many)\n",
		  n,(n==1 ? "" : "s" ));
	}

      if ((seg = malloc(n*sizeof(psp_opseg_t))) == NULL)
	{
	  fprintf(stderr,"failed malloc()");
	  return 1;
	}
      
      for (j=0 ; j<n ; j++) 
	err += read_opseg(s,seg+j);

      if (err) return err;

      grad->op.n   = n;
      grad->op.seg = seg;
    }

  return 0;
}

extern int clean_psp(psp_grad_t* grad)
{
  free(grad->name);
  free(grad->rgb.seg);
  free(grad->op.seg);

  return 0;
}


/*
  In all but the first rgbseg, the header seems to be

    0 0 0 0  : h1 
    0 0 x y  : z
    0 0 0 m  : midpoint

  with (x,y) interpreted as the z-value, we take it to 
  mean a big endian unsigned short, so (x,y) -> 256*x+y.
  The midpoint is an unsigned char which should be 
  intepreted as a percentage (of the )

  In the first segment the z block is absent and z is 
  implicitly zero. So the data are

    0 m z m z m
    --- --- ---
 
  i.e., the midpoint of the final segment is redundant,

  In all files so far tested, the z value for the final
  segment is 4096 = 2^12 (16,0) and the redundant 
  midpoint 50.
*/

static int read_rgbseg_head(FILE*,psp_rgbseg_t*);
static int read_rgbseg_midpoint(FILE*,psp_rgbseg_t*);
static int read_rgbseg_z(FILE*,psp_rgbseg_t*);
static int read_rgbseg_rgb(FILE*,psp_rgbseg_t*);

static int read_first_rgbseg(FILE *s,psp_rgbseg_t* seg)
{
  int err = 0;

  err += read_rgbseg_head(s,seg);
  err += read_rgbseg_midpoint(s,seg);
  err += read_rgbseg_rgb(s,seg);

  if (err) return err;

  seg->z = 0;

  return 0;
}

static int read_rgbseg(FILE *s,psp_rgbseg_t* seg)
{
  int err = 0;

  err += read_rgbseg_head(s,seg);
  err += read_rgbseg_z(s,seg);
  err += read_rgbseg_midpoint(s,seg);
  err += read_rgbseg_rgb(s,seg);

  return err;
}

static int print_array(unsigned char* b,int n,FILE* s,const char* fmt)
{
  int i;
  
  for (i=0 ; i<n ; i++) fprintf(s,fmt,(int)b[i]);

  return 1;
}

/* usually 0 0 0 0 -- smoothness parameter? */ 

static int read_rgbseg_head(FILE *s,psp_rgbseg_t* seg)
{
  unsigned char b[4];

  fread(b,1,4,s);

  if ((b[0] != 0) || (b[1] != 0))
    {
      fprintf(stderr,"unusual head found : ");
      print_array(b,4,stderr," %i");
      fprintf(stderr,"\n");

      return 1;
    }

  seg->h1 = 256*b[2]+b[3];

#ifdef DEBUG_H1
  printf("  %i\n",(int)(seg->h1));
#endif

  return 0;
}

/* 
   expect 0 0 0 m 

   m is the midpoint in percentage, though the 
   psp gui seems to limit it to 5 < m < 95
*/

static int read_rgbseg_midpoint(FILE *s,psp_rgbseg_t* seg)
{
  unsigned char b[4];

  fread(b,1,4,s);

  if ((b[0] != 0) || (b[1] != 0) || (b[2] != 0))
    {
      fprintf(stderr,"unusual midpoint block found : ");
      print_array(b,4,stderr," %i");
      fprintf(stderr,"\n");

      return 1;
    }

  seg->midpoint = b[3];

  return 0;
}

/* expect 0 0 x y, interpet xy as an unsigned short */  

static int read_rgbseg_z(FILE *s,psp_rgbseg_t* seg)
{
  unsigned char b[4];

  fread(b,1,4,s);

  if ((b[0] != 0) || (b[1] != 0))
    {  
      int j;
      
      fprintf(stderr,"unusual z octet found : ");
      for (j=0 ; j<4 ; j++) fprintf(stderr," %i",(int)b[j]);
      fprintf(stderr,"\n");

      return 1;
    }
  
  seg->z = b[2]*256 + b[3];

  return 0;
}

/* 
   read colour data, which looks like

     0 0 r r 
     g g b b

   not sure what the zeros are (alpha?) and I dont
   know why the rgb values are repeated. Possibly 
   colour are 2-byte unsigned ints. The initial zeros
   are not the alpha channel.
 */

static int read_rgbseg_rgb(FILE *s,psp_rgbseg_t* seg)
{
  int i;
  unsigned char b[8];

  fread(b,1,8,s);

  for (i=0 ; i<4 ; i++)
    {
      if (b[2*i] != b[2*i+1])
	{
	  int j;

	  fprintf(stderr,"unexpected rgbseg data : ");
	  for (j=0 ; j<8 ; j++) fprintf(stderr," %i",(int)b[j]);
	  fprintf(stderr,"\n");

	  return 1;
	}
    }

  if (b[0] || b[1])
    {
      fprintf(stderr,
	      "unexpected rgbseg initial block : %i %i\n",
	      b[0],b[1]);
      return 1;
    }

  seg->r = b[2];
  seg->g = b[4];
  seg->b = b[6];

  return 0;
}

/* 
   expect 
   
   0 0 
   z 0 
   0 0 
   0 m 
   0 p  
   
   where 

   z is the z-coordinate 0 <= z <=  16
   m is the mid-point    0 <= m <= 100
   p is the opacity      0 <= p <= 256 

   note that the opacity segments do not have a special
   initial form where the z is implicit, the first has
   z=0 and the last has z=16, which seems a rather coarse
   z-range.

   I assume that the final m is unused.
*/

static int read_opseg(FILE *s,psp_opseg_t* seg)
{
  unsigned char b[10];

  fread(b,1,10,s);

  if (b[0] || b[1] || b[3] || b[4] || b[5] || b[6] || b[8])
    {
      int j;

      fprintf(stderr,"unusual opacity segment:");
      for (j=0 ; j<10 ; j++) fprintf(stderr," %i",(int)b[j]);
      fprintf(stderr,"\n");
    }

  seg->z        = b[2];
  seg->midpoint = b[7];
  seg->opacity  = b[9];

#ifdef DEBUG_OPGRAD
  printf("  %.2i %.3i %.3i\n",(int)b[2],(int)b[7],(int)b[9]);
#endif

  return 0;
}

#ifdef TESTPROG

/* test program */

int main (int argc,char** argv)
{
  FILE *s;
  char *file = argv[1];
  psp_grad_t grad;
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

  if (read_psp(s,&grad) != 0)
    {
      fprintf(stderr,"failed to read %s\n",file);
      return EXIT_FAILURE;
    }

  fclose(s);

  printf("# %s\n",grad.name);
  for (i=0 ; i<grad.n ; i++)
    {
      psp_rgbseg_t seg = grad.seg[i];

      printf("%.4i %.3i %.3i %.3i\n",
	     seg.z,
	     seg.r,
	     seg.g,
	     seg.b);
    }

  return EXIT_SUCCESS;
}

#endif
