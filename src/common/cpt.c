/*
  cpt.h

  A struct to hold a GMT cpt file, and some operations 
  on theml

  (c) J.J.Green 2001
  $Id: cpt.c,v 1.3 2004/02/11 00:58:48 jjg Exp jjg $
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "colour.h"
#include "cpt.h"
#include "version.h"

static int print_cpt_aux(FILE*,char,rgb_t);

extern cpt_t* cpt_new(void)
{
    cpt_t* cpt;

    if ((cpt = malloc(sizeof(cpt_t))) != NULL)
    {
	cpt->segment=NULL;
    }

    return cpt;
}

extern int cpt_prepend(cpt_seg_t* seg,cpt_t* cpt)
{
    seg->lseg = NULL;
    seg->rseg = cpt->segment;

    cpt->segment = seg;

    return 0;
}

extern int cpt_append(cpt_seg_t* seg,cpt_t* cpt)
{
    cpt_seg_t* s;

    s = cpt->segment;

    if (!s) return cpt_prepend(seg,cpt);
 
    while (s->rseg) s = s->rseg;
 
    s->rseg = seg;
    seg->lseg = s;
    seg->rseg = NULL;
 
    return 0;
}

#define LBUF 1024
#define SNM(x) ((x) ? (x) : "<stdin>")

extern int cpt_read(char* file,cpt_t* cpt)
{
  FILE *stream;
  char  lbuf[LBUF];
  int   n;

  if (file)
    {
      char *s,*e,*name;

      if ((stream = fopen(file,"r")) == NULL)
	{
	  fprintf(stderr,"error reading %s : %s\n",SNM(file),strerror(errno));
	  return 1;
	}

      /* get the name from the filename */

      name = cpt->name;

      if ((s = strrchr(file,'/')) == NULL) 
	s = file;
      else
	s++;

      strncpy(name,s,CPT_NAME_LEN);

      /* chop off a trailing .cpt */

      if ((e = strrchr(name,'.')) != NULL)
	{   
	  if (strcmp(e,".cpt") == 0) *e = '\0';
	} 
    }
  else
    {
      stream = stdin;
      strncpy(cpt->name,"<stdin>",CPT_NAME_LEN);
    }

  /* assume rgb colour model */

  cpt->model = rgb;
  
  for (n=1 ; fgets(lbuf,LBUF,stream) ; n++)
    { 
      int lr,lg,lb,rr,rg,rb,gr,gg,gb;
      double lv,rv;
      char gt;

      /* check for comment lines */

      if (*lbuf == '#')
	{
	  char cm[6];

	  /* special comments specify the colour model */

	  if (sscanf(lbuf,"# COLOR_MODEL = %5s",cm) == 1)
	    {
	      if (strcmp(cm,"RGB") == 0)
		cpt->model = rgb;
	      else if (strcmp(cm,"HSV") == 0)
		cpt->model = hsv;
	      else
		{
		  fprintf(stderr,"dont know model %s\n",cm);
		  return 1;
		}
	    }
	  
	  continue;
	}

      /* check for segment line */
      
      if (sscanf(lbuf,
		 "%lf %i %i %i %lf %i %i %i",
		 &lv,&lr,&lg,&lb,
		 &rv,&rr,&rg,&rb
		 ) == 8)
	{
	  cpt_seg_t *seg;

	  seg = cpt_seg_new();

	  seg->lsmp.val       = lv;
	  seg->lsmp.rgb.red   = lr;
	  seg->lsmp.rgb.green = lg;
	  seg->lsmp.rgb.blue  = lb;

	  seg->rsmp.val       = rv;
	  seg->rsmp.rgb.red   = rr;
	  seg->rsmp.rgb.green = rg;
	  seg->rsmp.rgb.blue  = rb;

	  if (cpt_append(seg,cpt) != 0)
	    {
	      fprintf(stderr,"failed seg append");
	      return 1;
	    }

	  continue;
	}	    

      /* get background etc */

      if (sscanf(lbuf,"%c %i %i %i",&gt,&gr,&gg,&gb) == 4)
	{
	  switch (gt)
	    {
	    case 'F':
	    case 'f':
	      cpt->fg.red   = gr;
	      cpt->fg.green = gg;
	      cpt->fg.blue  = gb;
	      break;

	    case 'B':
	    case 'b':
	      cpt->bg.red   = gr;
	      cpt->bg.green = gg;
	      cpt->bg.blue  = gb;
	      break;

	    case 'N':
	    case 'n':
	      cpt->nan.red   = gr;
	      cpt->nan.green = gg;
	      cpt->nan.blue  = gb;
	      break;

	    default:
	      fprintf(stderr,"strange FBN at line %i\n",n);
	      return 1;
	    }

	  continue;
	}

      /* everthing else we ignore */

      fprintf(stderr,"failed to scan line %i",n);
    }

  if (!feof(stream))
    {
      if (ferror(stream))
	{
	  fprintf(stderr,"error reading %s\n",SNM(file));
	  return 1;
	}
      else
	{
	  fprintf(stderr,"wierd error reading %s\n",SNM(file));
	}
    }

  if (stream != stdin) 
    fclose(stream);

  return 0;
}

extern int cpt_write(char* outfile,cpt_t* cpt)
{
    FILE       *stream;
    cpt_seg_t  *seg;
    const char *model;
    time_t      t;

    if (outfile)
    {
	stream = fopen(outfile,"wb");
	if (!stream) return 1;
    }
    else stream = stdout;

    t = time(NULL);

    switch (cpt->model)
      {
      case rgb : model = "RGB"; break;
      case hsv : model = "HSV"; break;
      default :
	fprintf(stderr,"cpt file corrupt : bad model specified\n");
	return 1;
      }

    fprintf(stream,"# %s\n",(outfile ? outfile : "<stdout>"));
    fprintf(stream,"# autogenerated GMT palette \"%s\"\n",(cpt->name ? cpt->name : ""));
    fprintf(stream,"# libgimpcpt version %s, %s",VERSION,ctime(&t));
    fprintf(stream,"# COLOR_MODEL = %s\n",model);

    seg = cpt->segment;

    while (seg)
    {
	cpt_sample_t l,r;

	l = seg->lsmp;
	r = seg->rsmp;
	    
	fprintf(stream,
		"%#7e %3i %3i %3i %#7e %3i %3i %3i\n",
		l.val,
		l.rgb.red,
		l.rgb.green,
		l.rgb.blue,
		r.val,
		r.rgb.red,
		r.rgb.green,
		r.rgb.blue);
	
	seg = seg->rseg;
    }

    print_cpt_aux(stream,'B',cpt->bg);
    print_cpt_aux(stream,'F',cpt->fg);
    print_cpt_aux(stream,'N',cpt->nan);

    fclose(stream);

    return 0;
}

static int print_cpt_aux(FILE* stream,char c,rgb_t col)
{
    fprintf(stream,"%c %3i %3i %3i\n",
	    c, col.red, col.green, col.blue);

    return 1;
}

extern void cpt_destroy(cpt_t* cpt)
{
    cpt_seg_t *seg, *next;

    if (!cpt) return;
    
    if ((seg = cpt->segment) == NULL) return;

    while (seg) 
    {
	next = seg->rseg;
	cpt_seg_destroy(seg);
	seg = next;
    }
	    
    free(cpt);

    return;
}

extern cpt_seg_t* cpt_seg_new(void)
{
    cpt_seg_t *seg;

    if ((seg = malloc(sizeof(cpt_seg_t))) == NULL) 
	return NULL;

    seg->lseg = NULL;
    seg->rseg = NULL;

    return seg;
}

extern void cpt_seg_destroy(cpt_seg_t* seg)
{
    free(seg);
}



