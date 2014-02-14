/*
  svgread.c

  reads svg gradient files - a list of svg_t structs is 
  returned (since a single svg file may contain several 
  svg gradients)

  J.J. Green 2005
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "colour.h"
#include "stdcol.h"

#include "svgread.h"

static int svg_read_lingrads(xmlNodeSetPtr,svg_list_t*);

extern int svg_read(const char* file, svg_list_t* list)
{
  int err = 0;
  xmlDocPtr doc;

  xmlInitParser();

  /* Load XML document */

  if ((doc = xmlParseFile(file)) == NULL)
    {
      fprintf(stderr, "error: unable to parse file %s\n",file);
      err = 1;
    }
  else
    {
      xmlXPathContextPtr xpc; 

      /* Create xpath evaluation context */
   
      if ((xpc = xmlXPathNewContext(doc)) == NULL) 
	{
	  fprintf(stderr,"error: unable to create new XPath context\n");
	  err = 1;
	}
      else
	{
	  /*
	    register the svg namespace -- often svg images use the default
	    svg namespace

              <svg xmlns="http://www.w3.org/2000/svg" ...> 

	    so that each unprefixed child element has an implicit svg: prefix,
	    and this needs to be accounted for in the xpath specification.

	    This means we need to register the namespace with the xpath
	    context.
	  */

	  const unsigned char 
	    prefix[] = "svg",
	    href[]   = "http://www.w3.org/2000/svg";
	  
	  if (xmlXPathRegisterNs(xpc,prefix,href) != 0)
	    {
	      fprintf(stderr,"namespace error for %s:%s\n", prefix, href);
	      err = 1;
	    }
	  else
	    {
	      /*
		handle both linearGradient nodes in the svg namespace (common
		with images containing gradients) and in no namespace (usual
		in stand-alone gradients).

		in fact we do radialGradient at the same time since all we
		are looking for are the stops
	      */

	      const xmlChar xpe[] = 
		"//linearGradient | "
		"//svg:linearGradient | "
		"//radialGradient | "
		"//svg:radialGradient";

	      xmlXPathObjectPtr xpo; 

	      /* evaluate xpath expression */
	  
	      if ((xpo = xmlXPathEvalExpression(xpe, xpc)) == NULL) 
		{
		  fprintf(stderr,"unable to evaluate xpath expression %s\n",xpe);
		  err = 1;
		}
	      else
		{
		  /* process results */
		  
		  err = svg_read_lingrads(xpo->nodesetval,list);
		 		  
		  xmlXPathFreeObject(xpo);
		}
	    }
	  xmlXPathFreeContext(xpc);  
	}
      xmlFreeDoc(doc); 
    }

  xmlCleanupParser();
    
  return err;
}

static int svg_read_lingrad(xmlNodePtr, svg_t*); 

/*
  the first argument is a list of pointers to svg linearGradient
  nodes, the second our svg_list_t struct.

  we traverse the list, check that the linearGradient has an id
  attribute (it must) and if so fetch an svg_t from the svg_list_t
  and call svg_read_lingrad()
*/

static int svg_read_lingrads(xmlNodeSetPtr nodes, svg_list_t* list) 
{
  int   size,i;
    
  size = (nodes ? nodes->nodeNr : 0);

  for (i = 0 ; i<size ; ++i) 
    {
      int err;
      xmlChar *id,*href;
      xmlNodePtr cur;
      svg_t *svg;

      cur = nodes->nodeTab[i];   	    

      assert(cur);
	
      if (cur->type != XML_ELEMENT_NODE)
	{
	  fprintf(stderr,"bad svg: gradient is not a node!\n");
	  return 1;
	}

      /*
	references are used in gradients to share a set of
	stops, and since we are only interested in the stops
	we skip anything with a reference
      */

      if ((href = xmlGetProp(cur,(unsigned char*)"href")) != NULL)
	{
	  xmlFree(href);
	  continue; 
	}

      /*
	just about the only thing we require in the gradient
	is the name, so we check for it before fetching the
	svg from the svg_list
      */

      if ((id = xmlGetProp(cur,(xmlChar *)"id")) == NULL)
	{
	  fprintf(stderr,"gradient has no id attribute, skipping\n");
	  continue; 
	}

      if ((svg = svg_list_svg(list)) == NULL)
	{
	  fprintf(stderr,"failed to get svg object from list\n");
	  return 1;
	}

      svg->nodes = NULL;

      /* so we might as well write it to the svg_t */

      if (xmlStrPrintf(svg->name,
		       SVG_NAME_LEN-1,
		       (xmlChar *)"%s",id) >= SVG_NAME_LEN)
	{
	  fprintf(stderr,"long gradient name truncated!\n");
	}

      xmlFree(id);

      /* now process the gradient */

      err = svg_read_lingrad(cur,svg);

      if (err) return err;
    }

  return 0;
}

static int parse_style(const char*,rgb_t*,double*);
static int parse_offset(const char*,double*);
static int parse_colour(char*,rgb_t*,double*);
static int parse_opacity(char*,double*);

static int svg_read_lingrad(xmlNodePtr lgrad, svg_t* svg) 
{
  xmlNode *nodes,*node;

  /*
    here is where you would read the relevant attributes 
    from the gradient, but we are not interested in them,
    we just want the nodes.
  */

  nodes = lgrad->children;

  for (node = nodes ; node ; node = node->next)
    {
      xmlNode *stop;
      xmlChar *offset;

      if (node->type != XML_ELEMENT_NODE) continue;
	
      if (strcmp((const char*)node->name,"stop") != 0)
	{
	  fprintf(stderr,"unexpected %s node\n",node->name);
	  continue;	  
	}

      /* we have a stop node */

      stop = node;

      /* offset is required */

      if ((offset = xmlGetProp(stop,(const unsigned char*)"offset")) == NULL)
	{
	  fprintf(stderr,"stop has no offset attribute, skipping\n");
	}
      else
	{
	  double z, op = 1.0;
	  rgb_t rgb;

	  if (parse_offset((char*)offset,&z) != 0)
	    {
	      fprintf(stderr,"failed to parse offset \"%s\"\n",offset);
	    }      
	  else
	    {
	      xmlChar *colour,*opacity,*style;
	      svg_stop_t svgstop;

	      if ((colour = xmlGetProp(stop,(const unsigned char*)"stop-color")) != NULL)
		{
		  if (parse_colour((char*)colour, &rgb, &op) != 0)
		    {
		      fprintf(stderr,"failed on bad colour : %s\n",colour);
		      return 1;
		    }

		  xmlFree(colour);
		}

	      if ((style = xmlGetProp(stop,(const unsigned char*)"style")) != NULL)
		{
		  if (parse_style((char*)style, &rgb, &op) != 0)
		    {
		      fprintf(stderr,"error parsing stop style \"%s\"\n",style);
		      return 1;
		    }

		  xmlFree(style);
		}

	      if ((opacity = xmlGetProp(stop,(const unsigned char*)"stop-opacity")) != NULL)
		{
		  if (parse_opacity((char*)opacity,&op) != 0)
		    {
		      fprintf(stderr,"problem parsing opacity \"%s\"\n",opacity);
		      return 1;
		    }

		  xmlFree(opacity);
		}

	      /* need to check we have everything, assume it for now */

	      svgstop.type    = stop_user;
	      svgstop.value   = z;
	      svgstop.opacity = op;
	      svgstop.colour  = rgb;
	      
	      if (svg_append(svgstop, svg) != 0)
		{
		  fprintf(stderr,"failed to insert stop\n");
		  return 1;
		}
	    }
	
	  xmlFree(offset);
	}
    }

  return 0;
}

static int parse_offset(const char *po,double *z)
{
  double x; 

  x = atof(po);

  *z = ((po[strlen(po)-1] == '%') ? x : x*100);

  return 0;
}

/*
  for some crackerjack reason loads of applications write

    <stop offset="0%" style="stop-color:white;stop-opacity:1">

  when

    <stop offset="0%" stop-color="white" stop-opacity="1">

  seems the obvious thing to do. ho hum. 

  This function writes the values in a style string into its
  rgb and opacity argument if they are there, but does not touch 
  them otherwise. Neither case is an error.

  We do a simple tokenisation with strtok_r, rather than 
  introduce a dependency on a css parsing library
*/

static int parse_style_statement(const char*,rgb_t*,double*);

static int parse_style(const char *style,rgb_t* rgb,double* opacity)
{
  size_t sz = strlen(style);
  char buf[sz+1],*st,*state;

  strcpy(buf,style);

  if ((st = strtok_r(buf,";",&state)) != NULL)
    {
      if (parse_style_statement(st,rgb,opacity) != 0)
	{
	  fprintf(stderr,"failed on clause %s\n",st);
	  return 1;
	}

      while ((st = strtok_r(NULL,";",&state)) != NULL)
	{
	  if (parse_style_statement(st,rgb,opacity) != 0)
	    {
	      fprintf(stderr,"failed on clause %s\n",st);
	      return 1;
	    }
	}
    }

  return 0;
}

static int parse_style_statement(const char *stmnt,rgb_t* rgb,double* opacity)
{
  size_t sz = strlen(stmnt);
  char buf[sz+1],*key,*val,*state;

  strcpy(buf,stmnt);

  if ((key = strtok_r(buf,":",&state)) != NULL)
    {
      if ((val = strtok_r(NULL,":",&state)) != NULL)
	{
	  if (strcmp(key,"stop-color") == 0)
	    {
	      if (parse_colour(val,rgb,opacity) != 0)
		{
		  fprintf(stderr,"stop-colour parse failed on %s [%s]\n",val,stmnt);
		  return 1;
		}
	    }
	  else if (strcmp(key,"stop-opacity") == 0)
	    {
	      if (parse_opacity(val,opacity) != 0)
		{
		  fprintf(stderr,"opacity parse failed on %s\n",val);
		  return 1;
		}
	    }
	}
    }

  return 0;
}

/* this one will take some work */

static int parse_colour_numeric1(const char*);
static int parse_colour_numeric2(const char*);

static char* whitespace_trim(char*,int*); 

static int parse_colour(char *st,rgb_t *rgb,double *opacity)
{
  struct stdcol_t *p;
  int r,g,b;
  int n = 0;

  if (st == NULL) return 1;

  st = whitespace_trim(st,&n);

  if (n == 0) return 1;

  if (st[0] == '#')
    {
      switch (n)
	{
	case 4:

	  r = parse_colour_numeric1(st+1);
	  g = parse_colour_numeric1(st+2);
	  b = parse_colour_numeric1(st+3);
	  
	  break;

	case 7:

	  r = parse_colour_numeric2(st+1);
	  g = parse_colour_numeric2(st+3);
	  b = parse_colour_numeric2(st+5);

	  break;

	default:

	  fprintf(stderr,"bad hex colour %s\n",st);

	  return 1;
	}

      if (r<0 || b<0 || g<0)
	{
	  fprintf(stderr,"bad hex colour %s\n",st);

	  return 1;
	}

      rgb->red   = r;
      rgb->green = g;
      rgb->blue  = b;

      return 0;
    }

  if (sscanf(st,"rgb(%i,%i,%i)",&r,&g,&b) == 3)
    {
      rgb->red   = r;
      rgb->green = g;
      rgb->blue  = b;

      return 0;
    }

  if ((p = stdcol(st)) != NULL)
    {
      rgb->red   = p->r;
      rgb->green = p->g;
      rgb->blue  = p->b;

      *opacity = (1.0 - p->t);

      return 0;
    }

  return 1;
}

static char* whitespace_trim(char* st,int* pn)
{
  int i,n = strlen(st);

  for (i=0 ; i<n ; i++)
    {
      switch (st[i])
	{
	case ' ':
	case '\t':
	case '\n':
	  break;
	default:
	  *pn = n;

	  return st + i;
	}
    }

  /* all is whitespace */

  *pn = 0;

  return st+n;
}


static int parse_hex(char);

static int parse_colour_numeric1(const char* st)
{
  int p;

  if ((p = parse_hex(st[0])) < 0) return -1;

  return p*17;
}

static int parse_colour_numeric2(const char* st)
{
  int p,q,v;

  p = parse_hex(st[0]);
  q = parse_hex(st[1]);

  if (p<0 || q<0) return -1;

  v = p*16+q;

  return v;
}

static int parse_hex(char c)
{
  if ('0' <= c && c <= '9') return c - '0';
  if ('a' <= c && c <= 'f') return c - 'a' + 10;
  if ('A' <= c && c <= 'F') return c - 'A' + 10;

  return -1;
}

static int parse_opacity(char *st,double *opacity)
{
  if (st == NULL) return 1;

  *opacity = strtod(st,NULL);

  if (errno) return 1;

  return 0;
}


