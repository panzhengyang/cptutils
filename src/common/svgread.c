/*
  svgread.c

  reads svg gradient files - a list of svg_t structs is 
  returned (since a single svg file may contain several 
  svg gradients)

  $Id: svgread.c,v 1.1 2005/06/15 22:41:45 jjg Exp jjg $
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

#include "svgread.h"

static int svg_read_lingrads(xmlNodeSetPtr,svg_list_t*);

extern int svg_read(const char* file,svg_list_t* list)
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

	  const char 
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
	      */

	      const xmlChar xpe[] = "//linearGradient | //svg:linearGradient";
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

static int svg_read_lingrad(xmlNodePtr,svg_t*); 

/*
  the first argument is a list of pointers to svg linearGradient
  nodes, the second our svg_list_t struct.

  we traverse the list, check that the linearGradient has an id
  attribute (it must) and if so fetch an svg_t from the svg_list_t
  and call svg_read_lingrad()
*/

static int svg_read_lingrads(xmlNodeSetPtr nodes,svg_list_t* list) 
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
	we dont do references
      */

      if ((href = xmlGetProp(cur,"href")) != NULL)
	{
	  /* this needs testing */

	  fprintf(stderr,"gradient has references, skipping\n");
	  xmlFree(href);

	  continue; 
	}

      /*
	just about the only thing we require in the gradient
	is the name, so we check for it before fetching the
	svg from the svg_list
      */

      if ((id = xmlGetProp(cur,"id")) == NULL)
	{
	  fprintf(stderr,"gradient has no id attribute, skipping\n");
	  continue; 
	}

      if ((svg = svg_list_svg(list)) == NULL)
	{
	  fprintf(stderr,"failed to get svg object from list\n");
	  return 1;
	}

      /* so we might as well write it to the svg_t */

      if (snprintf(svg->name,SVG_NAME_LEN,"%s",id) >= SVG_NAME_LEN)
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

static int svg_read_lingrad(xmlNodePtr lgrad,svg_t* svg) 
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
	
      if (strcmp(node->name,"stop") != 0)
	{
	  fprintf(stderr,"unexpected %s node\n",node->name);
	  continue;	  
	}

      /* we have a stop node */

      stop = node;

      /* get offset */

      if ((offset = xmlGetProp(stop,"offset")) == NULL)
	{
	  fprintf(stderr,"gradient has no id attribute, skipping\n");
	}
      else
	{
	  double z,op;
	  rgb_t rgb;

	  if (parse_offset(offset,&z) != 0)
	    {
	      fprintf(stderr,"failed to parse offset \"%s\"\n",offset);
	    }      
	  else
	    {
	      xmlChar *colour,*opacity,*style;

	      printf("offset %f\n",z);

	      if ((colour = xmlGetProp(stop,"stop-color")) != NULL)
		{
		  /* parse colour */

		  xmlFree(colour);
		}

	      if ((opacity = xmlGetProp(stop,"stop-opacity")) != NULL)
		{
		  /* parse opacity */

		  xmlFree(opacity);
		}

	      if ((style = xmlGetProp(stop,"style")) != NULL)
		{
		  printf("style %s\n",style);

		  if (parse_style(style,&rgb,&op) != 0)
		    {
		      fprintf(stderr,"error parsing stop style %s\n",style);
		      return 1;
		    }

		  xmlFree(style);
		}

	      printf("opacity = %.5f\n",op);
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

  *z = ((po[strlen(po)-1] == '%') ? x/100.0 : x);

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
  introduce a dependancy on a css paring library
*/

static int parse_style_statement(const char*,rgb_t*,double*);

static int parse_style(const char *style,rgb_t* rgb,double* opacity)
{
  size_t sz = strlen(style);
  char buf[sz],*st,*state;

  strcpy(buf,style);

  if ((st = strtok_r(buf,";",&state)) != NULL)
    {
      if (parse_style_statement(st,rgb,opacity) != 0) return 1;

      while ((st = strtok_r(NULL,";",&state)) != NULL)
	{
	  if (parse_style_statement(st,rgb,opacity) != 0) return 1;
	}
    }

  return 0;
}

static int parse_colour(char*,rgb_t*);
static int parse_opacity(char*,double*);

static int parse_style_statement(const char *stmnt,rgb_t* rgb,double* opacity)
{
  size_t sz = strlen(stmnt);
  char buf[sz],*key,*val,*state;

  strcpy(buf,stmnt);

  if ((key = strtok_r(buf,":",&state)) != NULL)
    {
      if ((val = strtok_r(NULL,":",&state)) != NULL)
	{
	  if (strcmp(key,"stop-color") == 0)
	    {
	      if (parse_colour(val,rgb) != 0) 
		return 1;
	    }
	  else if (strcmp(key,"stop-opacity") == 0)
	    {
	      if (parse_opacity(val,opacity) != 0) 
		return 1;
	    }

	  printf("  %s => %s\n",key,val);
	}
    }

  return 0;
}

/* this one will take some work */

static int parse_colour(char *st,rgb_t *rgb)
{
  return 0;
}

static int parse_opacity(char *st,double *opacity)
{
  if (st == NULL) return 1;

  *opacity = strtod(st,NULL);

  if (errno) return 1;

  return 0;
}


