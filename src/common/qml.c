/*
  qml.c
  Copyright (c) J.J. Green 2015
*/

#include <stdlib.h>
#include <string.h>

#include "qml.h"

extern qml_t* qml_new(int type, size_t n)
{
  qml_t *qml;

  if ((qml = malloc(sizeof(qml_t))) != NULL)
    {
      qml->type = type;

      if ((qml->entries = calloc(n, sizeof(qml_entry_t))) != NULL)
	{
	  qml->n = n;
	  return qml;
	}

      free(qml);
    }

  return NULL;
}

extern void qml_destroy(qml_t* qml)
{
  for (size_t i = 0 ; i < qml->n ; i++)
    free(qml->entries[i].label);

  free(qml->entries);
  free(qml);
}

extern int qml_set_entry(qml_t* qml, size_t i, qml_entry_t *entry)
{
  if (i >= qml->n) return 1;

  qml_entry_t *dest = qml->entries + i;

  memcpy(dest, entry, sizeof(qml_entry_t));

  if (entry->label != NULL)
    {
      if ((dest->label = strdup(entry->label)) == NULL)
	return 1;
    }

  return 0;
}
