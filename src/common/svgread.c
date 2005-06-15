/*
  svgread.c

  reads svg gradient files - a list of svg_t structs is 
  returned (since a single svg file may contain several 
  svg gradients)

  $Id$
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

typedef struct state_t
{
} state_t;

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

static int parse_offset(const char*,double*); 

static int parse_style_ini(state_t*);
static int parse_style(state_t*,const char*,rgb_t*);
static int parse_style_free(state_t*);

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
      xmlChar *po;
      xmlNodePtr cur;
      svg_t *svg;

      cur = nodes->nodeTab[i];   	    

      assert(cur);
	
      if (cur->type != XML_ELEMENT_NODE)
	{
	  fprintf(stderr,"not a node!\n");
	  continue; 
	}

      if ((po = xmlGetProp(cur,"id")) == NULL)
	{
	  fprintf(stderr,"node has no id attribute\n");
	  continue; 
	}

      printf(" - %s\n",po);
 
      if ((svg = svg_list_svg(list)) == NULL)
	{
	  fprintf(stderr,"failed to get svg object from list\n");
	  return 1;
	}

      err = svg_read_lingrad(cur,svg);

      xmlFree(po);

      if (err) return err;
    }

  return 0;
}

static int svg_read_lingrad(xmlNodePtr node,svg_t* svg) 
{
  return 0;

  /*
  int   size,i;
  state_t state;

  size = (nodes ? nodes->nodeNr : 0);

  if (parse_style_ini(&state) != 0)
    {
      fprintf(stderr,"failed to initialise parse state\n");
      return 1;
    }

  for (i = 0; i < size; ++i) 
    {
      xmlChar *po,*ps;
      xmlNodePtr cur;
      double z;
      rgb_t rgb;

      assert(nodes->nodeTab[i]);
	
      if (nodes->nodeTab[i]->type != XML_ELEMENT_NODE)
	{
	  fprintf(stderr,"not a node!\n");
	  continue; 
	}

      cur = nodes->nodeTab[i];   	    
      if (cur->ns)
	{ 
	  fprintf(stderr,"element node %s:%s!\n", 
		  cur->ns->href, cur->name); 
	  continue; 
	} 
	      
      if ((po = xmlGetProp(cur,"offset")) == NULL)
	{
	  fprintf(stderr,"node has no offset attribute\n");
	  continue; 
	}
      
      if ((ps = xmlGetProp(cur,"style")) == NULL)
	{
	  fprintf(stderr,"node has no style attribute\n");
	  continue; 
	}
	
      if (parse_offset(po,&z) != 0)
	{
	  fprintf(stderr,"failed to parse offset \"%s\"\n",po);
	  continue; 
	}      

      if (parse_style(&state,ps,&rgb) != 0)
	{
	  fprintf(stderr,"failed to parse style \"%s\"\n",ps);
	  continue; 
	}      

      printf("%s -> %f, %s\n",po,z,ps);
    }

  if (parse_style_free(&state) != 0)
    {
      fprintf(stderr,"failed to free parse state\n");
      return 1;
    }

  return 0;
 */
}

static int parse_offset(const char *po,double *z)
{
  double x; 

  x = atof(po);

  *z = ((po[strlen(po)-1] == '%') ? x/100.0 : x);

  return 0;
}

static int parse_style_ini(state_t* state)
{
  return 0;
}

static int parse_style(state_t* state,const char *po,rgb_t* rgb)
{
  size_t sz = strlen(po);
  char buf[sz];

  /* 
     copy the cost string to a modifiable buffer
  */

  strcpy(buf,po);

  return 0;
}

static int parse_style_free(state_t* state)
{
  return 0;
}
