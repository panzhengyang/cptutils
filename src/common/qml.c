/*
  qml.c
  Copyright (c) J.J. Green 2015
*/

#include <stdlib.h>
#include <string.h>

#include "qml.h"

extern qml_ramp_t* qml_ramp_new(int type, size_t n)
{
  qml_ramp_t *ramp;

  if ((ramp = malloc(sizeof(qml_ramp_t))) != NULL)
    {
      ramp->type = type;

      if ((ramp->entries = calloc(n, sizeof(qml_entry_t))) != NULL)
	{
	  ramp->n = n;
	  return ramp;
	}

      free(ramp);
    }

  return NULL;
}

extern void qml_ramp_destroy(qml_ramp_t* ramp)
{
  for (size_t i = 0 ; i < ramp->n ; i++)
    free(ramp->entries[i].label);

  free(ramp->entries);
  free(ramp);
}

extern int qml_set_entry(qml_ramp_t* ramp, size_t i, qml_entry_t *entry)
{
  qml_entry_t *dest = ramp->entries + i;

  memcpy(dest, entry, sizeof(qml_entry_t));

  if (entry->label != NULL)
    {
      if ((dest->label = strdup(entry->label)) == NULL)
	return 1;
    }

  return 0;
}
