/*
  svgcpt.c : convert svg file to cpt file
 
  This file is a derived work from the file xpath1.c, an example
  distributed with the libxml2 library. The modifications are 

    Copyright (c) J.J. Green 2004.
    $Id$

  The original file header follows (retained for reference : the information
  below is mostly wrong/inapproriate

  -------
  section: 	XPath
  synopsis: 	Evaluate XPath expression and prints result node set.
  purpose: 	Shows how to evaluate XPath expression and register 
           	known namespaces in XPath context.
  usage:	xpath1 <xml-file> <xpath-expr> [<known-ns-list>]
  test: ./xpath1 test3.xml '//child2' > xpath1.tmp ; diff xpath1.tmp xpath1.res ; rm xpath1.tmp
  author: 	Aleksey Sanin
  copy: 	see Copyright for the status of this software.
  -------
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

#include "svgcpt.h"

typedef int (*mapfn_t)(xmlNodeSetPtr,void*);

typedef struct mapfn_arg_t 
{
  FILE *stream;
}  mapfn_arg_t;

static int map_xpath(const char*,const xmlChar*,mapfn_t,void*);
static int xpath_nodes(xmlNodeSetPtr,mapfn_arg_t*);

extern int svgcpt(svgcpt_opt_t opt)
{
  int err=0;
  FILE *stream;
  mapfn_arg_t arg;
  const char xpath[] = "/svg/linearGradient/stop";

  xmlInitParser();
  LIBXML_TEST_VERSION;
  
  if (opt.file.output)
    {
      stream = fopen(opt.file.output,"w");
      if (stream == NULL)
	{
	  fprintf(stderr,"error opening file %s : %s\n",
		  opt.file.output,
		  strerror(errno));
	  err = 1;
	}
      else
	{
	  arg.stream = stream;
	  err = map_xpath(opt.file.input, 
			  BAD_CAST xpath,
			  (mapfn_t)xpath_nodes,
			  (void*)&arg); 

	  fclose(stream);
	}
    }
  else
    {
      arg.stream = stdout;
      err = map_xpath(opt.file.input, 
		      BAD_CAST xpath,
		      (mapfn_t)xpath_nodes,
		      (void*)&arg);
    } 

  xmlCleanupParser();

  return err;
}

static int map_xpath(const char* name, const xmlChar* xpe,mapfn_t f,void *arg) 
{
  int err = 0;
  xmlDocPtr doc;
  
  assert(name);
  assert(xpe);
  
  /* Load XML document */

  if ((doc = xmlParseFile(name)) == NULL)
    {
      fprintf(stderr, "error: unable to parse file \"%s\"\n",name);
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
	  xmlXPathObjectPtr xpo; 

	  /* Evaluate xpath expression */
	  
	  if ((xpo = xmlXPathEvalExpression(xpe, xpc)) == NULL) 
	    {
	      fprintf(stderr,"error: unable to evaluate xpath expression \"%s\"\n", xpe);
	      err = 1;
	    }
	  else
	    {
	      err = f(xpo->nodesetval,arg);
	      xmlXPathFreeObject(xpo);
	    }
	  xmlXPathFreeContext(xpc); 
	}
      xmlFreeDoc(doc); 
    }
    
  return err;
}

static int xpath_nodes(xmlNodeSetPtr nodes, mapfn_arg_t *arg) 
{
  int   size,i;
  FILE *stream;
    
  assert(arg);

  stream = arg->stream;

  assert(stream);

  size = (nodes ? nodes->nodeNr : 0);

  fprintf(stream, "# svgcpt output (%d nodes)\n", size);

  for (i = 0; i < size; ++i) 
    {
      xmlChar *po,*ps;
      xmlNodePtr cur;

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
	      
      po = xmlGetProp(cur,"offset");
      ps = xmlGetProp(cur,"style");
	      
      fprintf(stream, "%s %s\n",po,ps);
    }

  return 0;
}

