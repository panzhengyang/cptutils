/*
  psp.c

  read photoshop pro gradients.
  2005 (c) J.J. Green
  $Id: psp.c,v 1.2 2005/01/27 21:32:11 jjg Exp jjg $
*/

#include <stdio.h>

#include "psp.h"

#define SKIP(stream,n) fseek((stream),(n),SEEK_CUR)

static int read_first_segment(FILE*,psp_seg_t*);
static int read_segment(FILE*,psp_seg_t*);
static int read_footer(FILE*);

extern int read_psp(FILE* s,psp_grad_t* grad)
{
  unsigned char b[4],magic[4] = "8BGR";
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

  /* next 4 looks like a format version - 00 03 00 01 */

  SKIP(s,4);

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
    
  /* then an unsigned char, the number of samples */

  n = fgetc(s);

  if (n<2)
    {
      fprintf(stderr,"too few samples (%i)\n",n);
      return 1;
    }
  else 
    {
      int j, err = 0;
      psp_seg_t* seg;

      if ((seg = malloc(n*sizeof(psp_seg_t))) == NULL)
	{
	  fprintf(stderr,"failed malloc()");
	  return 1;
	}
      
      err += read_first_segment(s,seg);

      for (j=1 ; j<n ; j++) 
	err += read_segment(s,seg+j);

      if (err) return err;

      grad->n   = n;
      grad->seg = seg;
    }

  if (read_footer(s) != 0)
    {
      fprintf(stderr,"error reading footer\n");
      return 1;
    }

  return 0;
}

extern int clean_psp(psp_grad_t* grad)
{
  free(grad->name);
  free(grad->seg);

  return 0;
}


/*
  In all but the first segment, the header seems to be

    0 0 0 0  : h1 
    0 0 x y  : val
    0 0 0 50 : h2

  with (x,y) interpreted as the z-value, we take it to 
  mean a big endian unsigned short, so (x,y) -> 256*x+y.

  In the first segment the value octet is absent, and so
  we assume an implicit value of zero.

  In all files so far tested, the value for the final
  segment is 4096 (16,0)
*/

static int read_segment_h1(FILE*,psp_seg_t*);
static int read_segment_h2(FILE*,psp_seg_t*);
static int read_segment_pos(FILE*,psp_seg_t*);
static int read_segment_data(FILE*,psp_seg_t*);

static int read_first_segment(FILE *s,psp_seg_t* seg)
{
  int err = 0;

  err += read_segment_h1(s,seg);
  err += read_segment_h2(s,seg);
  err += read_segment_data(s,seg);

  if (err) return err;

  seg->z = 0;

  return 0;
}

static int read_segment(FILE *s,psp_seg_t* seg)
{
  int err = 0;

  err += read_segment_h1(s,seg);
  err += read_segment_pos(s,seg);
  err += read_segment_h2(s,seg);
  err += read_segment_data(s,seg);

  return err;
}

static int print_array(unsigned char* b,int n,FILE* s,const char* fmt)
{
  int i;
  
  for (i=0 ; i<n ; i++) fprintf(s,fmt,(int)b[i]);

  return 1;
}

/* usually 0 0 0 0 -- smoothness parameter? */ 

static int read_segment_h1(FILE *s,psp_seg_t* seg)
{
  unsigned char b[4];

  fread(b,1,4,s);

  if ((b[0] != 0) || (b[1] != 0))
    {
      fprintf(stderr,"unusual h1 found : ");
      print_array(b,4,stderr," %i");
      fprintf(stderr,"\n");

      return 1;
    }

  seg->h1 = 256*b[2]+b[3];

  return 0;
}

/* expect 0 0 0 50 */

static int read_segment_h2(FILE *s,psp_seg_t* seg)
{
  unsigned char b[4];

  fread(b,1,4,s);

  if ((b[0] != 0) || (b[1] != 0) || (b[2] != 0))
    {
      fprintf(stderr,"unusual h2 found : ");
      print_array(b,4,stderr," %i");
      fprintf(stderr,"\n");

      return 1;
    }

  seg->h2 = b[3];
  
  return 0;
}

/* expect 0 0 x y, interpet xy as an unsigned short */  

static int read_segment_pos(FILE *s,psp_seg_t* seg)
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
   know why the rgb values are repeated.
 */

static int read_segment_data(FILE *s,psp_seg_t* seg)
{
  int i;
  unsigned char b[8];

  fread(b,1,8,s);

  for (i=0 ; i<4 ; i++)
    {
      if (b[2*i] != b[2*i+1])
	{
	  int j;

	  fprintf(stderr,"unexpected segment data : ");
	  for (j=0 ; j<8 ; j++) fprintf(stderr," %i",(int)b[j]);
	  fprintf(stderr,"\n");

	  return 1;
	}
    }

  seg->a = b[0];
  seg->r = b[2];
  seg->g = b[4];
  seg->b = b[6];

  return 0;
}

/* 
   the footer -- not sure what is in here, possibly
   - the alpha channel (but then what is the first 
     pair of chars in the segment data?)
   - the angle of the gradient
   - the type (linear, circular,..)
   - the number of repeats
   ???

   The first would be nice to work out (so we could have
   alpha translation to convert to ggr) but not so much
   for the cpt conversion. The second to are specific to
   the painting/display of art gradients -- we just want
   the colour path. The last would be nice (but we would
   leave it up to calling function whether to implement 
   it)
*/

#ifdef DEBUG

static int read_footer(FILE *s)
{
  unsigned char b[4];

  while (fread(b,1,4,s))
    {
      int i;

      for (i=0 ; i<4 ; i++)
	     printf("%.3i ",(int)b[i]);

      printf("%s",(*((int*)b)) ? "" : "*");

      printf("\n");
    }

  return 0;
}

#else

static int read_footer(FILE *s)
{
  return 0;
}

#endif

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
      psp_seg_t seg = grad.seg[i];

      printf("%.4i %.3i %.3i %.3i\n",
	     seg.z,
	     seg.r,
	     seg.g,
	     seg.b);
    }

  return EXIT_SUCCESS;
}

#endif
