/*
  qml.c
  Copyright (c) J.J. Green 2015
*/

#include "qml.h"

extern qml_ramp_t* qml_ramp_new(int type, size_t n)
{
  return NULL;
}

extern void qml_ramp_destroy(qml_ramp_t* qml_ramp)
{

}

/*
  the 'label' part of the entry is duped, so should be freed
  by the caller -- the copy is freed on qml_ramp_destroy
*/

extern int qml_set_entry(qml_ramp_t* qml_ramp, size_t i,
			 qml_entry_t *entry)
{
  return 1;
}
