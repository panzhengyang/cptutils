/*
  i8n.h

  common internationalisation macros, as advised in
  http://www.gnu.org/software/gettext/manual/gettext.html

  Copyright (c) J.J. Green 2012
  $Id$
*/

#ifndef I8N_H
#define I8N_H

#if 1

#define _(String) (String)
#define N_(String) String
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)

#else

#include <libintl.h>
#define _(String) gettext(String)
#define gettext_noop(String) String
#define N_(String) gettext_noop(String)

#endif

#endif
