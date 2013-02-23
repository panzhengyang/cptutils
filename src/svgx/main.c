/*
  main.c 

  part of the cptutils package

  This program is free software; you can redistribute it
  and/or modify it under the terms of the GNU General
  Public License as published by the Free Software
  Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be
  useful, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A
  PARTICULAR PURIGHTE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public
  License along with this program; if not, write to the
  Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, 
  Boston, MA 02110-1301 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "options.h"
#include "svgx.h"

int main(int argc,char** argv)
{
  struct gengetopt_args_info info;
  svgx_opt_t opt;
  char *infile,*outfile;
  int err;

  /* use gengetopt */

  if (options(argc,argv,&info) != 0)
    {
      fprintf(stderr,"failed to parse command line\n");
      return EXIT_FAILURE;
    }

  /* check arguments & transfer to opt structure */ 

  opt.verbose    = info.verbose_flag;
  opt.permissive = ! info.strict_flag;
  opt.name       = NULL;

  if (info.all_given + info.list_given + info.select_given > 1)
    {
      fprintf(stderr,"only one of --all, --list and --select allowed\n");
      return EXIT_FAILURE;
    }

  /* job type */

  if (info.all_flag)
    opt.job = job_all;
  else if (info.list_flag)  
    opt.job = job_list;
  else if (info.select_given)
    {
      opt.job = job_named;
      opt.name = info.select_arg;
    }
  else
    opt.job = job_first;

  /* output formar */

  if (info.type_given)
    {
      const char* name = info.type_arg;

      struct {const char* name; int type; } 
      *p, types[] =
	{
	  {"cpt", type_cpt},
	  {"ggr", type_ggr},
	  {"gimp",type_ggr},
	  {"inc", type_pov},
	  {"pov", type_pov},
	  {"css3", type_css3},
	  {"c3g", type_css3},
	  {"gpt", type_gpt},
	  {"gnuplot", type_gpt},
	  {"psp", type_psp},
	  {"PspGradient", type_psp},
	  {"jgd", type_psp},
	  {"grd", type_psp},
	  {"sao", type_sao},
	  {"ds9", type_sao},
	  {"png", type_png},
	  {"svg", type_svg},
	  {NULL, 0}};

      for (p = types ; ; p++)
	{
	  if (! p->name )
	    {
	      fprintf(stderr,"output type %s not understood\n",name);

	      fprintf(stderr,"supported types:\n");
	      for (p = types ; p->name ; p++)
		fprintf(stderr,"  %s\n",p->name);
	      fprintf(stderr,"\n");

	      options_print_help();

	      return EXIT_FAILURE;
	    }

	  if (strcmp(name,p->name) == 0)
	    {
	      opt.type = p->type;
	      break;
	    }
	}
    }
  else
    opt.type = type_cpt;

  /* null outfile for stdout */

  outfile = (info.output_given ? info.output_arg : NULL);

  /* null infile for stdin */

  switch (info.inputs_num)
    {
    case 1:
      infile = info.inputs[0];
      break;
    default:
      fprintf(stderr,"Exactly one SVG file must be specified!\n");
      options_print_help();
      return EXIT_FAILURE; 
    }
  
  opt.input.file  = infile;
  opt.output.file = outfile;

  /* fg/bg/nan for cpt output */

  if (parse_rgb(info.background_arg,&(opt.format.cpt.bg)) != 0)
    {
      fprintf(stderr,"bad background %s\n",info.background_arg);
      return EXIT_FAILURE;
    }

  if (parse_rgb(info.foreground_arg,&(opt.format.cpt.fg)) != 0)
    {
      fprintf(stderr,"bad foreground %s\n",info.foreground_arg);
      return EXIT_FAILURE;
    }

  if (parse_rgb(info.nan_arg,&(opt.format.cpt.nan)) != 0)
    {
      fprintf(stderr,"bad nan colour %s\n",info.nan_arg);
      return EXIT_FAILURE;
    }

  /* colour to replace transparency */

  if (parse_rgb(info.transparency_arg, &(opt.format.alpha)) != 0)
    {
      fprintf(stderr,"bad transparency colour %s\n",info.nan_arg);
      return EXIT_FAILURE;
    }

  /* parse image (preview) size for png, svg output */

  if (info.geometry_given)
    {
      switch (opt.type)
	{
	case type_png:
	case type_svg:
	  break;

	default:
	  fprintf(stderr,"geometry option not valid for this output type\n");
	  return EXIT_FAILURE;
	}
    }
  
  switch (opt.type)
    {
    case type_png:

      if (sscanf(info.geometry_arg,"%zux%zu",
		 &opt.format.png.width,
		 &opt.format.png.height) != 2)
	{
	  fprintf(stderr,"bad argument \"%s\" to geometry option",
		  info.geometry_arg);
	  return EXIT_FAILURE;
	}
      break;

    case type_svg:

      if (info.preview_flag)
	{
	  opt.format.svg.preview.use = true;
	  if (svg_preview_geometry(info.geometry_arg, 
				   &(opt.format.svg.preview)) != 0)
	    {
	      fprintf(stderr,"bad argument \"%s\" to geometry option",
		      info.geometry_arg);
	      return EXIT_FAILURE;
	    }
	}
      else
	opt.format.svg.preview.use = false;

      break;

    default:
      break;
    }

  /* 
     we write the translation of the svg gradient <name> to stdout 
     if <name> is specified, so then we suppress verbosity
  */

  if (opt.name && !outfile && opt.verbose)
   {
      fprintf(stderr,"verbosity suppressed (<stdout> for results)\n");
      opt.verbose = 0;
    }

  /* say hello */

  if (opt.verbose)
      printf("This is svgx (version %s)\n",VERSION);

  /* for conversion, give details of what we will do */

  if (opt.verbose && (opt.job != job_list))
    {
      const char* tstr;

      switch (opt.type)
	{
	case type_cpt  : tstr = "GMT colour palette table"; break;
	case type_ggr  : tstr = "GIMP gradient"; break;
	case type_pov  : tstr = "POV-Ray colour map"; break;
	case type_gpt  : tstr = "Gnuplot colour map"; break;
	case type_css3 : tstr = "CSS3 gradient"; break;
	case type_psp  : tstr = "grd v3"; break;
	case type_sao  : tstr = "SAO (DS9) colour map"; break;
	case type_png  : tstr = "png image"; break;
	case type_svg  : tstr = "SVG gradient"; break;
	  
	default:
	  fprintf(stderr,"weird output format!\n");
	  return EXIT_FAILURE;
	}
	  
      printf("convert svg to %s\n",tstr);
      printf("%s format limits\n",
		 (opt.permissive ? "ignoring" : "respecting"));

      if (opt.type == type_png)
	printf("png output size is %zu x %zu px\n", 
	       opt.format.png.width, 
	       opt.format.png.height);
      else if ( (opt.type == type_svg) && 
		(opt.format.svg.preview.use) )
	printf("svg preview size is %zu x %zu px\n",
	       opt.format.svg.preview.width,
	       opt.format.svg.preview.height);
    }

  err = svgx(opt);

  if (err) fprintf(stderr,"failed (error %i)\n",err);

  if (opt.verbose) printf("done.\n");

  return (err ? EXIT_FAILURE : EXIT_SUCCESS);
}
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	
    	

